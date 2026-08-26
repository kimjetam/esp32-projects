# esp32-projects

## Secrets
In order to use secrets, such as the Wi-Fi SSID and password, create a file named secrets.h in the project folder and add them there. This file is included in .gitignore and will not be checked into version control.

Example:
```
#pragma once

#define WIFI_SSID "your_wifi_name"
#define WIFI_PASSWORD "your_wifi_password"
```