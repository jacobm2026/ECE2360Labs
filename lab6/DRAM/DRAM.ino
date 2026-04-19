const int writePin = 26;
const int readPin = 35;
unsigned long startTime;
unsigned long endTime;
unsigned long elapsedTime1;
unsigned long elapsedTime2;

void setup() {
  Serial.begin(115200);
  pinMode(writePin, OUTPUT);
  }
void loop() {
  startTime=micros();           // Get current time
  digitalWrite(writePin, LOW);   // activate the PNP transistor to write a 1 to the capacitor
  int bitValue = analogRead(readPin); // observe the saved data
  while (bitValue < 3722)  //Wait for capacitor to charge
  { // Serial.printf("Current bit value is %d\n",bitValue);  //Comment this out for questions
    bitValue = analogRead(readPin); //Keep checking voltage
  }
  endTime=micros(); //Get current time
  elapsedTime1 = endTime - startTime; //How long it took to charge the capacitor
  digitalWrite(writePin, HIGH);  // Turn off the transistor
  startTime=millis(); // Get current time
  while (bitValue > 0.75*4095) //Wait for capcitor voltage to fall below 75% of max 
  { bitValue = analogRead(readPin); //keep checking voltage 
    // Serial.printf("Current bit value is %d\n",bitValue); //Comment this out for questions
  }
  endTime=millis(); //Get current time
  elapsedTime2 = endTime - startTime; //Time it took to fall below 75%
  Serial.printf("Writing the bit high took: %lu microseconds.\nHigh value held for: %lu milliseconds.\n", elapsedTime1, elapsedTime2); //Display charge/discharge times
  Serial.flush();
  delay(10000); //Delay to read screen before repeating process
}
