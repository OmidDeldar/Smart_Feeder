#include <ESP8266WiFi.h>
#include <Servo.h>
const char* ssid = "Omid";
const char* password = "oM1234567890";
int ledPin = 13; // LED PIN GPIO13
int trig = D6;
int echo = D5;
float duration, distance;// declare two variable of type float for the time and the the distance
WiFiServer server(80);
Servo myservo;
void setup() {
  Serial.begin(115200);
  pinMode(trig, OUTPUT);// initialize trig as an output
  pinMode(echo, INPUT);// initialize echo as an input
  delay(10);
  // LED
  myservo.attach(D1);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
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
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  digitalWrite(trig, LOW);// set trig to LOW
  delayMicroseconds(2);// wait 2 microseconds
  digitalWrite(trig, HIGH);// set trig to HIGH
  delayMicroseconds(10);// wait 10 microseconds
  digitalWrite(trig, LOW);// set trig to LOW
  duration = pulseIn(echo, HIGH);// use the function pulsein to detect the time of the echo when it is in a high state
  distance = duration * 0.0343 / 2;// divide the time by 2 then multiply it by 0.0343
  Serial.print("Distance: ");// print "Distance: "
  Serial.print(distance);// print the value of distance
  Serial.println(" cm");// print " cm"
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
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  
  // Match the request
  // open SG90 for 5 milisecond
  if (request.indexOf("/FEED=ON") != -1)  {
    myservo.write(180); 
    digitalWrite(ledPin, HIGH);
    delay(500);
    myservo.write(0);
    digitalWrite(ledPin, LOW);
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("fedding now: ");
 
  // if(value == HIGH) {
  //   client.print("On");
  // } else {
  //   client.print("Off");
  // }
  client.println("<br><br>");
  client.println("<a href=\"/FEED=ON\"\"><button>feed </button></a>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}