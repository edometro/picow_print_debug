#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
// #include <LEAmDNS.h>

#ifndef STASSID
#define STASSID "ESP32C6"
#define STAPSK "0123456789"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const IPAddress ip(192, 168, 40, 1);
const IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
// const int led = LED_BUILTIN;

// 並列処理で共有するログバッファ
String usbBuffer;
String uartBuffer;

void handleRoot() {
  // digitalWrite(led, HIGH);
  server.send(200, "text/plain", "hello from esp32 c3\r\n");
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
    // Serial1.print(msg); // UARTへ送信
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
  // Serial1.begin(115200);
  // 使用するピンに合わせてコメントを外して設定してください
  // 例: Serial1.setTX(GP12);
  //     Serial1.setRX(GP13);
  usbBuffer.reserve(256);
  uartBuffer.reserve(256);

  // WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(ip, ip, subnet);
  // WiFi.begin(ssid, password);

  Serial.println("");
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // if (MDNS.begin("picow")) {
  //   Serial.println("MDNS responder started (http://picow.local/)");
  // }

  server.on("/", handleRoot);
  server.on("/usb", handleUSB);
  server.on("/uart", handleUART);
  
  server.onNotFound(handleNotFound);

  server.begin();
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

  // // UART入力
  // while (Serial1.available()) {
  //   char c = Serial1.read();
  //   uartBuffer += c;
  // }
}