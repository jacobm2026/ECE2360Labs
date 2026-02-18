#include <stdbool.h>

const int ledPin = 25;       // LED output pin
const int photoresistorPin = 35;
const int potentiometerPin = 36;
const int buttonPin = 39;
bool ledStatus = false;
int buttonPrevState = 0;

// This will always be betweek [0, 4)
int brightnessLevel = 0;

void writeBrightnessToLED(int brightness) {
  switch (brightness) {
    case 0:
      ledcWrite(ledPin, 0);
      break;
    case 1:
      ledcWrite(ledPin, 20);
      break;
    case 2:
      ledcWrite(ledPin, 100);
      break;
    case 3:
      ledcWrite(ledPin, 255);
      break;
    default:
      __builtin_unreachable(); // This is not reachable -- only invoke this function with [0, 3]. Let compiler optimize.
  }
}

void setup() {
  Serial.begin(115200);
  // pinMode(ledPin, OUTPUT);
  ledcAttach(ledPin, 5000, 8);
  pinMode(photoresistorPin, INPUT);
  pinMode(potentiometerPin, INPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {

  // Read in the values from the photoresistor and potentiometer
  int photoresistorValue = analogRead(photoresistorPin); // Integer in 0 to about 1100
  int potentiometerValue = analogRead(potentiometerPin); // Integer in [0, 4095]
  // Serial.printf("Photoresistor analog value: %d.\n", photoresistorValue);
  // Serial.printf("Potentiometer analog value: %d.\n", potentiometerValue);
  // Divide potentiometer by 4 to get about the same range
  int threshold = potentiometerValue / 4;

  int buttonDown = digitalRead(buttonPin);
  // Serial.println(buttonDown);
  // Change the brightness level if the button was pressed and wasn't pressed on the previous loop
  if (buttonDown && buttonDown != buttonPrevState) {
    // Swtich the brightnesslevel
    ++brightnessLevel;
    brightnessLevel = brightnessLevel == 4 ? 0 : brightnessLevel;
    Serial.printf("Button pressed: switched to brightness level %d.\n", brightnessLevel);
  }

  // Determine if the light should turn on/off
  // Multiply by a number close to 1 to prevent light from switching quickly when threshold and photoresistorValue are close
  if ((buttonDown != buttonPrevState && ledStatus && buttonDown) || (!ledStatus && photoresistorValue > threshold * 1.1f)) {
    Serial.println("Environment change -- writing new LED brightness.");
    writeBrightnessToLED(brightnessLevel);
    ledStatus = true;
  } else if (ledStatus && photoresistorValue < threshold * 0.9f) {
    Serial.println("Room went light -- turning off LED.");
    writeBrightnessToLED(0);
    ledStatus = false;
  }

  // Update the previous state
  buttonPrevState = buttonDown;
  delay(10);
}
