/* Обновляет всю информацию на странице */
function update_data(showing_message=true) {
    sendform(null, 'get_data', {answer_type: showing_message ? undefined : 'ButtonProgress', data: {}, func_success: function(res, arg) {
        document.getElementById('form_device_name').device_name.value = res.data.settings.device_name;

        document.getElementById('form_wifi_mode').wifi_mode.value = res.data.settings.wifi_mode;

        document.getElementById('form_wifi_wifi').password.value = res.data.settings.password;
        document.getElementById('form_wifi_wifi').ssid.value = res.data.settings.ssid;
        document.getElementById('form_wifi_wifi').passwordAP.value = res.data.settings.passwordAP;
        document.getElementById('form_wifi_wifi').ssidAP.value = res.data.settings.ssidAP;
    }});
}
