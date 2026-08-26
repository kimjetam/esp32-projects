# esp32-projects

## Secrets
In order to use secrets, like wifi ssid and password, create a file in project folder named `secrets.h` and add it there. This file is included in `.gitignore` file and will not be checked-in version control.

Example:
```
#pragma once

#define WIFI_SSID "your_wifi_name"
#define WIFI_PASSWORD "your_wifi_password"
```