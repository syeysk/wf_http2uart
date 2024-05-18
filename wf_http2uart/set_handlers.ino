/* This code was generated with
** https://github.com/syeysk/tool_webdata2esp
** try online: https://py2c.ru/web2esp/
*/

void set_handlers(void) {
    webServer.on("/after_load.js", HTTP_GET, handler_after_load_js);
    webServer.on("/functions_device.js", HTTP_GET, handler_functions_device_js);
    webServer.on("/functions_embedded.js", HTTP_GET, handler_functions_embedded_js);
    webServer.on("/index.css", HTTP_GET, handler_index_css);
    webServer.on("/index.html", HTTP_GET, handler_index_html);
}
