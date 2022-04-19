# TinkableParamsServer
Parameter config server for Tinkable IoT Kit.
Based on https://github.com/tzapu/WiFiManager.
Run a web server on ESP8266 to configure settings.

## changes from WiFiManager

* Deleted the WiFi connection function, leaving only the part to set the name and value of settings.
* To reduce memory usage, String objects have been replaced with char arrays where possible.
* The setting value can be specified in the form of select or textarea.
* bi-languages. config page has two languages: english, korean. you can edit korean to other language.
* ESP32 related code has been deleted.

## Feature to be upgraded

* Supports ESP32