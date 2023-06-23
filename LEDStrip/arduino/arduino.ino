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
#define ANIMATION_RAINBOW_FADE 3
#define ANIMATION_SET_ALARM_HOUR 100
#define ANIMATION_SET_ALARM_QUARTER_HOUR 101
#define ANIMATION_SET_ALARM_COLOR 102
#define ANIMATION_ALARM 103

const int MAX_MESSAGE_LENGTH = 64;

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
  Serial.begin(9600);
}
char message[MAX_MESSAGE_LENGTH];
unsigned int messagePos = 0;

uint16_t hue = 0; // hue
long msDelay = 200;
long shortDelay = 40;
double step = 128;
double brightness = 255;
int currentAnimation = 3;
long color0 = 0xFFFFFF;

bool lastButton0Pressed = false;
bool lastButton1Pressed = false;
bool lastButton2Pressed = false;
bool lastButton3Pressed = false;
long button0PressTime = 0;
long button1PressTime = 0;
long button2PressTime = 0;
long button3PressTime = 0;
long button0ReleaseTime = 0;
long button1ReleaseTime = 0;
long button2ReleaseTime = 0;
long button3ReleaseTime = 0;

long alarmHour = 8;
long alarmQuarterHour = 0;
long alarmTime = 0;
long alarmSetTime = 0;
double alarmLength = 1000.0 * 10.0 * 60.0; // 10 minutes
long alarmType = 0;
long alarmShowRemainingPressTime = 0;

bool button0DidSomething = false;
bool lastBrightnessDirectionWasUp = false;
bool lastColorDirectionWasUp = false;

long lastBrightnessChange = 0;
long lastLoop = 0;
double brightnessDirection = -1;
double colorDirection = 1;
bool stripOn = true;
int savedBrightness = 0;

bool blockNextButton0Release = false;

// Turn signal
bool leftSignalOn = false;
bool rightSignalOn = false;

uint32_t blinkColor = 0xFF6600;

bool blinkOn = false;
long blinkDelay = 300;

bool useTurnSignal = false;

u32 lastBlink = 0;

void loop() {
  // Read from serial
  ReadFromSerial();
  if(useTurnSignal) {
    ReadTurnSignalButtons();
    DoTurnSignalAnimation();
  } else {
    DoAnimation();
    ReadNormalButtons();
  }
}

void DoTurnSignalAnimation() {
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
        if((i < 30 && i >= 24 || i < 7) && blinkOn) {
            strip.setPixelColor(i, blinkColor);
        }
    }
}

void RightTurnSignal() {
    if(!rightSignalOn) return;
    for(int i=0; i<strip.numPixels(); i++) { 
        if((i >= 30 && i <= 35 || i >= 53) && blinkOn) {
            strip.setPixelColor(i, blinkColor);
        }
    }
}

void ReadTurnSignalButtons() {
  if(millis() < 1000) return; // ignore first second
  // Button 0
  bool button0Pressed = digitalRead(PIN_BUTTON0) == HIGH;
  if(button0Pressed != lastButton0Pressed && !button0Pressed) {
    button0ReleaseTime = millis();
    useTurnSignal = false;
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

void ReadNormalButtons() {
  // Button 0
  // Press: Next Animation or turn LED Strip on if off
  // Hold >1s: Turn off/on LED strip
  bool button0Pressed = digitalRead(PIN_BUTTON0) == HIGH;
  if(button0Pressed != lastButton0Pressed && !button0Pressed) {
    button0ReleaseTime = millis();
    if(blockNextButton0Release) {
      blockNextButton0Release = false;
    } else {
      // Button 0 released
      if(SettingAlarm()) {
        currentAnimation++;
        if(currentAnimation == ANIMATION_ALARM) {
          alarmSetTime = millis();
          savedBrightness = brightness;
          alarmTime = millis() + alarmHour * 1000 * 60 * 60 + alarmQuarterHour * 1000 * 60 * 15;
        } else if(currentAnimation == ANIMATION_ALARM) {
          currentAnimation = ANIMATION_RAINBOW_FADE;
          digitalWrite(LED_BUILTIN, LOW);
          SetBrightness(savedBrightness);
        }
      } else if(!button0DidSomething) {
        // Display next Animation
        NextAnimation();
      }
    }
    
  }
  if(button0Pressed && button0PressTime > button0ReleaseTime) {
    if(millis() - button0PressTime > 1000 && millis() - button0PressTime < 2000 && !button0DidSomething) {
      button0DidSomething = true;
      // turn strip on/off
      stripOn = !stripOn;
      if(!stripOn) {
        // Save brightness for turning on again
        savedBrightness = brightness;
        SetBrightness(0);
      } else {
        SetBrightness(savedBrightness);
      }
    } else if(millis() - button0PressTime > 2000) {
      // Start setting alarm
      SetBrightness(savedBrightness);
      stripOn = true;
      currentAnimation = ANIMATION_SET_ALARM_HOUR;
      blockNextButton0Release = true;
    }
  }
  if(button0Pressed != lastButton0Pressed && button0Pressed) {
    // Button 0 pressed
    button0PressTime = millis();
    button0DidSomething = false;
  }
  lastButton0Pressed = button0Pressed;

  // Button 1
  // Hold: change color0 up
  // Hold after double tab: change color0 down
  bool button1Pressed = digitalRead(PIN_BUTTON1) == HIGH;
  if(button1Pressed != lastButton1Pressed && !button1Pressed) {
    button1ReleaseTime = millis();
    // Button 1 released
    if(SettingAlarm()) {
      if(currentAnimation == ANIMATION_SET_ALARM_HOUR) {
        alarmHour--;
        if(alarmHour < 0) alarmHour = 0;
      } else if(currentAnimation == ANIMATION_SET_ALARM_QUARTER_HOUR) {
        alarmQuarterHour--;
        if(alarmQuarterHour < 0) alarmQuarterHour = 0;
      }
    }
  }
  if(button1Pressed) {
    if(!SettingAlarm()) {
      if(UseStepInsteadOfColor0()) {
        // On some animations color can't be controlled. In that case adjust step
        step += (millis() - lastLoop) * colorDirection;
      } else {
        hue += (millis() - lastLoop) * 10 * colorDirection;
        color0 = strip.gamma32(strip.ColorHSV(hue));
      }
    }
  }
  if(button1Pressed != lastButton1Pressed && button1Pressed) {
    button1PressTime = millis();
    // Button 1 pressed
    if(!SettingAlarm()) {
      // When not setting alarm
      if(lastColorDirectionWasUp) button1ReleaseTime = 0;
      if(button1PressTime - button1ReleaseTime < 150) {
        // Set brightness to be changed up
        colorDirection = -1;
        lastColorDirectionWasUp = true;
      } else {
        // Set brightness to be changed down
        colorDirection = 1;
        lastColorDirectionWasUp = false;
      }
    }
  }
  lastButton1Pressed = button1Pressed;



  // Button 2
  // Hold: Change brightness down
  // Double tap then hold: Change brightness up
  bool button2Pressed = digitalRead(PIN_BUTTON2) == HIGH;
  if(button2Pressed != lastButton2Pressed && !button2Pressed) {
    button2ReleaseTime = millis();
    // Button 2 released
    if(SettingAlarm()) {
      if(currentAnimation == ANIMATION_SET_ALARM_HOUR) {
        alarmHour++;
        if(alarmHour > 24) alarmHour = 24;
      } else if(currentAnimation == ANIMATION_SET_ALARM_QUARTER_HOUR) {
        alarmQuarterHour++;
        if(alarmQuarterHour > 3) alarmQuarterHour = 3;
      } else if(currentAnimation == ANIMATION_ALARM) {
        alarmShowRemainingPressTime = millis();
      }
    }
  }
  // While pressed do every 50 ms
  if(button2Pressed && millis() - lastBrightnessChange > 10) {
    if(!SettingAlarm()) {
      SetBrightness(brightness + brightnessDirection);
      lastBrightnessChange = millis();
    }
  }

  // When button is pressed down
  if(button2Pressed != lastButton2Pressed && button2Pressed) {
    button2PressTime = millis();
    // Button 2 pressed
    if(!SettingAlarm()) {
      // When not sending alarm
      // For double tab reset double tab counter after double tab has been registered
      if(lastBrightnessDirectionWasUp) button2ReleaseTime = 0;
      if(button2PressTime - button2ReleaseTime < 150) {
        // Set brightness to be changed up
        brightnessDirection = .3;
        lastBrightnessDirectionWasUp = true;
      } else {
        // Set brightness to be changed down
        brightnessDirection = -.8;
        lastBrightnessDirectionWasUp = false;
      }
    }

    
  }
  lastButton2Pressed = button2Pressed;


// Button 3
  // Right Turn
  bool button3Pressed = digitalRead(PIN_BUTTON3) == HIGH;
  if(button3Pressed != lastButton3Pressed && !button3Pressed) {
    button3ReleaseTime = millis();
    useTurnSignal = true;
  }

  // When button is pressed down
  if(button3Pressed != lastButton3Pressed && button3Pressed) {
    button3PressTime = millis();
    // Button 3 pressed
    
  }
  lastButton3Pressed = button3Pressed;

  lastLoop = millis();
}

bool SettingAlarm() {
  return currentAnimation == ANIMATION_SET_ALARM_HOUR || currentAnimation == ANIMATION_SET_ALARM_QUARTER_HOUR || currentAnimation == ANIMATION_SET_ALARM_COLOR || currentAnimation == ANIMATION_ALARM;
}

void SetBrightness(double b) {
  if(b < 0) b = 0;
  if(b > 255) b = 255;
  brightness = b;
  strip.setBrightness((int)round(brightness));
  strip.show();
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
    Serial.println((int)round(step));
  }
  
  else if (strcmp(cmd, "ss") == 0) {
    // Set step
    if (arg1 != NULL) {
      step = static_cast<double>(strtol(arg1, NULL, 10));
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
      SetBrightness(strtol(arg1, NULL, 10));
      strip.show();
    }
  } else if(strcmp(cmd, "info") == 0) {
    Serial.print("numleds,");
    Serial.println(N_LEDS);

    Serial.print("step,");
    Serial.println((int)round(step));

    Serial.print("color0,");
    Serial.println(color0);
    Serial.print("msdelay,");
    Serial.println(msDelay);
    Serial.print("shortdelay,");
    Serial.println(shortDelay);
    Serial.print("animation,");
    Serial.println(currentAnimation);
    Serial.print("brightness,");
    Serial.println((int)round(brightness));
  }
}

int numAnimations = 9;

void NextAnimation() {
  if(millis() < 1000) return;
  currentAnimation++;
  currentAnimation %= numAnimations;
}

int useStepInsteadOfColor0[] = {0, 1, 2, 3, 4};
bool UseStepInsteadOfColor0() {
  for(int i: useStepInsteadOfColor0) {
    if(i == currentAnimation) return true;
  }
  return false;
}

void DoAnimation() {
  digitalWrite(PIN_LED05V, LOW);
  if(currentAnimation == 0) {
    RainbowLeftRightBounceMiddle();
  } else if(currentAnimation == 1) {
    RainbowLeftRightBounce();
  } else if(currentAnimation == 2) {
    RainbowLeftRight();
  } else if(currentAnimation == ANIMATION_RAINBOW_FADE) {
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
  } else if(currentAnimation == ANIMATION_SET_ALARM_HOUR) {
    AlarmSetHour();
  } else if(currentAnimation == ANIMATION_SET_ALARM_QUARTER_HOUR) {
    AlarmSetQuarterHour();
  } else if(currentAnimation == ANIMATION_ALARM) {
    Alarm();
  }
}

double EaseInCubic(double t) {
  return t * t * t;
}

void Alarm() {
  if(millis() > alarmTime) {
    digitalWrite(LED_BUILTIN, HIGH);
    double deltaTime = static_cast<double>(millis() - alarmTime);
    if(alarmType == 0) {
      long b = (long)round(EaseInCubic(deltaTime / alarmLength) * 254) + 1;
      if(b > 255) b = 255;
      for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0xFF922C);
      }
      SetBrightness(b);
    }
  } else {
    // not time for alarm, keep everything off
    if(millis() - alarmSetTime < 4000) {
      for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0x00FF11);
      }
      // blink animation with sin
      double deltaTime = static_cast<double>(millis() - alarmSetTime);
      double b = sin(deltaTime / 2000.0 * PI) * 30.0;
      if(b < 0) b *= -1;
      SetBrightness(b);
    } else if (millis() - alarmShowRemainingPressTime < 4000){
      double progress = 1.0 - static_cast<double>(millis() - alarmSetTime) / static_cast<double>(alarmTime - alarmSetTime);
      for(int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, i < progress * strip.numPixels() ? 0x00FF11 : 0x000000);
      }
      SetBrightness(20);
    } else {
      SetBrightness(0);
    }
  }
}

void AlarmSetHour() {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, i < alarmHour ? 0xFFFF00 : 0x000000);
  }
  strip.show();
}

void AlarmSetQuarterHour() {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, i < alarmQuarterHour ? 0x00FFFF : 0x000000);
  }
  strip.show();
}

void Static() {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color0);
  }
  strip.show();
}

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

void ResetVars() {

}

double GetStepForTime() {
  return step * (millis() - lastLoop);
}

void RainbowStatic() {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
  }
  hue += static_cast<long>(GetStepForTime() / 4.0);
  strip.show();
}

void RainbowFade() {
  for(int i=0; i<strip.numPixels(); i++) { 
    int pixelHue = hue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  hue += static_cast<long>(GetStepForTime() / 4.0);
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