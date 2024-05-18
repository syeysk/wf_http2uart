# wf_http2uart

Arduino program to send data to UART through HTTP

## Install

Upload the programm to ESP8266.

Connect ESP8266 to a [CNC-growbox](https://github.com/syeysk/sy-cnc-growbox) through UART.

Turn on Esp8266 and a CNC-growbox.

If everything is OK, you will see **"SY_WF_HTTP2UART"** WI-FI access point on your computer. Connect to the access point.

## Use

You can send the G-code into a CNC-growbox directly by HTTP:

```python
import requests

response = requests.post('http://192.168.4.1/api.c', params={'action': 'send_to_serail', 'string_data': 'E1501 A2\n'}, timeout=8)
print(response.status_code)
print(response.text)
```

You can run [GUI-programm](https://github.com/syeysk/sy-cnc-growbox-py) to manage your CNC-growbox more comfortable:

![window to connect]

## Set the private access

Go to the URL: http://192.168.4.1/ to set the access to the WI-FI device

