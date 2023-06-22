// Simple NeoPixel test.  Lights just a few pixels at a time so a
// 1m strip can safely be powered from Arduino 5V pin.  Arduino
// may nonetheless hiccup when LEDs are first connected and not
// accept code.  So upload code first, unplug USB, connect pixels
// to GND FIRST, then +5V and digital pin 6, then re-plug USB.
// A working strip will show a few pixels moving down the line,
// cycling between red, green and blue.  If you get no response,
// might be connected to wrong end of strip (the end wires, if
// any, are no indication -- look instead for the data direction
// arrows printed on the strip).

#include <Adafruit_NeoPixel.h>

#define PIN 6
#define PIN_BUTTON0 2
#define PIN_BUTTON1 3
#define PIN_BUTTON2 4
#define PIN_BUTTON3 5
#define PIN_LED05V 7
#define PIN_LED0GND 8
#define N_LEDS 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  pinMode(PIN_BUTTON0, INPUT);
  pinMode(PIN_BUTTON1, INPUT);
  pinMode(PIN_BUTTON2, INPUT);
  pinMode(PIN_BUTTON3, INPUT);
  pinMode(PIN_LED0GND, OUTPUT);
  digitalWrite(PIN_LED0GND, LOW);
  pinMode(PIN_LED05V, OUTPUT);
}

bool lastButton0Pressed = true;
bool lastButton1Pressed = true;
bool lastButton2Pressed = true;
bool lastButton3Pressed = true;
long button0PressTime = 0;
long button1PressTime = 0;
long button2PressTime = 0;
long button3PressTime = 0;
long button0ReleaseTime = 0;
long button1ReleaseTime = 0;
long button2ReleaseTime = 0;
long button3ReleaseTime = 0;


bool leftSignalOn = false;
bool rightSignalOn = false;

uint32_t blinkColor = 0xFF6600;

bool blinkOn = false;
long blinkDelay = 300;

long lastLoop = 0;
u32 lastBlink = 0;

void loop() {
  // Read from serial
  ReadButtons();
  DoAnimation();
}

void ReadButtons() {
  if(millis() < 1000) return; // ignore first second
  // Button 0
  bool button0Pressed = digitalRead(PIN_BUTTON0) == HIGH;
  if(button0Pressed != lastButton0Pressed && !button0Pressed) {
    button0ReleaseTime = millis();
  }
  if(button0Pressed != lastButton0Pressed && button0Pressed) {
    // Button 0 pressed
    button0PressTime = millis();
  }
  lastButton0Pressed = button0Pressed;



  // Button 1
  // Danger light
  bool button1Pressed = digitalRead(PIN_BUTTON1) == HIGH;
  if(button1Pressed != lastButton1Pressed && !button1Pressed) {
    // Button 1 released
    button1ReleaseTime = millis();
    bool setTo = true;
    if(leftSignalOn && rightSignalOn) setTo = false;
    leftSignalOn = setTo;
    rightSignalOn = setTo;
    blinkOn = false;
    lastBlink = 0;
  }
  if(button1Pressed != lastButton1Pressed && button1Pressed) {
    button1PressTime = millis();
    // Button 1 pressed
    
  }
  lastButton1Pressed = button1Pressed;



  // Button 2
  // Left Turn
  bool button2Pressed = digitalRead(PIN_BUTTON2) == HIGH;
  if(button2Pressed != lastButton2Pressed && !button2Pressed) {
    button2ReleaseTime = millis();
    // Button 2 released
    leftSignalOn = !leftSignalOn;
    rightSignalOn = false;
    blinkOn = false;
    lastBlink = 0;
  }

  // When button is pressed down
  if(button2Pressed != lastButton2Pressed && button2Pressed) {
    button2PressTime = millis();
    // Button 2 pressed
    
  }
  lastButton2Pressed = button2Pressed;



  // Button 3
  // Right Turn
  bool button3Pressed = digitalRead(PIN_BUTTON3) == HIGH;
  if(button3Pressed != lastButton3Pressed && !button3Pressed) {
    button3ReleaseTime = millis();
    // Button 3 released
    rightSignalOn = !rightSignalOn;
    leftSignalOn = false;
    blinkOn = false;
    lastBlink = 0;
  }

  // When button is pressed down
  if(button3Pressed != lastButton3Pressed && button3Pressed) {
    button3PressTime = millis();
    // Button 3 pressed
    
  }
  lastButton3Pressed = button3Pressed;


  lastLoop = millis();
}

void DoAnimation() {
    UpdateBlink();
    ClearAll();
    AddBrakeLight();
    LeftTurnSignal();
    RightTurnSignal();
    // Enable LED if any turn signal is on
    digitalWrite(PIN_LED05V, leftSignalOn || rightSignalOn ? HIGH : LOW);


    strip.show();
}

void AddBrakeLight() {
    // Brake light
    for(int i=strip.numPixels()/2 -4; i<strip.numPixels()/2+4; i++) { 
        strip.setPixelColor(i, 0xFF0000);
    }
}

void ClearAll() {
    for(int i=0; i<strip.numPixels(); i++) { 
        strip.setPixelColor(i, 0x000000);
    }
}

void UpdateBlink() {
    if(millis() - lastBlink > blinkDelay) {
        lastBlink = millis();
        blinkOn = !blinkOn;
    }
}

void LeftTurnSignal() {
    if(!leftSignalOn) return;
    for(int i=0; i<strip.numPixels(); i++) { 
        if(i < 20 && blinkOn) {
            strip.setPixelColor(i, blinkColor);
        }
    }
}

void RightTurnSignal() {
    if(!rightSignalOn) return;
    for(int i=0; i<strip.numPixels(); i++) { 
        if(i >= 40 && blinkOn) {
            strip.setPixelColor(i, blinkColor);
        }
    }
}