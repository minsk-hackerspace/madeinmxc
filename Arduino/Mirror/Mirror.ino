
//#define USE_FadeLed 1 //switch beetwen two Fade libs

#ifdef USE_FadeLed
#include <FadeLed.h> // https://github.com/septillion-git/FadeLed
#else
#include <LEDFader.h> //https://github.com/jgillick/arduino-LEDFader
#include <Curve.h>
#endif

#include <SimpleKalmanFilter.h> //https://github.com/denyssene/SimpleKalmanFilter

const int analogInPin = A0;  // Analog input pin that the proximyty sensor (sharp) is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to
const int onboardLedPin = 13; // Analog output pin that the onboard LED is attached to

int sensorValue = 0;        // value read from the sharp

const int fadeMills = 700; //milliseconds to fade

#ifdef USE_FadeLed
FadeLed mainLed1(analogOutPin);
FadeLed statusLed1(onboardLedPin);
#else
LEDFader mainLed2 = LEDFader(analogOutPin);
LEDFader statusLed2 = LEDFader(onboardLedPin);
#endif

SimpleKalmanFilter simpleKalmanFilter(30, 30, 0.0003);

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(onboardLedPin, OUTPUT);
  pinMode(analogOutPin, OUTPUT);
  pinMode(analogInPin, INPUT);


#ifdef USE_FadeLed
  mainLed1.setTime(fadeMills);
  statusLed1.setTime(fadeMills);
#else
  mainLed2.set_curve(Curve::exponential);
  statusLed2.set_curve(Curve::exponential);
  mainLed2.fade(0, 1);
  statusLed2.fade(0, 1);
#endif
    
  
}


const long REFRESH_TIMEOUT = 100;
unsigned long refresh_time;
bool onState = false;

void loop() {

#ifdef USE_FadeLed
  FadeLed::update();
#else
  mainLed2.update();
  statusLed2.update();
#endif
  
  float sensorValue = (float) analogRead(A0);
  
  float filteredValue = simpleKalmanFilter.updateEstimate(sensorValue);

  if (millis() > refresh_time) {
    
    //Serial.print(sensorValue,4);
    //Serial.print(",");
    //Serial.print(filteredValue,4);
    //Serial.println();
    
    refresh_time = millis() + REFRESH_TIMEOUT;

      if (filteredValue > 130 && onState == false ) {
        on();
      } else if (filteredValue < 100 && onState == true ) {
        off();
      }
    
  }

}

void on() {
   onState = true;
   Serial.println(" - ");
   Serial.println(" ON ");
   Serial.println(" - ");

#ifdef USE_FadeLed
   mainLed1.on();
   statusLed1.on(); 
#else
   mainLed2.fade(255, fadeMills);
   statusLed2.fade(255, fadeMills);
#endif 

}

void off() {
   onState = false; 
   Serial.println(" - ");
   Serial.println(" OFF ");
   Serial.println(" - ");

#ifdef USE_FadeLed
   mainLed1.off();
   statusLed1.off(); 
#else
   mainLed2.fade(0, fadeMills);
   statusLed2.fade(0, fadeMills);
#endif 

}

