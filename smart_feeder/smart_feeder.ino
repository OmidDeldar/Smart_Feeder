

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
 <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
 <meta charset='utf-8'>
 <style>
  body {font-size:140%; background-color: #374453;} 
  #main {display: table; margin: auto;  padding: 10px 100px 0 100px;   border-style: solid;
  border-color: #bfc7d1;}  

  #FEED_button { padding:10px 10px 10px 10px; width:100%;  background-color: #007400; font-size: 120%;}
  h2 {text-align:center; margin: 10px 0px 10px 0px;} 
  p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
  #time_P { margin: 10px 0px 15px 0px;}

 </style>

<script>
   function updateTime() 
  {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;

  }
   function updateCount() 
  {  
       ajaxLoad('getCount'); 
  }
//   function updateTemp() 
  //{  
  //     ajaxLoad('getTemp'); 
  //}
   function switchFeed() 
  {
       var button_text = document.getElementById("FEED_button").value;
       document.getElementById("FEED_button").value = "Feeding...";
       ajaxLoad('FeedOn'); 
       setTimeout(function() {
      document.getElementById("FEED_button").value = "Feed Now!";
      },500)

     
  }

var ajaxRequest = null;
if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }


function ajaxLoad(ajaxURL)
{
  if(!ajaxRequest){ alert("AJAX is not supported."); return; }
  
  ajaxRequest.open("GET",ajaxURL,true);
  ajaxRequest.onreadystatechange = function()
  {
    if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
    {
      var ajaxResult = ajaxRequest.responseText;
      var tmpArray = ajaxResult.split("|");
      if(tmpArray[2] == "Empty!"){
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('fed_time').innerHTML = t;
      }
      document.getElementById('count').innerHTML = tmpArray[0];
      document.getElementById('dst').innerHTML = tmpArray[1];
      document.getElementById('boul').innerHTML = tmpArray[2];
    }
  }
  ajaxRequest.send();
}
   // var myVar1 = setInterval(updateTemp, 5000);
    var myVar2 = setInterval(updateTime, 1000);    
</script>

 <title>Pet Feeder</title>
</head>
<body>
 <div id='main'>
  <h2>Pet Feeder</h2>
  <input type="button" id = "FEED_button" onclick="switchFeed()" value="Feed Now!"       /> 
  
 
 
     <div id='content'>
       <h2>Time</h2> 
       <p id='P_time'>-</p>
       <h2>Last fed time</h2>
       <p id='fed_time'>-</p>
       <h2>Manual feed</h2>
       <p> <span id='count'>0</span>  </p>
       <h2>App feed</h2>
       <p> <span id='appCount'>0</span>  </p>
       <h2>Storage</h2>
       <p> <span id='dst'>NA</span> % </p>
       <h2>Boul status</h2>
       <p> <span id='boul'>NA</span>  </p>
     </div>
     </div>
</body>
</html>
)=====";




#include <ESP8266WiFi.h>
#include <Servo.h>
// change these values to match your network
char ssid[] = "Omid";          //  your network SSID (name)
char pass[] = "oM1234567890";  //  your network password

WiFiServer server(80);
int trig = D6;
int echo = D5;
float duration, distance;  // declare two variable of type float for the time and the the distance
int charge;
String request = "";
int LED_Pin = 13;
const int ldrPin = A0;  // select the input pin for the potentiometer
int ldrValue = 0;       // variable to store the value coming from the sensor
Servo myservo;
int manualFeed = 0;
int boulStatus = 0;
void setup() {
  pinMode(LED_Pin, OUTPUT);
  myservo.attach(D1);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println("ESP8266_LED_CONTROL_AJAX_02");
  Serial.println();
  pinMode(trig, OUTPUT);  // initialize trig as an output
  pinMode(echo, INPUT);   // initialize echo as an input
  // Connect to a WiFi network
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println(F("[CONNECTED]"));
  Serial.print("[IP ");
  Serial.print(WiFi.localIP());
  Serial.println("]");

  // start a server
  server.begin();
  Serial.println("Server started");


}  // void setup()


void ultrasonic() {
  digitalWrite(trig, LOW);           // set trig to LOW
  delayMicroseconds(2);              // wait 2 microseconds
  digitalWrite(trig, HIGH);          // set trig to HIGH
  delayMicroseconds(10);             // wait 10 microseconds
  digitalWrite(trig, LOW);           // set trig to LOW
  duration = pulseIn(echo, HIGH);    // use the function pulsein to detect the time of the echo when it is in a high state
  distance = duration * 0.0343 / 2;  // divide the time by 2 then multiply it by 0.0343
  charge = 100 - ((distance * 100) / 20);
  if (charge < 0) {
    charge = 0;
  }
  // if(charge < 20){
  //   digitalWrite(LED_Pin, HIGH);
  // }
  // else{
  //   digitalWrite(LED_Pin, LOW);
  // }
  Serial.println(distance);
}
void boul() {
  ldrValue = analogRead(ldrPin);
  Serial.println(ldrValue);
  if (ldrValue > 190) {
    boulStatus = 0;
  } else {
    boulStatus = 1;
  }
}
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) { return; }

  // Read the first line of the request
  request = client.readStringUntil('\r');

  Serial.print("request: ");
  Serial.println(request);


  if (request.indexOf("FeedOn") > 0) {
    ultrasonic();
    boul();
    // charge = 100 - ((distance * 100) / 20);
    // Serial.println("charge: "+String(charge));
    // if (distance > 0) {
    //   client.print(header);
    //   client.print(distance);
    //   Serial.println("data sent");
    // }
    if (boulStatus == 0) {
      myservo.write(180);
      digitalWrite(LED_Pin, HIGH);
      manualFeed += 1;
      client.print(header);
      client.print(manualFeed);
      client.print("|");
      client.print(charge);
      client.print("|");
      client.print("Empty!");
      Serial.println("data sent");
      delay(500);
      myservo.write(0);
      digitalWrite(LED_Pin, LOW);
    } else {
      client.print(header);
      client.print(manualFeed);
      client.print("|");
      client.print(charge);
      client.print("|");
      client.print("Not empty!");
      Serial.println("data sent");
    }
  }
  else {
    boolean pinStatus = digitalRead(LED_Pin);

  client.flush();
  client.print(header);
  client.print(html_1);


  delay(5);
}


// The client will actually be disconnected when the function returns and 'client' object is detroyed
}  // void loop()
