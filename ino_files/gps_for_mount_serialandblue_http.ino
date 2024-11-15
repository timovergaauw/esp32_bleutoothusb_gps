//build for ESP32-Wroom-DA-Module id= ESP32-D0WD Rev 301  programmeer op ESP32 dev module:
//flash size 4 MB -  partition scheme huge APP 3mb no OTA / 1mb spiffs  - psram disabled - jtag  esp usb bridge
//made for astro mount
#include "BluetoothSerial.h"
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>

// Set these to your desired credentials.
const char *ssid PROGMEM = "mijn-gps";
const char *password PROGMEM = "1234567890";
NetworkServer server(80);
IPAddress APip PROGMEM = "";

static const int RXPin PROGMEM = 16, TXPin = 17;//geek dongl 43,44 tx,rx // esp32wroom 16,17
static const uint32_t GPSBaud PROGMEM = 9600;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
String device_name PROGMEM = "ESP32-BT-Slave";//bluetooth devicename
// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled!
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available.
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  SerialBT.begin(device_name);
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation fail.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  APip = myIP;
  server.begin();
}

void loop()
{
  if (ss.available()) {
  SerialBT.write(ss.read());
  Serial.write(ss.read());
  }// seriel to bt write
  //if (SerialBT.available()) {
    //ss.write(SerialBT.read());
    //Serial.write(F(SerialBT.read()));
  //}
  NetworkClient client = server.accept();  // listen for incoming clients
  if (client) {
    while (client.connected()) {
      client.println(F("HTTP/1.1 200 OK"));
      client.println("Content-type:text/html");
      client.println();
      // the content of the HTTP response follows the header:
      client.print("BT_name= ");
      client.print(device_name);
      client.print("<br>");
      client.print("ipaddress= ");
      client.print(APip);
      client.print("<br>");
      // The HTTP response ends with another blank line:
      client.println();
      break;
    }
    // close the connection:
    client.stop();
  }
}