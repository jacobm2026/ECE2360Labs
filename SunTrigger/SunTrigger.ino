/// @file SunTrigger.ino
/// @author Jacob Myers
/// SunTrigger is an alarm clock based off of the sun.
/// The alarm will sound when the light reaches below a certain level, and will stop being sound when a button is pressed.
/// Once the button is pressed, a small delay is set to prevent going back and fourth between
/// states, and the same happens when the light level reaches the threshold again going up.
///
/// The delay and other settings can be modifed under Other Constants

// ------------ Pins ------------

inline constexpr int kPhotoresistorPin = 34;
inline constexpr int kButtonPin = 14;
inline constexpr int kLedPin = 18;
inline constexpr int kSpeakerPin = 19;


// ------------ Other Constants ------------

inline constexpr int kLightThreshold = 2800;
inline constexpr int kDelayBetweenIntervals = 5000;
inline constexpr int kDelayBetweenChecks = 500;
inline constexpr int kAlarmFrequency = 2048;


// ------------ Globals ------------

enum class State {
  kAlarmSounding,
  kWaitingForLight,
  kWaitingForDark
} state;

unsigned long last_significant;

void setup(void) {
  // Pin modes
  pinMode(kPhotoresistorPin, INPUT);
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
  pinMode(kSpeakerPin, OUTPUT);

  // Initial pin values
  noTone(kSpeakerPin);
  digitalWrite(kLedPin, LOW);

  // Initialize the global variables
  state = State::kWaitingForDark;
  last_significant = millis();

  
  Serial.begin(115200);
}


class DelayOnDestroy {
public:
  ~DelayOnDestroy() {
    delay(kDelayBetweenChecks);
  }
};

void loop(void) {

  // So I can just use return to go to the next loop iteration while still getting delay
  DelayOnDestroy d;

  if (state == State::kAlarmSounding) {

    if (digitalRead(kButtonPin) == LOW) {
      // Turn off the alarm
      Serial.println("Transistioning to waiting for light");
      state = State::kWaitingForLight;
      digitalWrite(kLedPin, LOW); 
      noTone(kSpeakerPin);
      
    }

  } else if (state == State::kWaitingForLight) {

    if (millis() - last_significant < kDelayBetweenIntervals) {
      Serial.println("Waiting for light, but were in the timeout interval");
      return;
    }
    int light_value = analogRead(kPhotoresistorPin);
    Serial.printf("Photoresistor value: %d.\n", light_value);
    if (light_value > kLightThreshold) {
      state = State::kWaitingForDark;
      Serial.println("Transistioning to waiting for dark");
      last_significant = millis();
    }

  } else { // State == State::kWaitingForDark

    if (millis() - last_significant < kDelayBetweenIntervals) {
      Serial.println("Waiting for dark, but were in the timeout interval");
      return;
    }

    int light_value = analogRead(kPhotoresistorPin);
    Serial.printf("Photoresistor value: %d.\n", light_value);
    if (light_value < kLightThreshold) {
      Serial.println("Transistioning to alarm sounding");
      state = State::kAlarmSounding;
      tone(kSpeakerPin, kAlarmFrequency);
      digitalWrite(kLedPin, HIGH);
      last_significant = millis();
    }

  }
}