/*
 */
#define RELAY_PIN 2

#include <ESP8266WiFi.h>

#include <Espwifi.h> // Custom library that creates getters for ssid and password.
Espwifi Espwifi(1);  // Create instance of custom library, ignore the parameter 1 as it can be anything.

// NOTE!
// The custom library was created in order to publish this project without including the wifi ssid and password details.
// You can just set ssid and password to your own details in the the SETUP function instead.
// You can then comment out the above two lines.

// Create an instance of the server, specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  char* ssid = Espwifi.getssid(); // get ssid from custom library.
  char* password = Espwifi.getpassword(); // get password from custom library.
  
  // If using this project, comment out the above two lines, and uncomment the following,
  // replacing <YOUR SSID> and <YOUR PASSWORD> with your own connection details.
  
  // char* ssid = "<YOUR SSID>";
  // char* password = "<YOUR PASSWORD>";
  
  delay(10);

  // prepare GPIO2
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/relay/off") != -1)
    val = 1;
  else if (req.indexOf("/relay/on") != -1)
    val = 0;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(RELAY_PIN, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nSOCKET IS NOW ";

  s += (val)?"OFF":"ON";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}
