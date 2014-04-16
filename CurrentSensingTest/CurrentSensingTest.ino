/*
  Analog Input
 Demonstrates analog input by reading an analog sensor on analog pin 0 and
 turning on and off a light emitting diode(LED)  connected to digital pin 13. 
 The amount of time the LED will be on and off depends on
 the value obtained by analogRead(). 
 
 The circuit:
 * Potentiometer attached to analog input 0
 * center pin of the potentiometer to the analog pin
 * one side pin (either one) to ground
 * the other side pin to +5V
 * LED anode (long leg) attached to digital output 13
 * LED cathode (short leg) attached to ground
 
 * Note: because most Arduinos have a built-in LED attached 
 to pin 13 on the board, the LED is optional.
 
 
 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe
 
 This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/AnalogInput
 
 */

int sensorPinLow = A0;
int sensorPinHigh = A1;// select the input pin for the potentiometer
int sensorPinDp = A2;
int sensorPinDm = A3;
int sensorValueLow = 0;  // variable to store the value coming from the sensor
int sensorValueHigh = 0;
int sensorValueDp = 0;
int sensorValueDm = 0;
float voltageLow = 0;
float voltageHigh = 0;
float voltageDp = 0;
float voltageDm = 0;
float current = 0;
void setup() {
  // declare the ledPin as an OUTPUT:
 Serial.begin(9600); 
}

void loop() {
  // read the value from the sensor:
  sensorValueLow = analogRead(sensorPinLow);
  voltageLow = sensorValueLow*5.0/1024;
  sensorValueHigh = analogRead(sensorPinHigh);
  voltageHigh = sensorValueHigh*5.0/1024;
  sensorValueDp = analogRead(sensorPinDp);
  voltageDp = sensorValueDp*5.0/1024;
  sensorValueDm = analogRead(sensorPinDm);
  voltageDm = sensorValueDm*5.0/1024;
  current = ((voltageHigh-voltageLow)/151.0)/0.01;
  Serial.print("High: ");
  Serial.print(voltageHigh);
  Serial.print(" Low: ");
  Serial.print(voltageLow);
  Serial.print(" Current: ");
  Serial.print(current);
  Serial.print(" Dp: ");
  Serial.print(voltageDp);
  Serial.print(" Dm: ");
  Serial.println(voltageDm);
  
  
  delay(500);           
}
