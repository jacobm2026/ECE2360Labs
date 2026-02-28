#include <algorithm>
#include <array>

constexpr std::array<int, 4> output_led = {32, 33, 25, 26};

using arduino_ms = int;
constexpr arduino_ms PRESS_DURATION = 30;
constexpr arduino_ms RELEASE_DURATION = 80;


// Simulates the press for the given digit (0 to 3)
void simulatePress(int digit) {
    // Set pin to LOW for 25ms
    digitalWrite(output_led[digit], LOW);
    delay(PRESS_DURATION);
    digitalWrite(output_led[digit], HIGH);
    delay(RELEASE_DURATION);
}

void setup() {
    std::for_each(output_led.begin(), output_led.end(), [] (int i) {
        // Set pin to output mode
        pinMode(i, OUTPUT);
        // High means no press
        digitalWrite(i, HIGH);
    });
}

void loop() {
    // Try all combinations in order
    for (int first_number = 0; first_number < 4; ++first_number) {
        for (int second_number = 0; second_number < 4; ++second_number) {
            for (int third_number = 0; third_number < 4; ++third_number) {
                for (int fourth_number = 0; fourth_number < 4; ++fourth_number) {
                    simulatePress(first_number);
                    simulatePress(second_number);
                    simulatePress(third_number);
                    simulatePress(fourth_number);
                }
            }
        }
    }
}