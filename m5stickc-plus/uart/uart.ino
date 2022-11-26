/* Include necessary libraries */
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Wire.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/* Preprocessor directives */
#define BAUD_RATE 115200
#define RX_PIN 32
#define TX_PIN 33
#define PICO_BARCODE 'a'
#define PICO_DISTANCE 'b'
#define PICO_HUMP 'c'
#define PICO_MAP 'd'

/* Server object */
AsyncWebServer server(80);

/* Global variables */
const char *ssid = "A3_M5StickC_Plus";
const char *password = "12345678";
const char *paramX = "coordinatesX";
const char *paramY = "coordinatesY";

char inst = '\0';
char barcode_data = '\0';
float distance_data = 0.00;
float hump_data = 0.00;
uint8_t map_data = 0;
int x = -1;
int y = -1;

/* Web form*/
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
    <title>ICT2104 A3</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="refresh" content="5">
    <style>
      body { text-align: center; font-family: 'Trebuchet MS', 'Arial';}
      table {border-collapse: collapse; width: 35%%; margin-left: auto; margin-right: auto; margin-bottom: 50px;}
      th {padding: 12px; background-color: #0043af; color: white;}
      tr {border: 1px solid #ddd ;padding: 12px;}
      tr:hover {background-color: #bcbcbc;}
      td {border: none; padding: 18px;}
    </style>
  </head>
  <body>
    <h1>ICT2104 Robot Car</h1>
    <table>
      <tr>
        <th>MEASUREMENT</th>
        <th>VALUE</th>
      </tr>
      <tr>
        <td>Barcode Data :</td>
        <td>%barcode_data%</td>
      </tr>
      <tr>
        <td>Distance Travelled :</td>
        <td>%distance_data% cm</td>
      </tr>
      <tr>
        <td>Hump Height Detected :</td>
        <td>%hump_data% cm</td>
      </tr>

      <tr>
        <td>Input Coordinates X & Y <br> To Move Car:</td>
          <td>
          <form action="/get">
            <input type="text" name="coordinatesX"/><br>
            <input type="text" name="coordinatesY"/><br><br>
            <input type="submit" value="Submit"/>
          </form>
        </td>
      </tr>
    </table>
  </body></html>)rawliteral";

/* Function to read data from Pico */
void readData()
{
  uint8_t beforeDP = 0;
  uint8_t afterDP = 0;
  if (Serial2.available() >= 2)
  {
    inst = Serial2.read();
    switch (inst)
    {
    case PICO_BARCODE:
      barcode_data = Serial2.read();
      break;
    case PICO_DISTANCE:
      beforeDP = Serial2.read();
      afterDP = Serial2.read();
      distance_data = beforeDP + ((float)afterDP / 100);
      break;
    case PICO_HUMP:
      beforeDP = Serial2.read();
      afterDP = Serial2.read();
      hump_data = beforeDP + ((float)afterDP / 100);
      break;
    case PICO_MAP:
      map_data = Serial2.read();
      break;
    default:
      Serial.println("No data");
      break;
    }
  }
}

/* Function to send coordinates to Pico */
void sendData(int x, int y)
{
  // Considering to include conditional to check when car stops
  if (x != -1 && y != -1)
  {
    Serial2.write(x);
    Serial2.write(y);
    x = -1;
    y = -1;
  }
}

String processor(const String &var)
{
  if (var == "barcode_data")
  {
    return String(barcode_data);
  }
  else if (var == "distance_data")
  {
    return String(distance_data);
  }
  else if (var == "hump_data")
  {
    return String(hump_data);
  }
  return String();
}

/* Runs only on first init */
void setup()
{
  M5.begin();                  // M5
  WiFi.softAP(ssid, password); // Setting ssid and password for the access point
  // // server.begin();               // Starting server

  // UART
  Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });

  // // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    String coordinateX;
    String coordinateY;

    // GET coordinate X & Y values on <192.168.4.1>/get?coordinatesX=<coordinateX>&coordinatesY=<coordinateY>
    if (request->hasParam(paramX) && request->hasParam(paramY)) {
      coordinateX = request->getParam(paramX)->value();
      coordinateY = request->getParam(paramY)->value();
      M5.Lcd.setCursor(25, 100);
      M5.Lcd.print(coordinateX);
      M5.Lcd.print(coordinateY);
      Serial.println(coordinateX);
      Serial.println(coordinateX);
    }
    request->send(200, "text/html", "Coordinate X: " + coordinateX + " Coordinate Y: " + coordinateY + "<br><a href=\"/\">Return to Home Page</a>"); });

  server.begin();
}

/* Main program */
void loop()
{
  // Read data from buffer
  readData();
}
