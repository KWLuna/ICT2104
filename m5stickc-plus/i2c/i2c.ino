/* Include necessary libraries */
#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Wire.h>

/* Preprocessor directives */
#define SLAVE_ADDR 8
#define PICO_BARCODE 'a'
#define PICO_DISTANCE 'b'
#define PICO_HUMP 'c'
#define PICO_MAP 'd'

/* Server object */
WiFiServer server(80);

/* Global variables */
const char *ssid = "A3_M5StickC_Plus";
const char *password = "12345678";
char inst = '\0';                 // Instruction
char barcode_data = '\0';         // Alphabet read from barcode
float distance_data = 0.00;       // Distance
float hump_data = 0.00;           // Hump height
uint8_t map_data = 0;             // Map
int x = -1;
int y = -1;

/* Function to handle received data */
void receiveData(int numBytes) {
  uint8_t beforeDP = 0;
  uint8_t afterDP = 0;
  if (Wire.available() >= 2) {
    inst = Wire.read();
    switch(inst) {
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
        map_data = Wire.read();
        break;
      default:
        Serial.println("CHECK");
        break;
    }
  }
}

/* Function to send coordinates to Pico */
void sendData() {
  // Considering to include conditional to check when car stops
  if (x != -1 && y != -1) {
    Wire.write(x);
    Wire.write(y);
    x = -1;
    y = -1;
  }
}

/* Runs only on first init */
void setup() {
  // M5
  M5.begin();
  WiFi.softAP(ssid, password);  // Setting ssid and password for the access point
  server.begin();               // Starting server

  // I2C
  Serial.begin(115200);
  Wire.begin(SLAVE_ADDR);       // Join I2C bus with address 8
  Wire.onReceive(receiveData);  // Register event (Pico sending data)
  Wire.onRequest(sendData);     // Register event (Pico requesting for data)
}

/* Main program */
void loop() {
  // Test display
  M5.Lcd.setCursor(25, 100);
  M5.Lcd.print("TI: ");
  M5.Lcd.print(inst);
  M5.Lcd.print(", TD: ");
  M5.Lcd.print(distance_data);

  // App display
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println(); //Blank line before request content

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // Styling
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; margin-bottom:50px; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");

            // Heading
            client.println("</style></head><body><h1>ICT2104 Robot Car</h1>");
            client.println("<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>");

            // Barcode
            client.println("<tr><td>Barcode Data : </td><td>");
            client.println(barcode_data);
            client.println("</td></tr>");
            
            // Distance
            client.println("<tr><td>Distance : </td><td>");
            client.println(distance_data);
            client.println(" cm</td></tr>");
             
            // Hump Height
            client.println("<tr><td>Hump Height Detected : </td><td>");
            client.println(hump_data);
            client.println(" cm</td></tr></table>"); 
            
            // Mapping
            client.println("<table><tr><th>MAZE MAP</th></tr>");
            client.println("<tr><td>print map here?</td></tr></table>");
            client.println("</body></html>");
            
            client.println(); // Blank line for end of request
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
  }
  client.stop();
}
