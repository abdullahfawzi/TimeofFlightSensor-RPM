#include <Wire.h>
#include <VL53L1X.h>
VL53L1X sensor;
// Initialize variables
double distance;
double distance_ambient;
double scaling_constant;
bool calibrated;
int time_to_calibrate;
int count;
bool blade_detected;
int num_of_blades;

void setup()
{
 Serial.begin(115200);
 Wire.begin();
 Wire.setClock(400000); // use 400 kHz I2C
 sensor.setTimeout(500);
 if (!sensor.init())
  {
  Serial.println("Failed to detect and initialize sensor!");
  while (1);
 }
 sensor.setDistanceMode(VL53L1X::Long);
 sensor.setMeasurementTimingBudget(15000);
 sensor.startContinuous(15);
 Serial.println("new program");
 // Initialize values
 distance_ambient = 0;
 scaling_constant = 0.5; // User input
 calibrated = false;
 time_to_calibrate = 100; // User input
 count = 0;
 blade_detected = false;
 num_of_blades = 15; // User input
}

void loop()
{
 //Serial.println(String(millis())+","+String(sensor.read()));
 
//Serial.println(String(millis())+","+String(sensor.read()));

//Get distance from sensor
distance = sensor.read();

//Calibrate ambient distance
 if (calibrated == false){
 if (count < time_to_calibrate){
    distance_ambient = distance_ambient + distance;
    count = count + 1;
    Serial.println("Calibrating (" + String(count) + ")");
} else {
 distance_ambient = distance_ambient / time_to_calibrate;
 calibrated = true;
 Serial.println("Ambient distance has been calibrated to be: " + String(distance_ambient/1000) + " m");
 }
 }
// Calculate RPM
 unsigned long start_time;
 unsigned long end_time;
 double period;
 double rpm;
 if (calibrated == true && distance <= distance_ambient * scaling_constant && blade_detected == false){
 blade_detected = true;
 // Serial.println("Blade detected, distance = " + String(distance) + ", sensor read = " + String(sensor.read()) + ", max distance = " + String(distance_ambient * scaling_constant));
 start_time = millis();
 }
 
 if (blade_detected == true){
  if (distance > distance_ambient * scaling_constant){
  blade_detected = false;
    end_time = millis();
 // Serial.println("start_time = " + String(start_time) + ", end_time = " + String(end_time));
    period = end_time - start_time;
    Serial.println("period = " + String(period / 1000) + " s");
    period = (double) period / 60000; // ms to minute
    rpm = 1 / (num_of_blades * period);
    Serial.println("rpm = " + String(rpm));
 }
 }
 
}
