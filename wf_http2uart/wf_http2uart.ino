/*
 * @author: Polyakov Konstantin
 * 
 */

#define LANG_RU 1
//#define LANG_EN 1

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

ESP8266WebServer webServer(80);

byte pin_btn_reset = 14;

struct DefaultSettings {
    byte wifi_mode = 0; // 0 - точка доступа, 1 - клиент, 2 - оба варианта одновременно

    char ssidAP[32] = "SY_WF_HTTP2UART";
    char passwordAP[32] = "";

    char ssid[32] = "Your name";
    char password[32] = "Your password";

    char device_name[64] = "Device name";
};

// eeprom addresses
const unsigned int ee_addr_start_firstrun = 0;
const unsigned int ee_addr_start_settings = 1;

const byte code_firstrun = 4;
DefaultSettings ee_data;
byte is_wifi_client_connected = 0;

void notFoundHandler() {
    webServer.send(404, "text/html", "<h1>Not found :-(</h1>");
}

void apiHandler() {

    String action = webServer.arg("action");

    JsonDocument answer;
    answer["success"] = 1;
    #if defined(LANG_RU)
        answer["message"] = "Успешно!";
    #elif defined(LANG_EN)
         answer["message"] = "Success!";
    #endif
    JsonObject data = answer["data"].to<JsonObject>();

    if (action == "send_to_serail") {
        String string_data = webServer.arg("string_data");

        #if defined(LANG_RU)
            answer["message"] = "Строка отправлена!";
        #elif defined(LANG_EN)
            answer["message"] = "The string has been send!";
        #endif

        Serial.print(string_data);
        unsigned long start_time = millis();
        while (Serial.available() == 0) {
            if (millis() - start_time >= 5000) {
                answer["message"] = "The string has been send! The device did'nt answer";
                break;
            }
        }
        data["string_response_data"] = Serial.readString();

    } else if (action == "settings_mode") {

        String mode = webServer.arg("wifi_mode");
        if (mode.toInt() > 2 ||mode.toInt() < 0) mode = "0";
        ee_data.wifi_mode = mode.toInt();

        EEPROM.put(ee_addr_start_settings, ee_data);
        EEPROM.commit();

        data["value"] = ee_data.wifi_mode;
        #if defined(LANG_RU)
            answer["message"] = "Сохранено!";
        #elif defined(LANG_EN)
           answer["message"] = "Saved!";
        #endif

    } else if (action == "settings_device_name") {

        String _device_name = webServer.arg("device_name");
        _device_name.toCharArray(ee_data.device_name, sizeof(ee_data.device_name));

        EEPROM.put(ee_addr_start_settings, ee_data);
        EEPROM.commit();

        #if defined(LANG_RU)
            answer["message"] = "Сохранено!";
        #elif defined(LANG_EN)
           answer["message"] = "Saved!";
        #endif

    } else if (action == "settings") {

        String _ssid = webServer.arg("ssidAP");
        String _password = webServer.arg("passwordAP");
        String _ssidAP = webServer.arg("ssid");
        String _passwordAP = webServer.arg("password");
        _ssid.toCharArray(ee_data.ssidAP, sizeof(ee_data.ssidAP));
        _password.toCharArray(ee_data.passwordAP, sizeof(ee_data.passwordAP));
        _ssidAP.toCharArray(ee_data.ssid, sizeof(ee_data.ssid));
        _passwordAP.toCharArray(ee_data.password, sizeof(ee_data.password));

        EEPROM.put(ee_addr_start_settings, ee_data);
        EEPROM.commit();

        #if defined(LANG_RU)
            answer["message"] = "Сохранено!";
        #elif defined(LANG_EN)
           answer["message"] = "Saved!";
        #endif

    } else if (action == "get_data") {

        String data_type = webServer.arg("data_type");

        JsonObject _settings = data["settings"].to<JsonObject>();
        _settings["wifi_mode"] = ee_data.wifi_mode;
        _settings["password"] = ee_data.password;
        _settings["ssidAP"] = ee_data.ssidAP;
        _settings["passwordAP"] = ee_data.passwordAP;
        _settings["ssid"] = ee_data.ssid;
        _settings["device_name"] = ee_data.device_name;

        #if defined(LANG_RU)
            answer["message"] = "Информация на странице обновлена";
        #elif defined(LANG_EN)
           answer["message"] = "Infoirmation was updated on the page!";
        #endif

    } else if (action == "settings_reboot") {
        restart();
    } else if (action == "settings_reset") {
        reset_settings();
    } else {
        answer["success"] = 0;
        #if defined(LANG_RU)
            answer["message"] = "неверный API";
        #elif defined(LANG_EN)
           answer["message"] = "Unknown API!";
        #endif
    }

    String sAnswer;
    serializeJson(answer, sAnswer);
    webServer.send(200, "text/html", sAnswer);
}

void restart() {
    EEPROM.get(ee_addr_start_settings, ee_data);
    WiFi.disconnect(true);
    WiFi.softAPdisconnect(true);
    ESP.restart();
}

void reset_settings() {

    // мигаем светодиодом
    byte y = 0;
    for(byte x = 0; x < 10; x++) {
        y = ~y;
        digitalWrite(2, y);
        delay(250);
    }

    EEPROM.write(ee_addr_start_firstrun, 0);
    EEPROM.commit();
    
    restart();

}

// сброс настроек по нажатию на кнопку
void reset_settings_btn() {
    for(byte x = 0; x < 50; x++) {
        delay(100);
        if (digitalRead(pin_btn_reset)==0) return;
    }
    // Настройки сбросятся только если кнопка была зажата в тнчение 5-ти секунд
    reset_settings();
}

/*
 * argument byte address - 7 bit, without RW bit.
 * argument int value - for PCF8575 - two bytes (for ports P00-P07 and P10-P17)
 */
/*void _i2c_channel_write(byte address, byte value[]) {
  Wire.beginTransmission(address);
  Wire.write(value, 2);
  Wire.endTransmission();
}*/

byte wfr_wifiClient_start(byte trying_count) {
    WiFi.begin(ee_data.ssid, ee_data.password);

    byte x = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        x += 1;
        if (x == trying_count) break;
    }

    if (WiFi.status() != WL_CONNECTED) return 0;
    else return 1;
}

void setup() {

    /* Первичная инициализация */

    digitalWrite(2, HIGH);
  
    //Serial.begin(115200);
    Serial.begin(9600);
    delay(10);
    EEPROM.begin(4096);//EEPROM.begin(ee_addr_start_settings+sizeof(ee_data));
    delay(10);
    pinMode(pin_btn_reset, INPUT);
    Serial.println();

    pinMode(pin_btn_reset, INPUT);
    // https://mirrobo.ru/c-arduino-ide-esp8266-preryvaniya-interrupt/
    //attachInterrupt(pin_btn_reset, reset_settings_btn, RISING);

    /* Инициализация настроек */

    if (EEPROM.read(ee_addr_start_firstrun) != code_firstrun) { // Устанавливаем настройки по умолчанию (если изделие запущено первый раз или настройки были сброшены пользователем)
        EEPROM.put(ee_addr_start_settings, ee_data);
        EEPROM.write(ee_addr_start_firstrun, code_firstrun); // при презапуске устройства код в этих скобках уже не выполнится, если вновь не сбросить настройки
        EEPROM.commit();
    }

    EEPROM.get(ee_addr_start_settings, ee_data);

    /* подготовка к запуску wifi */

    Serial.println("");
    byte _wifi_mode = ee_data.wifi_mode; // Не трогаем исходное значение

    /* WiFi как клиент */

    if (_wifi_mode == 1 || _wifi_mode == 2) {

        /*WiFi.begin(ee_data.ssid, ee_data.password);

        byte x = 0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            x += 1;
            if (x == 20) break;
        }*/

        is_wifi_client_connected = wfr_wifiClient_start(20);
    
        if (is_wifi_client_connected == 1) {
            Serial.println("WiFi connected");
            Serial.print("Client IP: ");
            Serial.println(WiFi.localIP());
        } else { // если не подключились в качестве клиента, то запускаемся в качестве точки доступа
            WiFi.disconnect(true);
            _wifi_mode = 0;
        }

    }

    /* WiFi как точка доступа */

    if (_wifi_mode == 0 || _wifi_mode == 2) {

        WiFi.softAP(ee_data.ssidAP, ee_data.passwordAP);
      
        Serial.println("AP started");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    }

    /* Запускаем веб-сервер */

    //webServer.on("/", HTTP_GET, handler_index_html);
    webServer.on("/api.c", HTTP_POST, apiHandler);
    //set_handlers();

    webServer.onNotFound(notFoundHandler);
    webServer.begin();
    Serial.println("Server started");
}

void loop() {
    webServer.handleClient();

    // если мы не смогли подключиться к сети при старте - пробуем ещё
    if (is_wifi_client_connected == 0 && (ee_data.wifi_mode == 1 || ee_data.wifi_mode == 2)) {
        is_wifi_client_connected = wfr_wifiClient_start(8);
        // если мы подключились к сети, но точку доступа включать не планировали, то выключим её
        if (is_wifi_client_connected == 1 && ee_data.wifi_mode == 1) {
            WiFi.softAPdisconnect();
        }
    }
}

