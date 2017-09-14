#include "SimpleKalmanFilter.h"

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

const long SERIAL_REFRESH_TIME = 200;
long refresh_time;

SimpleKalmanFilter simpleKalmanFilter(0.1, 1, 0.005);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_ON_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(DUST_SENSOR_IN_PIN, INPUT);
  pinMode(OUT_VALUE_PIN, OUTPUT);
  Serial.begin(115200);

  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);

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
    
    
    Serial.print(readValue*255,2);
    Serial.print("  #  ");
    Serial.print(estimated_value*255,2);
    Serial.println();
    refresh_time = millis() + SERIAL_REFRESH_TIME;
  } 
  
}
