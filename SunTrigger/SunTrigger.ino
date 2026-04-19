#include <Arduino.h>
#include <driver/gpio.h>


// ------------ Pins ------------

inline constexpr int kPhotoresistorPin = 34;
inline constexpr int kButtonPin = 2;
inline constexpr int kLedPin = 18;
inline constexpr int kSpeakerPin = 19;


// ------------ Other Constants ------------

inline constexpr int kLightThreshold = 2000;
inline constexpr int kDelayBetweenIntervals = 5000;
inline constexpr int kDelayBetweenChecks = 500;
inline constexpr int kAlarmFrequency = 2048;



enum class State {
  kAlarmSounding,
  kWaitingForLight,
  kWaitingForDark
};


extern "C" void app_main(void) {
  initArduino();

  pinMode(kPhotoresistorPin, INPUT);
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  pinMode(kSpeakerPin, OUTPUT);

  noTone(kSpeakerPin);
  digitalWrite(kLedPin, LOW);

  State state = State::kWaitingForDark;
  unsigned long last_significant = millis();
  for (;; delay(kDelayBetweenChecks)) {

    if (state == State::kAlarmSounding) {

      if (digitalRead(kButtonPin) == LOW) {
        // Turn off the alarm
        state = State::kWaitingForLight;
        digitalWrite(kLedPin, LOW);
        noTone(kSpeakerPin);
        
      }

    } else if (state == State::kWaitingForLight) {

      if (millis() - last_significant < kDelayBetweenIntervals) {
        continue;
      }

      if (analogRead(kPhotoresistorPin) < kLightThreshold) {
        state = State::kWaitingForDark;
        last_significant = millis();
      }

    } else { // State == State::kWaitingForDark

      if (millis() - last_significant < kDelayBetweenIntervals) {
        continue;
      }

      if (analogRead(kPhotoresistorPin) > kLightThreshold) {
        state = State::kAlarmSounding;
        tone(kSpeakerPin, kAlarmFrequency);
        digitalWrite(kLedPin, HIGH);
        last_significant = millis();
      }

    }
  }
}