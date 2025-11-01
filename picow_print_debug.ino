// #include <ESPAsyncWebServer.h> // これいる？

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
// #include <LEAmDNS.h>

// #define esp32
#define PicoW


#ifndef STASSID

#ifdef PicoW
#define STASSID "PicoW"
#endif
#ifdef esp32
#define STASSID "SeedXiaoC3"
#endif

#define STAPSK "0123456789"
#endif

// 接続したいWi-FiのSSIDとパスワードを設定
#define WIFI_SSID "giga-tmu-1gou" // 接続先のSSIDに変更
#define WIFI_PASS "giga-1gou"   // 接続先のパスワードに変更

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// const IPAddress ip(192, 168, 40, 1);
// const IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
// const int led = LED_BUILTIN;

// 並列処理で共有するログバッファ
String usbBuffer;
String uartBuffer;

void handleRoot() {
  // digitalWrite(led, HIGH);
  #ifdef PicoW
  server.send(200, "text/plain", "hello from PicoW\r\n");
  #endif
  #ifdef esp32
  server.send(200, "text/plain", "hello from esp32 c3\r\n");
  #endif
  // digitalWrite(led, LOW);
}

void handleUSB() {
  String data = usbBuffer;
  server.send(200, "text/plain", data.c_str());
  usbBuffer = "";
}

void handleUART() {
  String data = uartBuffer;
  server.send(200, "text/plain", data.c_str());
  uartBuffer = "";
}

void handleOutput() {
  String path = server.uri(); // 例: "/output/aaa"
  String msg;
  if (path.startsWith("/output/")) {
    msg = path.substring(8); // "/output/" の後ろを切り出す
  }

  if (msg.length() > 0) {
    Serial.print(msg);  // USBへ送信
    Serial1.print(msg); // UARTへ送信
    server.send(200, "text/plain", "Sent: " + msg);
  } else {
    server.send(200, "text/plain", "No Massage");
  }
}

void handleNotFound() {
  String path = server.uri();
  if (path.startsWith("/output/")) {
    handleOutput();
  } else {
    server.send(404, "text/plain", "Not Found");
  }
}

void setup() {
  // pinMode(led, OUTPUT);
  // digitalWrite(led, LOW);
  Serial.begin(115200);
  // USB: Serial は既に使ってるので Serial1 を UART 用に
  #ifdef PicoW
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(115200);
  #endif
  #ifdef esp32
  Serial1.begin(115200, SERIAL_8N1, D7, D6); // RX, TX
  #endif
  usbBuffer.reserve(256);
  uartBuffer.reserve(256);

  // --- ★ 変更箇所 (通常のWi-Fi接続) ★ ---
  WiFi.mode(WIFI_STA); // STAモード（クライアントモード）に設定
  WiFi.begin(ssid, password); // 接続開始

  Serial.print("Connecting to WiFi...");
  // 接続が完了するまで待機
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  // ----------------------------------------

  // if (MDNS.begin("picow")) {
  //   Serial.println("MDNS responder started (http://picow.local/)");
  // }

  server.on("/", handleRoot);
  server.on("/usb", handleUSB);
  server.on("/uart", handleUART);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connected to SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // 割り当てられたIPアドレスを表示
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
  // MDNS.update();
      // バッファが大きくなりすぎたらクリア
  if (usbBuffer.length() > 250) {
    usbBuffer = "";
  }
  if (uartBuffer.length() > 250) {
    uartBuffer = "";
  }

  // USB入力
  while (Serial.available()) {
    char c = Serial.read();
    usbBuffer += c;
  }

  // UART入力
  while (Serial1.available()) {
    char c = Serial1.read();
    uartBuffer += c;
  }
}