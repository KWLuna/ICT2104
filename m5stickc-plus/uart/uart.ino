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

/* Server object */
// WiFiServer server(80);
AsyncWebServer server(80);


/* Global variables */
const char* ssid = "M5StickC_Plus_AP";
const char* password = "12345678";
const char* paramX = "coordinatesX";
const char* paramY = "coordinatesY";

uint8_t inst = 0;
uint8_t barcode_data = 0;
uint8_t distance_data = 0;
uint8_t hump_data = 0;
uint8_t map_data = 0;
int x = -1;
int y = -1;


/* Web form*/
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
    <title>ICT2104 A3</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="refresh" content="5" >
    <style>
      body { text-align: center; font-family: 'Trebuchet MS', 'Arial';}
      table {border-collapse: collapse; width: 35%%; margin-left: auto; margin-right: auto; margin-bottom: 50px;}
      th {padding: 12px; background-color: #0043af; color: white;}
      tr {border: 1px solid #ddd ;padding: 12px;}
      tr:hover {background-color: #bcbcbc;}
      td {border: none; padding: 12px;}
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
        <td>Coordinates <br>X & Y:</td>
          <td>
          <form action="/get">
            <input type="text" name="coordinatesX"/>
            <input type="text" name="coordinatesY"/>
            <input type="submit" value="Submit"/>
          </form>
        </td>
      </tr>
    </table>
    <table>
      <tr>
        <th>MAZE MAP</th>
      </tr>
      <tr>
        <td>print map here?</td>
      </tr>
    </table>
  </body></html>)rawliteral";

/* Function to send coordinates to Pico */
void sendData(int x, int y) {
  // Considering to include conditional to check when car stops
  if (x != -1 && y != -1) {
    Serial2.write(x);
    Serial2.write(y);
    x = -1;
    y = -1;
  }
}


String processor(const String& var) {
  if (var == "barcode_data") {
    return String(barcode_data);
  } else if (var == "distance_data") {
    return String(distance_data);
  } else if (var == "hump_data") {
    return String(hump_data);
  }
  return String();
}

/* Runs only on first init */
void setup() {
  M5.begin();                   // M5
  WiFi.softAP(ssid, password);  // Setting ssid and password for the access point
  // // server.begin();               // Starting server

  // // UART
  Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
    String coordinateX;
    String coordinateY;

    // GET coordinate X & Y values on <192.168.4.1>/get?coordinatesX=<coordinateX>&coordinatesY=<coordinateY>
    if (request->hasParam(paramX) && request->hasParam(paramY)) {
      coordinateX = request->getParam(paramX)->value();
      coordinateY = request->getParam(paramY)->value();
      M5.Lcd.print(coordinateX);
      M5.Lcd.print(coordinateY);
    }
    request->send(200, "text/html", "Coordinates X: " + coordinateX + " Coordinates Y: " + coordinateY + " has been sent to the car.<br><a href=\"/\">Return to Home Page</a>");
  });

  server.begin();
}


/* Main program */
void loop() {
  // server.handleClient();
  // Read data from buffer

  if (Serial2.available()) {
    inst = Serial2.read();
    switch (inst) {
      case 97:
        barcode_data = Serial2.read();
        break;
      case 98:
        distance_data = Serial2.read();
        break;
      case 99:
        hump_data = Serial2.read();
        break;
      case 100:
        map_data = Serial2.read();
        break;
      default:
        Serial.println("CHECK");
        break;
    }
  }

  // M5 test display
  M5.Lcd.setCursor(25, 100);
  M5.Lcd.print("Test data: ");
  M5.Lcd.print(inst);

  // // App display
  // WiFiClient client = server.available();
  // if (client) {
  //   String currentLine = "";
  //   while (client.connected()) {
  //     if (client.available()) {
  //       char c = client.read();

  //       if (c == '\n') {
  //         if (currentLine.length() == 0) {
  //           client.println("HTTP/1.1 200 OK");
  //           client.println("Content-type:text/html");
  //           client.println();  //Blank line before request content

  //           client.println("<!DOCTYPE html><html>");
  //           client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  //           client.println("<link rel=\"icon\" href=\"data:,\">");

  //           // Styling
  //           client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
  //           client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; margin-bottom:50px; }");
  //           client.println("th { padding: 12px; background-color: #0043af; color: white; }");
  //           client.println("tr { border: 1px solid #ddd; padding: 12px; }");
  //           client.println("tr:hover { background-color: #bcbcbc; }");
  //           client.println("td { border: none; padding: 12px; }");
  //           client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");

  //           // Heading
  //           client.println("</style></head><body><h1>ICT2104 Robot Car</h1>");
  //           client.println("<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>");

  //           // Barcode
  //           client.println("<tr><td>Barcode Data : </td><td>");
  //           client.println(uart_data);
  //           client.println("</td></tr>");

  //           // Distance
  //           client.println("<tr><td>Distance Travelled : </td><td>");
  //           client.println(uart_data);
  //           client.println(" cm</td></tr>");

  //           // Hump Height
  //           client.println("<tr><td>Hump Height Detected : </td><td>");
  //           client.println(uart_data);
  //           client.println(" cm</td></tr>");

  //           client.println("<tr><td>");
  //           client.println("<form action='/get'>Coordinates: <input type='text' name='coordinates'><input type='submit' value='Submit'></form></td></tr>");
  //           client.println("</table>");

  //           // Mapping
  //           client.println("<table><tr><th>MAZE MAP</th></tr>");
  //           client.println("<tr><td>print map here?</td></tr></table>");

  //           client.println("</body></html>");

  //           client.println();  // Blank line for end of request
  //           break;
  //         } else {
  //           currentLine = "";
  //         }
  //       } else if (c != '\r') {
  //         currentLine += c;
  //       }
  //       M5.Lcd.setCursor(50, 100);
  //       M5.Lcd.print(currentLine);
  //     }
  //   }
  // }
  // client.stop();
}