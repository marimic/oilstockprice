/*

Oil Price Sketch
 
*/


#include <SPI.h>
#include <WiFi101.h>


char ssid[] = "BanffCentre";       
char pass[] = "";    

String text;
int endResponse = 0;
int status = WL_IDLE_STATUS;

const char server[] = "utilities.banff.org";   

WiFiClient client;
unsigned long lastConnectionTime = 0;     
const unsigned long postingInterval = 10 * 1000;  // 10 seconds delay for testing

void setup() {
  Serial.begin(9600);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);
    delay(10000);
  }
  printWifiStatus();
}

void loop() {
  // this only happens at the indicated interval
  if (millis() - lastConnectionTime > postingInterval) {
    lastConnectionTime = millis();
    httpRequest(); 
  }

  //this goes as fast as the http client can accept data
  char c = 0;
  if (client.available()) {
    c = client.read();
    Serial.print(c);
  }
 
}


// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    // Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /oilprice/ HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}



void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
