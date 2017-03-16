/****Includes****/
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the &onfiguration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>          //Allow custom URL

void setupServer();
void setupWifi();
void metronome();
void flash(int pin);
void handleRoot();
void setBpm();
void getBpm();

char* html_index = "\
<!DOCTYPE html><html><head><link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><meta charset='utf-8'>\
<meta http-equiv='X-UA-Compatible' content='IE=edge'> <meta name='viewport' content='width=device-width, initial-scale=1'>\
<title>IoT - Rythm'n Hits</title><style type='text/css'>body{padding-top: 50px;}.content{padding: 40px 15px; text-align: center;}</style></head>\
<body><nav class='navbar navbar-inverse navbar-fixed-top'> <div class='container'> <div class='navbar-header'> <a class='navbar-brand' href='#'>Rythm'n Hits</a> </div>\
</div></nav><div class='container'><div class='content'><h1>IoT Project - Rythm'n Hits</h1><br><br><h3>Actual BPM: <span id='actualBPM'></span></h3><h3>Update BPM</h3>\
<form method='get' action='http://10.33.0.77/setbpm' target=':blank'><br><input type='number' name='bpm'><br><input type='submit' value='Update BPM'></form></div>\
<script src='https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>\
<script>// get$.get('http://10.33.0.77/getbpm', function (data){$('#actualBPM').html(data);});</script></body></html>\
";

ESP8266WebServer server(80);
const char *ssid = "metronome_server";

long bpm = 60;                               //Beat Per Minutes
long beatCounter = 0;                       // Used to know where we are in the measure (in 4/4 there's 4 times in a measure)
const long msCountIn1Min = 60000;          //  Number of ms in 1 sec 
long waitBtwNotes = msCountIn1Min / bpm;  //   The wait beetwen each note
long lastTime = 0;                       //    Last time we checked the metronome time   

// the setup routine runs once when you press reset:
void setup() {

  //Init serial connection
  Serial.begin(9600);

  //init the pins 
  pinMode(A0, INPUT);     // Input for the tilt sensor 
  pinMode(D5, OUTPUT);   //  B -> RGB LED
  pinMode(D4, OUTPUT);  //   G -> RGB LED
  pinMode(D2, OUTPUT); //    Buzzer

  setupWifi();
  setupServer();
}

void loop() {

  metronome();   // here we handle the metronome code
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", html_index);
}

void setBpm() {
  String bpm_str;
  if ( server.hasArg("bpm") ) {

    bpm_str = server.arg(0);
    long old_bpm = bpm;
    bpm = bpm_str.toInt(); // if cast fail = 0

    if (bpm <= 0 || bpm > 250)  {
      bpm = old_bpm;
      server.send(400, "text/html", "error");
      return;
    }

    waitBtwNotes = msCountIn1Min / bpm;
  }

  server.send(200, "text/html", "OK");
}

void getBpm() {
  server.send(200, "text/html", String(bpm));
}

void setupWifi() {

  Serial.println("Starting wifi");
  WiFiManager wifiManager;
  wifiManager.setAPCallback(
  [](WiFiManager * manager) {
     Serial.println("Falling back to AP Mode");
  });

  wifiManager.autoConnect(ssid);
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.println("Wifi OK");
}

void setupServer() {
  server.on("/", handleRoot);
  server.on("/setbpm", setBpm);
  server.on("/getbpm", getBpm);
  server.begin();
  Serial.println("HTTP server started");
}

void metronome() {

  //when we its time for the next beat
  if ((millis() - lastTime) >= waitBtwNotes) {
    lastTime = millis();

    if (beatCounter % 4 == 0) {
      tone(D2, 440, 50);
      flash(D4);
    }
    else {
      tone(D2, 300, 50);
      flash(D5);
    }

    beatCounter++;
    if (beatCounter > 3) {
      beatCounter = 0;
    }
  }
}

void flash(int pin) {
  digitalWrite(pin, HIGH);
  delay(20);
  digitalWrite(pin, LOW);
}





