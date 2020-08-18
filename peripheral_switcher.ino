#include <Servo.h>

const int usbInput1Pin = 2;
const int usbInput2Pin = 3;
const int hdmiInput1Pin = 4;
const int hdmiInput2Pin = 5;
const int buttonPin = 6;
const int hdmiSwitch = 7;
const int usbSwitch = 8;
const int device1IndicatorLedPin = 9;
const int device2IndicatorLedPin = 10;
const int errorIndicatorLedPin = 11;
const int servoSignalPin = 12;

const int defaultSwitchState = HIGH;
int hdmiSwitchState = defaultSwitchState;
int usbSwitchState = defaultSwitchState;
int buttonState;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long lastHdmiSwitchTime = 0;
unsigned long lastUsbSwitchTime = 0;
unsigned long switchOnDelay = 150;

Servo audioSwitchServo;

const int servoPos1 = 0;
const int servoPos2 = 90;

void setup() {
  pinMode(usbInput1Pin, INPUT);
  pinMode(usbInput2Pin, INPUT);
  pinMode(hdmiInput1Pin, INPUT);
  pinMode(hdmiInput2Pin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(hdmiSwitch, OUTPUT);
  pinMode(usbSwitch, OUTPUT);
  pinMode(device1IndicatorLedPin, OUTPUT);
  pinMode(device2IndicatorLedPin, OUTPUT);
  pinMode(errorIndicatorLedPin, OUTPUT);

  digitalWrite(hdmiSwitch, hdmiSwitchState);
  digitalWrite(usbSwitch, usbSwitchState);
  digitalWrite(device1IndicatorLedPin, LOW);
  digitalWrite(device2IndicatorLedPin, LOW);
  digitalWrite(errorIndicatorLedPin, LOW);

  audioSwitchServo.attach(servoSignalPin);
}

void loop() {
  alignSwitches(true);

  if (buttonHasBeenPressed()) {
    switchHdmi();
    switchUsb();
    switchAux();
  } else {
    turnOnIndicatorLed();
  }

  resetTimersIfNeeded();

  digitalWrite(hdmiSwitch, hdmiSwitchState);
  digitalWrite(usbSwitch, usbSwitchState);
}

bool pcHasUsb() {
  return digitalRead(usbInput1Pin) == HIGH;
}

bool laptopHasUsb() {
  return digitalRead(usbInput2Pin) == HIGH;
}

bool pcHasHdmi() {
  return digitalRead(hdmiInput1Pin) == HIGH;
}

bool laptopHasHdmi() {
  return digitalRead(hdmiInput2Pin) == HIGH;
}

void alignSwitches(bool retry) {
  if ((pcHasHdmi() && pcHasUsb()) ||
      (laptopHasHdmi() && laptopHasUsb())) {
    return;
  }

  if (retry) {
    switchUsb();

    alignSwitches(false);
  }
}

void switchUsb() {
  lastUsbSwitchTime = millis();

  usbSwitchState = !defaultSwitchState;
}

void switchHdmi() {
  lastHdmiSwitchTime = millis();

  hdmiSwitchState = !defaultSwitchState;
}

void switchAux() {
  int currentPosition = audioSwitchServo.read();

  if (currentPosition == servoPos1) {
    audioSwitchServo.write(servoPos2);
  } else {
    audioSwitchServo.write(servoPos1);
  }
}

bool buttonHasBeenPressed() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  lastButtonState = reading;

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        return true;
      }
    }
  }

  return false;
}

void resetTimersIfNeeded() {
  if (hdmiSwitchState != defaultSwitchState &&
      (millis() - lastHdmiSwitchTime) > switchOnDelay) {
    hdmiSwitchState = defaultSwitchState;
  }

  if (usbSwitchState != defaultSwitchState &&
      (millis() - lastUsbSwitchTime) > switchOnDelay) {
    usbSwitchState = defaultSwitchState;
  }
}

void turnOnIndicatorLed() {
  if (pcHasHdmi() && pcHasUsb()) {
    digitalWrite(device1IndicatorLedPin, HIGH);
    digitalWrite(device2IndicatorLedPin, LOW);
    digitalWrite(errorIndicatorLedPin, LOW);
  } else if (laptopHasHdmi() && laptopHasUsb()) {
    digitalWrite(device2IndicatorLedPin, HIGH);
    digitalWrite(device1IndicatorLedPin, LOW);
    digitalWrite(errorIndicatorLedPin, LOW);
  } else {
    digitalWrite(errorIndicatorLedPin, HIGH);
    digitalWrite(device1IndicatorLedPin, LOW);
    digitalWrite(device2IndicatorLedPin, LOW);
  }
}

void logOut() {
  Serial.println("==================");
  Serial.println();
  Serial.println("|  HDMI  |  USB  |");
  Serial.println("|--------|-------|");
  if (pcHasHdmi()) {
    if (laptopHasHdmi()) {
      Serial.print("| PC LAP |");
    } else {
      Serial.print("|   PC   |");
    }
  } else if (laptopHasHdmi()) {
    Serial.print("|  LAP   |");
  } else {
    Serial.print("|  NONE  |");
  }

  if (pcHasUsb()) {
    if (laptopHasUsb()) {
      Serial.print(" PC LAP|");
    } else {
      Serial.print("   PC  |");
    }
  } else if (laptopHasUsb()) {
    Serial.print("  LAP  |");
  } else {
    Serial.print("  NONE |");
  }
  Serial.println();

  Serial.println();
  Serial.println("==================");
}
