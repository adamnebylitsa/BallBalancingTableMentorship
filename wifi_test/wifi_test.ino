#include "WiFiS3.h"
#include "arduino_secrets.h" 
#include "Arduino_LED_Matrix.h"


char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
ArduinoLEDMatrix matrix;


int status = WL_IDLE_STATUS;
//WiFiServer server(80);
WiFiClient client;
uint8_t frame[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
const uint32_t full[] ={
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF
};

void setup() {
  matrix.begin();
  wifiSetup();

}

void loop() {
  // put your main code here, to run repeatedly:
  String value = read_response();
  if (value!=""){
    Serial.println(value);
    int center = value.indexOf(" ");
    int x= value.substring(0,center).toInt();
    int y=value.substring(center+1).toInt();
    frame[x][y]=!frame[x][y];
    Serial.print(String(x)+String(y)+" is now "+String(frame[x][y])+"\n");
    client.print(String(x)+String(y)+" is now "+String(frame[x][y])+"\n");
  }
  matrix.renderBitmap(frame, 8, 12);
}


void wifiSetup() {
  Serial.begin(9600);      // initialize serial communication

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  //server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  while (!client.connect(ip_address, REMOTE_PORT)) {
    Serial.println("Connection failed.");
    Serial.println("Waiting a moment before retrying...");
    matrix.loadFrame(full);
  }
  printWifiStatus();
}

void printWifiStatus() {
/* -------------------------------------------------------------------------- */  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
String read_response() {
  String message="";
  while (client.available()) {
    /* actual data reception */
    char c = client.read();
    /* print data to serial port */
    message = message+c;
  }
  return message;
}