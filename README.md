# ESP32 RS485 → TCP Server + HTTP Log Viewer

Firmware for ESP32 that reads RS485 packets (e.g. from Tigo TS4 systems), forwards them over TCP, and shows a real-time log via web interface.

## 🔧 Features

- Forwards raw RS485 packets via TCP (port 7160)
- Displays raw hex data on `/log` webpage
- Shows ESP32 IP and current TCP client (if any)
- Uses NTP to keep local time synced

## 📉 Hardware requirements

- ESP32
- TTL-RS485 adapter wired as:
  - RX to GPIO 16
  - TX to GPIO 17

## ⚖️ Configuration

Edit these lines in the sketch:

```cpp
const char* WIFI_SSID = "XXX";
const char* WIFI_PASS = "YYY";
```

## ▶️ Usage

1. Upload the firmware to your ESP32 (e.g. via Arduino IDE or PlatformIO)
2. Connect the RS485 line
3. Open browser to:
   ```
   http://<ESP32_IP>/log
   ```
4. From your PC, run `taptap`:
   ```sh
   ./taptap observe --tcp <ESP32_IP>
   ```

## 🔗 TCP Port

- The ESP32 listens on **port 7160**
- Packets are raw and end with `0xFE`

## ✨ Sample output

```text
ESP32 IP: 192.168.1.123
TCP Client: 192.168.1.45
7E 08 00 FF FF 7E 07 12 01 01 48 00 01 26 ...
```

---

Simple and efficient. Compatible with [taptap](https://github.com/willglynn/taptap) via TCP connection.

