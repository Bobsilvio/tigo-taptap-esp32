#include <WiFi.h>
#include <WebServer.h>
#include <WiFiServer.h>
#include <time.h>

const char* WIFI_SSID = "XXX";
const char* WIFI_PASS = "YYY";

#define RXD2 16
#define TXD2 17
#define RS485_BAUD 38400

WiFiServer tcpServer(7160);
WiFiClient tcpClient;
WebServer server(80);

const int MAX_LOG_LINES = 50;
String logLines[MAX_LOG_LINES];
int logCount = 0;

void addLogLine(const String &line) {
  if (logCount < MAX_LOG_LINES) logLines[logCount++] = line;
  else {
    for (int i = 1; i < MAX_LOG_LINES; ++i) logLines[i - 1] = logLines[i];
    logLines[MAX_LOG_LINES - 1] = line;
  }
}

void handleLog() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>Log RS485 Tigo</title></head><body>";
  html += "<h2>Pacchetti RS485 (Tigo)</h2><pre>";
  html += "<b>ESP32 IP:</b> " + WiFi.localIP().toString() + "\n";
  html += "<b>TCP Client:</b> ";
  html += (tcpClient && tcpClient.connected()) ? tcpClient.remoteIP().toString() : "Nessun client connesso";
  html += "\n\n";
  for (int i = 0; i < logCount; ++i) {
    html += logLines[i] + "\n";
  }
  html += "</pre>";
  time_t now = time(NULL);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  char timeStr[9];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  html += "<p>Ora: " + String(timeStr) + "</p></body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(RS485_BAUD, SERIAL_8N1, RXD2, TXD2);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  getLocalTime(&timeinfo, 5000);

  tcpServer.begin();

  server.on("/log", handleLog);
  server.onNotFound([]() {
    server.sendHeader("Location", "/log");
    server.send(302, "text/plain", "");
  });
  server.begin();
}

void loop() {
  server.handleClient();

  if (!tcpClient || !tcpClient.connected()) {
    tcpClient = tcpServer.available();
  }

  static uint8_t buf[256];
  static int bufPos = 0;
  while (Serial2.available() > 0) {
    uint8_t byte = Serial2.read();
    if (bufPos < 256) buf[bufPos++] = byte;
    else bufPos = 0;
    if (byte == 0xFE) {
      String rawLine = "";
      for (int i = 0; i < bufPos; ++i) {
        char hexByte[4];
        sprintf(hexByte, "%02X ", buf[i]);
        rawLine += hexByte;
      }
      addLogLine(rawLine);
      if (tcpClient && tcpClient.connected()) tcpClient.write(buf, bufPos);
      bufPos = 0;
    }
  }
}
