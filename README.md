# esp_course

ESP32 Wi-Fi application written while following an ESP-IDF course: SoftAP + station mode, an embedded web server for Wi-Fi provisioning, HTTP firmware (OTA) upload, credential storage in NVS and SNTP time sync.

## Features

- **Wi-Fi in AP + STA mode** (`main/wifi_app.c`): the ESP32 starts its own access point (`fabio_esp`, 192.168.5.1/24, channel 1, up to 5 clients) and can join another network as a station, retrying up to 5 times on disconnection.
- **Embedded web server** (`main/http_server.c`): the page in `main/webpage/` (HTML, CSS, JavaScript, jQuery 3.3.1) is embedded into the firmware with `EMBED_FILES` and served by `esp_http_server`.
- **Wi-Fi provisioning from the browser**: endpoints to connect to a network, check connection status, read connection info and disconnect.
- **OTA firmware update over HTTP**: a `.bin` uploaded through `/OTAupdate` is written to the next OTA partition with `esp_ota_*`; `/OTAstatus` reports the result and the device reboots into the new image.
- **Station credentials in NVS** (`main/app_nvs.c`): save, load and clear the SSID/password.
- **Wi-Fi reset button** (`main/wifi_reset_button.c`): GPIO 0 (BOOT button) with a falling-edge interrupt that disconnects the station and clears the stored credentials.
- **SNTP time sync** (`main/sntp_time_sync.c`): once connected, syncs time from `pool.ntp.org` (time zone set for Brazil) and exposes it to the web page.
- FreeRTOS tasks pinned to cores with priorities/stack sizes centralized in `main/tasks_common.h`.

### HTTP endpoints

| URI | Purpose |
|-----|---------|
| `/` , `/app.css`, `/app.js`, `/jquery-3.3.1.min.js`, `/favicon.ico` | Web page |
| `/OTAupdate`, `/OTAstatus` | Firmware upload and status |
| `/wifiConnect.json`, `/wifiConnectStatus.json`, `/wifiConnectInfo.json`, `/wifiDisconnect.json` | Wi-Fi provisioning |
| `/localTime.json` | Current SNTP time |
| `/apSSID.json` | SoftAP SSID |

## Hardware / target

- **Chip:** ESP32 (`CONFIG_IDF_TARGET="esp32"`), e.g. an ESP32 DevKit (the reset button uses the on-board BOOT button, GPIO 0)
- **Framework:** ESP-IDF (the `sdkconfig` was generated with ESP-IDF 5.3.0)

## Partition table (`partitions.csv`)

| Name | Type | SubType | Offset | Size |
|------|------|---------|--------|------|
| nvs | data | nvs | 0x9000 | 16 KB |
| otadata | data | ota | 0xD000 | 8 KB |
| phy_init | data | phy | 0xF000 | 4 KB |
| app0 | app | ota_0 | 0x10000 | 1.125 MB |
| app1 | app | ota_1 | 0x230000 | 1.125 MB |

## Repository layout

```
esp_course/
├── CMakeLists.txt
├── partitions.csv
├── sdkconfig
└── main/
    ├── main.c               # NVS init, start Wi-Fi app, reset button, SNTP on connect
    ├── wifi_app.c/.h        # Wi-Fi AP+STA state machine
    ├── http_server.c/.h     # web server, provisioning and OTA handlers
    ├── app_nvs.c/.h         # station credentials in NVS
    ├── sntp_time_sync.c/.h  # SNTP task
    ├── wifi_reset_button.c/.h
    ├── tasks_common.h       # task priorities, stacks and cores
    └── webpage/             # index.html, app.js, app.css, favicon, jQuery
```

## Build and flash

```bash
. $IDF_PATH/export.sh
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor   # replace with your serial port
```

Then connect to the `fabio_esp` access point and open `http://192.168.5.1/`. The SoftAP password is set in `main/wifi_app.h` (`WIFI_AP_PASSWORD`).
