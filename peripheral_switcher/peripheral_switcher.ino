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

const int servoPos1 = 75;
const int servoPos2 = 0;
int auxServoPosition;

enum InputDevice { device1, device2 };

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
  auxServoPosition = audioSwitchServo.read();
}

void loop() {
  alignSwitches();

  if (buttonHasBeenPressed()) {
    switchHdmi();
  } else {
    turnOnIndicatorLed();
  }

  resetTimersIfNeeded();
  writeOutputs();
}

void writeOutputs() {
  digitalWrite(hdmiSwitch, hdmiSwitchState);
  digitalWrite(usbSwitch, usbSwitchState);
  audioSwitchServo.write(auxServoPosition);
}

InputDevice getHdmiInput() {
  return digitalRead(hdmiInput1Pin) == HIGH ? device1 : device2;
}

InputDevice getUsbInput() {
  return digitalRead(usbInput1Pin) == HIGH ? device1 : device2;
}

InputDevice getAuxInput() {
  return audioSwitchServo.read() == servoPos1 ? device1 : device2;
}

void alignSwitches() {
  InputDevice hdmiInput = getHdmiInput();

  setUsb(hdmiInput);
  setAux(hdmiInput);
}

void setUsb(InputDevice targetDevice) {
  InputDevice currentUsbDevice = getUsbInput();

  if (currentUsbDevice != targetDevice) {
    switchUsb();
  }
}

void setAux(InputDevice targetDevice) {
  InputDevice currentAuxDevice = getAuxInput();

  if (currentAuxDevice != targetDevice) {
    switchAux();
  }
}

void switchHdmi() {
  lastHdmiSwitchTime = millis();

  hdmiSwitchState = !defaultSwitchState;
}

void switchUsb() {
  lastUsbSwitchTime = millis();

  usbSwitchState = !defaultSwitchState;
}

void switchAux() {
  auxServoPosition = auxServoPosition == servoPos1 ? servoPos2 : servoPos1;
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
  InputDevice hdmiDevice = getHdmiInput();
  InputDevice usbDevice = getUsbInput();
  InputDevice auxDevice = getAuxInput();

  if (hdmiDevice == device1 &&
      usbDevice == device1 &&
      auxDevice == device1) {
    digitalWrite(device1IndicatorLedPin, HIGH);
    digitalWrite(device2IndicatorLedPin, LOW);
    digitalWrite(errorIndicatorLedPin, LOW);
  } else if (hdmiDevice == device2 &&
             usbDevice == device2 &&
             auxDevice == device2) {
    digitalWrite(device1IndicatorLedPin, LOW);
    digitalWrite(device2IndicatorLedPin, HIGH);
    digitalWrite(errorIndicatorLedPin, LOW);
  } else {
    digitalWrite(device1IndicatorLedPin, LOW);
    digitalWrite(device2IndicatorLedPin, LOW);
    digitalWrite(errorIndicatorLedPin, HIGH);
  }
}

void logOut() {
  Serial.println("==================");
  Serial.println();
  Serial.println("|  HDMI  |  USB  |  AUX  |");
  Serial.println("|--------|-------|-------|");

  InputDevice hdmiDevice = getHdmiInput();
  InputDevice usbDevice = getUsbInput();
  InputDevice auxDevice = getAuxInput();

  if (hdmiDevice == device1) {
    Serial.print("|   1    |");
  } else {
    Serial.print("|   2    |");    
  }

  if (usbDevice == device1) {
    Serial.print("|   1   |");
  } else {
    Serial.print("|   2   |");    
  }

  if (auxDevice == device1) {
    Serial.print("|   1   |");
  } else {
    Serial.print("|   2   |");    
  }
  Serial.println();

  Serial.println();
  Serial.println("==================");
}
