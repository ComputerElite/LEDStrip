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
#define N_LEDS 30
const int MAX_MESSAGE_LENGTH = 64;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  Serial.begin(9600);
}
char message[MAX_MESSAGE_LENGTH];
unsigned int messagePos = 0;

long hue = 0; // hue
long msDelay = 200;
long shortDelay = 40;
long step = 32;
int brightness = 255;
char currentAnimation[16] = "rbf";
long color0 = 0;

void loop() {
  // Read from serial
  ReadFromSerial();
  DoAnimation();
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
      /*
      Serial.print("msg: ");
      Serial.println(message);
      */
      HandleSerialMsg(message);
      messagePos = 0;
    }
  }
}

void SetAnimation(char anim[]) {
  strncpy(currentAnimation, anim, sizeof(currentAnimation) - 1);
  currentAnimation[sizeof(currentAnimation) - 1] = '\0';
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
      SetAnimation("none");
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
      strncpy(currentAnimation, arg1, sizeof(currentAnimation) - 1);
      currentAnimation[sizeof(currentAnimation) - 1] = '\0';
      Serial.print("Setting animation to: ");
      Serial.println(currentAnimation);
    }
  }
  else if (strcmp(cmd, "sb") == 0) {
    // Set brightness
    if (arg1 != NULL) {
      brightness = strtol(arg1, NULL, 10);
      strip.setBrightness(brightness);
    }
  }
  else if (strcmp(cmd, "al") == 0) {
    // Set all LEDs
    if (arg1 != NULL) {
      SetAnimation("static");
      color0 = strtol(arg1, NULL, 16);
    }
  }
  
  else if (strcmp(cmd, "debug") == 0) {
    Serial.print("Current animation: ");
    Serial.print(currentAnimation);
    Serial.print(" (length: ");
    Serial.print(sizeof(currentAnimation));
    Serial.println(")");
    
    Serial.print("Comparison result: ");
    Serial.println(strcmp(currentAnimation, "rbf"));

    Serial.print("Current animation (debug): ");
    for (int i = 0; i < strlen(currentAnimation); i++) {
      Serial.print(currentAnimation[i]);
    }
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

void DoAnimation() {
  if(strcmp(currentAnimation, "rlbm") == 0) {
    RainbowLeftRightMiddle();
  } else if(strcmp(currentAnimation, "rlb") == 0) {
    RainbowLeftRight();
  } else if(strcmp(currentAnimation, "rbf") == 0) {
    RainbowFade();
  } else if(strcmp(currentAnimation, "rbs") == 0) {
    RainbowStatic();
  } else if(strcmp(currentAnimation, "blk") == 0) {
    Blink();
  } else if(strcmp(currentAnimation, "static") == 0) {
    Static();
  } else if(strcmp(currentAnimation, "static") == 0) {
    Static();
  }
}

bool blinkOn = false;

void Static() {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color0);
  }
  strip.show();
}

void Blink() {
  blinkOn = !blinkOn;
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, blinkOn ? color0 : 0x000000);
  }
  strip.show();
  delay(msDelay);
}

void ResetVars() {

}

void RainbowStatic() {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
  }
  hue += step;
  strip.show();
}

void RainbowFade() {
  for(int i=0; i<strip.numPixels(); i++) { 
    int pixelHue = hue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  hue += step;
  strip.show();
}

void RainbowLeftRight() {
  for(int i = 0; i < strip.numPixels(); i++) {
    hue += step;
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
    strip.show();
    delay(shortDelay);
  }
  for(int i = strip.numPixels(); i >= 0; i--) {
    hue += step;
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
    strip.show();
    delay(shortDelay);
  }
}

void RainbowLeftRightMiddle() {
  for(int i = 0; i < strip.numPixels() / 2; i++) {
    hue += step;
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
    strip.setPixelColor(strip.numPixels() - i, strip.gamma32(strip.ColorHSV(hue)));
    strip.show();
    delay(shortDelay);
  }
  for(int i = strip.numPixels() / 2; i >= 0; i--) {
    hue += step;
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
    strip.setPixelColor(strip.numPixels() - i, strip.gamma32(strip.ColorHSV(hue)));
    strip.show();
    delay(shortDelay);
  }
}