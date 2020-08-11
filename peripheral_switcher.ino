const int pcUsbInputPin = 2;
const int laptopUsbInputPin = 3;
const int pcHdmiInputPin = 4;
const int laptopHdmiInputPin = 5;
const int buttonPin = 6;
const int hdmiSwitch = 7;
const int usbSwitch = 8;
const int pcControlLedPin = 9;
const int laptopControlLedPin = 10;
const int errorStateLedPin = 11;

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

void setup() {
  pinMode(pcUsbInputPin, INPUT);
  pinMode(laptopUsbInputPin, INPUT);
  pinMode(pcHdmiInputPin, INPUT);
  pinMode(laptopHdmiInputPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(hdmiSwitch, OUTPUT);
  pinMode(usbSwitch, OUTPUT);
  pinMode(pcControlLedPin, OUTPUT);
  pinMode(laptopControlLedPin, OUTPUT);
  pinMode(errorStateLedPin, OUTPUT);

  digitalWrite(hdmiSwitch, hdmiSwitchState);
  digitalWrite(usbSwitch, usbSwitchState);
  digitalWrite(pcControlLedPin, LOW);
  digitalWrite(laptopControlLedPin, LOW);
  digitalWrite(errorStateLedPin, LOW);
}

void loop() {
  alignSwitches(true);

  if (buttonHasBeenPressed()) {
    switchHdmi();
    switchUsb();
  } else {
    turnOnIndicatorLed();
  }

  resetTimersIfNeeded();

  digitalWrite(hdmiSwitch, hdmiSwitchState);
  digitalWrite(usbSwitch, usbSwitchState);
}

bool pcHasUsb() {
  return digitalRead(pcUsbInputPin) == HIGH;
}

bool laptopHasUsb() {
  return digitalRead(laptopUsbInputPin) == HIGH;
}

bool pcHasHdmi() {
  return digitalRead(pcHdmiInputPin) == HIGH;
}

bool laptopHasHdmi() {
  return digitalRead(laptopHdmiInputPin) == HIGH;
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
    digitalWrite(pcControlLedPin, HIGH);
    digitalWrite(laptopControlLedPin, LOW);
    digitalWrite(errorStateLedPin, LOW);
  } else if (laptopHasHdmi() && laptopHasUsb()) {
    digitalWrite(laptopControlLedPin, HIGH);
    digitalWrite(pcControlLedPin, LOW);
    digitalWrite(errorStateLedPin, LOW);
  } else {
    digitalWrite(errorStateLedPin, HIGH);
    digitalWrite(pcControlLedPin, LOW);
    digitalWrite(laptopControlLedPin, LOW);
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
