/* Include necessary libraries */
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Wire.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/* Preprocessor directives */
#define SLAVE_ADDR 8
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

int coordinateX = -1;
int coordinateY = -1;
char inst = '\0';            // Instruction
char barcode_data = '\0';    // Alphabet read from barcode
float distance_data = 0.00;  // Distance
float hump_data = 0.00;      // Hump height
uint8_t map_data = 0;        // Map
int map_data[9][11];

/* Web form*/
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
    <title>ICT2104 A3</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="refresh" content="3">
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
      <tr>
        <td>
          <form action="/maze">
            View Maze Map
            <input type="submit" value="Submit"/>
          </form>
        </td>
      </tr>
    </table>
  </body></html>)rawliteral";

/* Function to handle received data */
void receiveData(int numBytes) {
  uint8_t beforeDP = 0;
  uint8_t afterDP = 0;
  if (Wire.available() >= 2) {
    inst = Wire.read();
    switch (inst) {
      case PICO_BARCODE:
        barcode_data = Wire.read();
        break;
      case PICO_DISTANCE:
        beforeDP = Wire.read();
        afterDP = Wire.read();
        distance_data = beforeDP + ((float)afterDP / 100);
        break;
      case PICO_HUMP:
        beforeDP = Wire.read();
        afterDP = Wire.read();
        distance_data = beforeDP + ((float)afterDP / 100);
        break;
      case PICO_MAP:
       for (int i = 0; i < 9; i++) {
          for (int j = 0; j < 11; j++) {
            map_data[i][j] = Wire.read();
          }
        }
        break;
      default:
        Serial.println("No data");
        break;
    }
  }
}

/* Function to send coordinates to Pico */
void sendData() {
  // Considering to include conditional to check when car stops
  if (coordinateX != -1 && coordinateY != -1) {
    Wire.write(coordinateX);
    Wire.write(coordinateY);
    coordinateX = -1;
    coordinateY = -1;
  }
}

String processor(const String &var) {
  if (var == "barcode_data") {
    return String(barcode_data);
  } else if (var == "distance_data") {
    return String(distance_data);
  } else if (var == "hump_data") {
    return String(hump_data);
  }
}

String maze_map() {
  String maze_map = "<!DOCTYPE html> <html>\n";
  maze_map += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  maze_map += "<title>ICT2104 A3</title>\n";
  maze_map += "<style>span{white-space: pre;display:inline-block; color: black; width: 20px; height: 20px; padding:0;margin:0;}\n";
  maze_map += "body { text-align: center; font-family: 'Trebuchet MS', 'Arial';}\n";
  maze_map += "</style>\n";
  maze_map += "</head>\n";
  maze_map += "<body>\n";
  maze_map += "<table>\n";
  maze_map += "<tr>\n";
  maze_map += "<th>MAZE MAP</th>\n";
  maze_map += "</tr>\n";
  maze_map += "<tr>\n";
  maze_map += "<td>\n";
  maze_map += "<script>\n";
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 11; j++) {
      maze_map += "var span = document.createElement('span');\n";
      if (map_data[i][j] == 1) {
          Serial.println("black");
          maze_map += "span.textContent = 'A+';\n";
          maze_map += "span.style.backgroundColor = 'black';\n";
          maze_map += "span.style.color = 'black';\n";
      } else if (map_data[i][j] == 0) {
        Serial.println("white");
        maze_map += "span.textContent = 'A+';\n";
        maze_map += "span.style.backgroundColor = 'white';\n";
        maze_map += "span.style.color = 'white';\n";
      }
      maze_map += "document.body.appendChild(span);\n";
    }
    maze_map += "document.body.appendChild(document.createElement('br'));\n";
  };
  maze_map += "</script>\n";
  maze_map += "</td>\n";
  maze_map += "</tr>\n";
  maze_map += "</table>\n";
  maze_map += "<br><a href=\"/\">Return to Home Page</a>";
  maze_map += "</body>\n";
  maze_map += "</html>\n";

  return maze_map;
}

/* Runs only on first init */
void setup() {
  // M5
  M5.begin();
  WiFi.softAP(ssid, password);  // Setting ssid and password for the access point
  server.begin();               // Starting server

 // I2C
  Serial.begin(115200);
  Wire.begin(SLAVE_ADDR);      // Join I2C bus with address 8
  Wire.onReceive(receiveData); // Register event (Pico sending data)
  Wire.onRequest(sendData);    // Register event (Pico requesting for data)

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String coordX;
    String coordY;

    // GET coordinate X & Y values on <192.168.4.1>/get?coordinatesX=<coordinateX>&coordinatesY=<coordinateY>
    if (request->hasParam(paramX) && request->hasParam(paramY)) {
      coordX = request->getParam(paramX)->value();
      coordY = request->getParam(paramY)->value();
      coordinateX = coordX.toInt();
      coordinateY = coordY.toInt();

      // M5.Lcd.setCursor(25, 100);
      // M5.Lcd.print(coordinateX);
      // M5.Lcd.print(coordinateY);
      // Serial.println(coordinateX);
      // Serial.println(coordinateX);
    }
    request->send(200, "text/html", "Coordinate X: " + coordinateX + " Coordinate Y: " + coordinateY + "<br><a href=\"/\">Return to Home Page</a>");
  });

  server.on("/maze", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", maze_map());
  });

  server.begin();
}

/* Main program */
void loop() {
  M5.Lcd.setCursor(50,100);
  M5.Lcd.print("I2C MODE");
}