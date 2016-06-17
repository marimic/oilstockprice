/*
Oil Price Get and Display SketchSketch -- Arduino Zero
Kenny Lozowski - kenny_lozowski@banffcentre.ca
for Maria Michails 
*/

#include <SPI.h> //include the library headers for the SPI Bus 
#include <WiFi101.h> //include the library headers for the Wifi shield
#define WIFI_SSID "BanffCentre" //define the macro for the Wifi SSID
#define WIFI_PASSWD "" //define the macro for the Wifi Password -- leave this empty for an open network 
#define HTTP_SERVER "utilities.banff.org"  //HTTP Server hostname
#define HTTP_PATH "/oilprice/"  //path to the oil price scraper
#define OILPRICE_DELIMITER 126 //the ASCII code for the oil price delimiter -- we're using tilde
String oilPriceText = "       "; //define and set the oil price as an 8 character Arduino String, I don't want to fool with multi-length strings so we leave enough room
WiFiClient client; //define the HTTP Client
int status = WL_IDLE_STATUS;
bool parseState = false; //defines whether we're reading the oil price from the longer HTTP return message
int oilStrIndex = 0; //what index we're at in the oil price string when adding characters to the string
unsigned long lastConnectionTime = 0;  //when we last connected 
const unsigned long postingInterval = 10000;  //the interval between connections -- it's 10 seconds right now

//this is an arduino boilerplate function that runs once on boot
void setup() {
  Serial.begin(115200); //open the serial port at 115200 baud
  delay(3000); //delay so we don't miss any of the Serial output
  if (WiFi.status() == WL_NO_SHIELD) { //if we don't have a Wifi shield
    Serial.println("WiFi shield not present"); //tell us
    while (true); //and hang
  }
  while ( status != WL_CONNECTED) { //if we're not connected 
    Serial.print(" Attempting to connect to SSID: "); //tell us an attempt is beginning
    Serial.println(WIFI_SSID); //on which SSID
    if (WIFI_PASSWD == "") status = WiFi.begin(WIFI_SSID); //connect if it's an open Wifi network
    else WiFi.begin(WIFI_SSID,WIFI_PASSWD); //or if it's a closed Wifi Network
    delay(10000); //wait a while
  }
  printWifiStatus(); //function that prints our status
}

//another boilerplate function that runs as fast as it can over and over
void loop() {
  if (millis() - lastConnectionTime > postingInterval) {  //this block  happens at the interval set by postingInterval
    lastConnectionTime = millis(); //reset the last connection time to now
    writeAndClearOilPrice(); //display the oil price, then clear it so we can fill it again -- the first time we'll just display the empty initialization string
    httpRequest(); //make an HTTP request to the server
  }
  httpReadBufferAndParse(); //read the HTTP Client buffer if there's any data  - this goes as fast as the http client can accept data
}

//write the oil price to the serial port and then clear it for next time
void writeAndClearOilPrice() {
  Serial.print("Price of Oil is: ");
  Serial.println(oilPriceText); // display the price of oil
  oilPriceText = "        "; // empty the oil price string so it can be filled again
}

//read the buffer from the HTTP client and grab the oil price from it
void httpReadBufferAndParse() {
  char c = 0;
  if (client.available()) { //do we have some characters in the HTTP client buffer?
    c = client.read(); // if yes, read one;
    if (c == OILPRICE_DELIMITER) { // did we get to the Delimiter? (a tilde)
      if (!parseState) c = client.read(); //if it's the first tilde, move beyond it
      else oilStrIndex = 0; // if it's the last tilde, set the string index at 0 again to fill it from the beginning
      parseState = !parseState; // turn the parse Staste on or off
    }
    if (parseState) { //are we receiving the oil price characters?
      oilPriceText.setCharAt(oilStrIndex,c); // fill the string with the character
      oilStrIndex++; //move the index onwards
    }
  }
}


// this function makes an HTTP client request to the server
void httpRequest() {
  client.stop();  // close any open connection before sending a new request.
  if (client.connect(HTTP_SERVER, 80)) {  // if there's a successful connection:
    //a properly formed HTTP request
    client.print("GET ");
    client.print(HTTP_PATH);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(HTTP_SERVER);
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {// if we couldn't make a connection:
    Serial.println("connection failed"); // tell us
  }
}

// this gives us some basic Wifi information
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());   // print the SSID of the network we're attached to:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip); // print your WiFi shield's IP address:
  long rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");  // print the received signal strength:
}
