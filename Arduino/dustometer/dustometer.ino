#include "SimpleKalmanFilter.h"
// #include <ESP8266WiFi.h>

#include <ESP8266WiFi.h>

char ssid[] = "hackerspace";          //  your network SSID (name) 
char pass[] = "....";   // your network password
int status = WL_IDLE_STATUS;
IPAddress server(74,125,115,105);  // Google

// Initialize the client library
WiFiClient client;


int MICROS_LED_IMPULSE = 320; //micros
int MICROS_READ_DELAY = 280; //micros
int MICROS_CYCLE_DELAY = 10000; //ms
int GREEN_LED_PIN = D8; 
int RED_LED_PIN = D7; 
float ALERT_TRESHOLD = 250.0/255;
float GREEN_TRESHOLD = 70.0/255;


int LED_ON_PIN = D2;
int DUST_SENSOR_IN_PIN = A0;

const int OUT_VALUE_PIN = 2;

const long SERIAL_REFRESH_TIME = 60*1000;
long refresh_time;

SimpleKalmanFilter simpleKalmanFilter(0.1, 1, 0.005);

void setup() {
   Serial.begin(115200);

   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, pass);
    
   Serial.print("Connecting to ");
   Serial.println(ssid);
   
   
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected!");


  
    // put your setup code here, to run once:
    pinMode(LED_ON_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(DUST_SENSOR_IN_PIN, INPUT);
    pinMode(OUT_VALUE_PIN, OUTPUT);
  
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
  
}


char host[]="192.168.128.8";//http://192.168.128.8/api/events/dust?value=66

void sendData(float data){
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/api/events/dust?value=";
  url += String(int(data*255));
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  }


void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_ON_PIN, 0);
  delayMicroseconds(MICROS_READ_DELAY);
  
  int result = analogRead(DUST_SENSOR_IN_PIN);
  
  delayMicroseconds(MICROS_LED_IMPULSE-MICROS_READ_DELAY);
  digitalWrite(LED_ON_PIN, 1);

  

  delayMicroseconds(MICROS_CYCLE_DELAY-MICROS_LED_IMPULSE);
  
  float readValue = 1.0 * result / 1024;
  float estimated_value = simpleKalmanFilter.updateEstimate(readValue);
  
  analogWrite(OUT_VALUE_PIN, estimated_value*1024);
  
  if (millis() > refresh_time) {

    digitalWrite(GREEN_LED_PIN, estimated_value > GREEN_TRESHOLD ? HIGH:LOW); 
    digitalWrite(RED_LED_PIN,   estimated_value > ALERT_TRESHOLD ? HIGH:LOW);
    sendData(estimated_value);
    
    Serial.print(readValue*255,2);
    Serial.print("  #  ");
    Serial.print(estimated_value*255,2);
    Serial.println();
    refresh_time = millis() + SERIAL_REFRESH_TIME;
  } 
  
}
