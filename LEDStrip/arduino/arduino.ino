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

#define PIN 5
#define PIN_BUTTON0 4
#define PIN_BUTTON1 3
#define PIN_BUTTON2 2
#define N_LEDS 30
const int MAX_MESSAGE_LENGTH = 64;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  pinMode(PIN_BUTTON0, INPUT);
  pinMode(PIN_BUTTON1, INPUT);
  pinMode(PIN_BUTTON2, INPUT);
  Serial.begin(9600);
}
char message[MAX_MESSAGE_LENGTH];
unsigned int messagePos = 0;

long hue = 0; // hue
long msDelay = 200;
long shortDelay = 40;
long step = 128;
int brightness = 255;
int currentAnimation = 3;
long color0 = 0;

bool lastButton0Pressed = true;
bool lastButton1Pressed = true;
bool lastButton2Pressed = true;
long button0PressTime = 0;
long button1PressTime = 0;
long button2PressTime = 0;
long button0ReleaseTime = 0;
long button1ReleaseTime = 0;
long button2ReleaseTime = 0;

void loop() {
  // Read from serial
  ReadFromSerial();
  DoAnimation();

  // Button 0
  bool button0Pressed = digitalRead(PIN_BUTTON0) == LOW;
  if(button0Pressed != lastButton0Pressed && button0Pressed) {
    button0ReleaseTime = millis();
    Serial.println("Button0 released");
    NextPattern();
    if(button0PressTime - millis() > 1000) {
      // turn strip on/off
      if(brightness > 0) brightness = 0;
      else brightness = 256;
      strip.setBrightness(brightness);
    }
  }
  if(button0Pressed != lastButton0Pressed && !button0Pressed) {
    button0PressTime = millis();
    Serial.println("Button0 pressed");
  }
  lastButton0Pressed = button0Pressed;

  // Button 1
  bool button1Pressed = digitalRead(PIN_BUTTON1) == LOW;
  if(button1Pressed != lastButton1Pressed && button1Pressed) {
    button1ReleaseTime = millis();
    Serial.println("Button1 released");
    hue += 2048;
    color0 = strip.gamma32(strip.ColorHSV(hue));
  }
  if(button1Pressed != lastButton1Pressed && !button1Pressed) {
    button1PressTime = millis();
    Serial.println("Button1 pressed");
  }
  lastButton1Pressed = button1Pressed;

  // Button 2
  bool button2Pressed = digitalRead(PIN_BUTTON2) == LOW;
  if(button2Pressed != lastButton2Pressed && button2Pressed) {
    button2ReleaseTime = millis();
    Serial.println("Button2 released");
  }
  if(button2Pressed != lastButton2Pressed && !button2Pressed) {
    button2PressTime = millis();
    Serial.println("Button2 pressed");
  }
  lastButton2Pressed = button2Pressed;
}

// Call this function whenever we can read from the serial buffer
void ReadFromSerial() {
  //return;
  while (Serial.available() > 0)
  {

    //Read the next available byte in the serial receive buffer
    char inByte = Serial.read();
    if(inByte != '\n')  {
      message[messagePos] = inByte;
      messagePos++;
    } else {
      // Message done, handle
      message[messagePos] = '\0';
      HandleSerialMsg(message);
      messagePos = 0;
    }
  }
}

void HandleSerialMsg(char data[]) {
  char* d;
  int i = 0;
  char* cmd;
  char* arg1;
  char* arg2;
  char* arg3;

  d = strtok(data, ",");
  i = 0;
  while (d != NULL) {
    if (i == 0)
      cmd = d;
    else if (i == 1)
      arg1 = d;
    else if (i == 2)
      arg2 = d;
    else if (i == 3)
      arg3 = d;
    
    d = strtok(NULL, ",");
    i++;
  }
  
  if (strcmp(cmd, "l") == 0) {
    // Set LED
    if (arg1 != NULL && arg2 != NULL) {
      currentAnimation = -1;
      long ledIndex = arg1[0];
      long color = arg2[0] * 0xFFFF + arg2[1] * 0xFF + arg2[2];
      strip.setPixelColor(ledIndex, strip.gamma32(color));
    }
  }
  else if(strcmp(cmd, "s") == 0) {
    // show
    strip.show();
  }
  else if(strcmp(cmd, "sc") == 0) {
    long colorIndex = strtol(arg1, NULL, 10);
    if(colorIndex == 0) {
      color0 = strtol(arg2, NULL, 16);
    }
  }
  else if (strcmp(cmd, "sa") == 0) {
    // Set animation
    if (arg1 != NULL) {
      currentAnimation = strtol(arg1, NULL, 10);
      Serial.print("Setting animation to: ");
      Serial.println(currentAnimation);
    }
  }
  else if (strcmp(cmd, "al") == 0) {
    // Set all LEDs
    if (arg1 != NULL) {
      currentAnimation = 6; // static
      color0 = strtol(arg1, NULL, 16);
    }
  }
  
  else if (strcmp(cmd, "debug") == 0) {
    Serial.print("Current animation: ");
    Serial.print(currentAnimation);
    Serial.println();
    
    Serial.print("hue: ");
    Serial.println(hue);

    Serial.print("color0: ");
    Serial.println(color0);
    
    Serial.print("Step: ");
    Serial.println(step);
  }
  
  else if (strcmp(cmd, "ss") == 0) {
    // Set step
    if (arg1 != NULL) {
      step = strtol(arg1, NULL, 10);
      Serial.print("Step set to: ");
      Serial.println(step);
    }
  } else if (strcmp(cmd, "sd") == 0) {
    // Set delay
    if (arg1 != NULL) {
      msDelay = strtol(arg1, NULL, 10);
      Serial.print("delay set to: ");
      Serial.println(msDelay);
    }
  } else if (strcmp(cmd, "ssd") == 0) {
    // Set short delay
    if (arg1 != NULL) {
      shortDelay = strtol(arg1, NULL, 10);
      Serial.print("short delay set to: ");
      Serial.println(shortDelay);
    }
  }
  else if (strcmp(cmd, "sb") == 0) {
    // Set brightness
    if (arg1 != NULL) {
      brightness = strtol(arg1, NULL, 10);
      strip.setBrightness(brightness);
      strip.show();
    }
  } else if(strcmp(cmd, "info") == 0) {
    Serial.print("numleds,");
    Serial.println(N_LEDS);

    Serial.print("step,");
    Serial.println(step);

    Serial.print("color0,");
    Serial.println(color0);
    Serial.print("msdelay,");
    Serial.println(msDelay);
    Serial.print("shortdelay,");
    Serial.println(shortDelay);
    Serial.print("animation,");
    Serial.println(currentAnimation);
    Serial.print("brightness,");
    Serial.println(brightness);
  }
}

int numAnimations = 9;

void NextPattern() {
  currentAnimation++;
  currentAnimation %= numAnimations;
}

void DoAnimation() {
  if(currentAnimation == 0) {
    RainbowLeftRightBounceMiddle();
  } else if(currentAnimation == 1) {
    RainbowLeftRightBounce();
  } else if(currentAnimation == 2) {
    RainbowLeftRight();
  } else if(currentAnimation == 3) {
    RainbowFade();
  } else if(currentAnimation == 4) {
    RainbowStatic();
  } else if(currentAnimation == 5) {
    Blink();
  } else if(currentAnimation == 6) {
    Static();
  } else if(currentAnimation == 7) {
    LeftTurnSignal();
  } else if(currentAnimation == 8) {
    RightTurnSignal();
  }
}

bool blinkOn = false;

void Static() {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color0);
  }
  strip.show();
}

u32 lastBlink = 0;
void Blink() {
  if(millis() - lastBlink > msDelay) {
    lastBlink = millis();
    blinkOn = !blinkOn;
    for(int i=0; i<strip.numPixels(); i++) { 
      strip.setPixelColor(i, blinkOn ? color0 : 0x000000);
    }
    strip.show();
  }
}

void LeftTurnSignal() {
  if(millis() - lastBlink > msDelay) {
    lastBlink = millis();
    blinkOn = !blinkOn;
    for(int i=0; i<strip.numPixels(); i++) { 
      if(i < 10) {
        strip.setPixelColor(i, blinkOn ? 0xFFFF00 : 0x000000);
      } else {
        strip.setPixelColor(i, 0x000000);
      }
    }
    strip.show();
  }
}

void RightTurnSignal() {
  if(millis() - lastBlink > msDelay) {
    lastBlink = millis();
    blinkOn = !blinkOn;
    for(int i=0; i<strip.numPixels(); i++) { 
      if(i > 20) {
        strip.setPixelColor(i, blinkOn ? 0xFFFF00 : 0x000000);
      } else {
        strip.setPixelColor(i, 0x000000);
      }
    }
    strip.show();
  }
}

void ResetVars() {

}

void RainbowStatic() {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
  }
  hue += step / 4;
  strip.show();
}

void RainbowFade() {
  for(int i=0; i<strip.numPixels(); i++) { 
    int pixelHue = hue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  hue += step / 4;
  strip.show();
}

bool goingRight = true;
u32 lastColorUpdate = 0;
int lastIndex = 0;
void RainbowLeftRight() {
  if(lastIndex >= strip.numPixels() || lastIndex < 0) lastIndex = 0;
  if(millis() - lastColorUpdate > shortDelay) {
    lastColorUpdate = millis();
    hue += step;
    strip.setPixelColor(lastIndex, strip.gamma32(strip.ColorHSV(hue)));
    strip.show();
    lastIndex++;
    if(lastIndex >= strip.numPixels()) {
      lastIndex = 0;
    }
  }
}

void RainbowLeftRightBounce() {
  if(millis() - lastColorUpdate > shortDelay) {
    lastColorUpdate = millis();
    if(goingRight) {
      hue += step;
      strip.setPixelColor(lastIndex, strip.gamma32(strip.ColorHSV(hue)));
      strip.show();
      lastIndex++;
      if(lastIndex >= strip.numPixels()) {
        goingRight = false;
        lastIndex = strip.numPixels();
      }
    } else if(lastIndex >= 0) {
      hue += step;
      strip.setPixelColor(lastIndex, strip.gamma32(strip.ColorHSV(hue)));
      strip.show();
      lastIndex--;
      if(lastIndex < 0) {
        goingRight = true;
        lastIndex = 0;
      }
    }
  }
}

void RainbowLeftRightBounceMiddle() {
  if(millis() - lastColorUpdate > shortDelay) {
    lastColorUpdate = millis();
    if(goingRight && lastIndex < strip.numPixels() / 2) {
      hue += step;
      strip.setPixelColor(lastIndex, strip.gamma32(strip.ColorHSV(hue)));
    strip.setPixelColor(strip.numPixels() - lastIndex, strip.gamma32(strip.ColorHSV(hue)));
      strip.show();
      lastIndex++;
      if(lastIndex >= strip.numPixels() / 2) {
        goingRight = false;
        lastIndex = strip.numPixels() / 2;
      }
    } else if(lastIndex >= 0) {
      hue += step;
      strip.setPixelColor(lastIndex, strip.gamma32(strip.ColorHSV(hue)));
    strip.setPixelColor(strip.numPixels() - lastIndex, strip.gamma32(strip.ColorHSV(hue)));
      strip.show();
      lastIndex--;
      if(lastIndex < 0) {
        goingRight = true;
        lastIndex = 0;
      }
    }
  }
}