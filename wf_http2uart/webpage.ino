/* This code was generated with
** https://github.com/syeysk/tool_webdata2esp
** try online: https://py2c.ru/web2esp/
*/

void handler_after_load_js() {
    webServer.sendHeader("Content-Encoding", "gzip");
    //File f = SPIFFS.open("/index.html", "r");
    webServer.send_P(200, "application/javascript", const_handler_after_load_js, 146);
}
void handler_functions_device_js() {
    webServer.sendHeader("Content-Encoding", "gzip");
    //File f = SPIFFS.open("/index.html", "r");
    webServer.send_P(200, "application/javascript", const_handler_functions_device_js, 247);
}
void handler_functions_embedded_js() {
    webServer.sendHeader("Content-Encoding", "gzip");
    //File f = SPIFFS.open("/index.html", "r");
    webServer.send_P(200, "application/javascript", const_handler_functions_embedded_js, 3432);
}
void handler_index_css() {
    webServer.sendHeader("Content-Encoding", "gzip");
    //File f = SPIFFS.open("/index.html", "r");
    webServer.send_P(200, "text/css", const_handler_index_css, 442);
}
void handler_index_html() {
    webServer.sendHeader("Content-Encoding", "gzip");
    //File f = SPIFFS.open("/index.html", "r");
    webServer.send_P(200, "text/html", const_handler_index_html, 1108);
}

