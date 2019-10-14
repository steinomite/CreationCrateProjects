/* Creation Crate Weather Station 2.0
This new version of the weather station includes a number of improvements in both the components we will be using the and the programming.  This is phase 1.  Instead of printing the results to  the serial monitor, Nokia 5110 LCD screen.  Not only is this super cool, it makes the station  portable!  We will be using some new commands to streamline the operation and creating a few functions.  In phase 2 and phase 3 we will be adding an aneometer and internet connectivity!
  
Components Required
  1. UNO R3 Microcontroller
  2. BMP180 Module
  3. DHT11 Module
  4. Nokia 5110 LCD screen
 
The circuit:
  BMP180: GND - Ground | SCL - A5 | SDA - A4 | VIN - 5V
  DHT11:  GND - Ground | Data - 11 | VCC - 5V
  Nokia 5110:
  RST - Reset | CE - Ground | DC - 10 | Din - 9 | Clk - 8 | VCC - 5V | Bl - 5V | Gnd - Ground
 
Functions
  As we've discussed in the past, functions are usually created for tasks that are needed 
  throughout the program so we don't have to write the same instructions over and over.
  printWeatherData, clearValues, componentCheck
  
  Created 11.19.18
  By David Hehman
  http://creationcrate.com/
*/
// Step 1: Include all necessary libraries
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
 
// Step 2: Define pins and initialize global variables
#define DHTPIN 11
#define DHTTYPE DHT11
 
float tempC = 0;
float tempF = 0;
float humidity = 0;
float absPressure = 0;
float seaLevelPressure = 0;
float heatIndexC = 0;
float heatIndexF = 0;
float altitudeM = 0;
float altitudeFt = 0;
int units = 'a';
bool DHT_OK = true;
bool BMP_OK = true;
 
// Constant variable (the value cannot be changed).
const float m_to_ft = 3.28084;
 
// Declare instances of classes
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_PCD8544 display = Adafruit_PCD8544(8,9,10,11,12);
 
// Step 3: Create setup loop
void setup() 
{
  Serial.begin (9600);
  bmp.begin();
  dht.begin();
  componentCheck();  // call FUNCTION: componentCheck, make sure components are connected & operational
 
//Start the Nokia Screen and display a message  
  display.begin();      
  display.setContrast(40);          // Set contrast to desired value (0 - 100)
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("ENV CONDITIONS");
  display.drawFastHLine(0,10,83,BLACK);
  display.display();   
 
/* -- USER INPUTS -- */
  units = 'i';                      //enter 'm' for Metric and 'i' for Imperial
  seaLevelPressure = 100;       //enter the sea level pressure you found online, converted to kPa
 
} // END OF SETUP LOOP
 
// Step 4: Create main loop
void loop() 
{
  /* -- Retrieve data from BMP180 sensor -- */
  // TEMPERATURE
  if (BMP_OK)                       // If the BMP sensor is working, then...
  {
    tempC = bmp.readTemperature();  // Get temperature from BMP180 (The BMP180 temp reading is more precise than the DHT)
    absPressure = bmp.readPressure();                       // BAROMETRIC PRESSURE
    altitudeM = bmp.readAltitude(seaLevelPressure * 1000);  // ALTITUDE in meters - multiply the mean sea level pressure in kPa by 1000 to get Pa
    tempF = (tempC * 1.8 + 32);         // Look up the formula to convert Celsius to Fahrenheit
    altitudeFt = (altitudeM * m_to_ft); // Convert altitude from meters to feet
  }
 
  /* -- Retrieve data from DHT11 sensor -- */
  else if (DHT_OK)                      // Otherwise, if BMP180 is not working, and if the DHT sensor is working, then...
  {
    tempC = dht.readTemperature();      // get temperature from DHT11, default is Celsius
    tempF = dht.readTemperature(true);  // when 'true' the temperature will return in Fahrenheit
  }
  humidity = dht.readHumidity();
 
  /* -- Calculate heat index by taking humidity into account. -- */
  heatIndexF = dht.computeHeatIndex(tempF, humidity);         // send Fahrenheit temp & humidity to the library to return heat index (default)
  heatIndexC = dht.computeHeatIndex(tempC, humidity, false);  // send Celsius temp & humidity & 'false' flag (NOT Fahrenheit) to get Celsius
 
  printWeatherData();               // Call our FUNCTION: printWeatherData
}   // END OF MAIN LOOP
 
 
/* -- FUNCTIONS -- */
/* -- FUNCTION: printWeatherData - prints data to the Nokia 5110 LCD display. -- */
void printWeatherData()
{
  clearValues();                                  // Call our FUNCTION: clearValues
  display.println ("Temperature: ");              // println moves the cursor to the next line after printing the characters
  display.print (units == 'm' ? tempC : tempF);   // when the units are set to m (metric) print temp in Celsius, otherwise Fahrenheit
  display.print ((char)247);
  display.print (units == 'm' ? " C" : " F");     // when the units are set to m (metric) print 'C' otherwise print 'F'
 
  display.setCursor(0, 31);                       // set the cursor to column 0 and row 31
  display.println ("Humidity: ");
  display.print (humidity);
  display.print ("%");
  display.display();
  delay (3000);
 
  clearValues();
  display.println ("Heat Index: ");
  display.print (units == 'm' ? heatIndexC : heatIndexF);
  display.print ((char)247);
  display.print (units == 'm' ? " C" : " F");
 
  display.setCursor(0, 31);
  display.println ("Barometric Pr:");
  display.print (absPressure/ 1000, 2);
  display.print (" kPa");
  display.display();
  delay (3000);
 
  clearValues();
  display.println ("Altitude: ");
  display.print (units == 'm' ? altitudeM : altitudeFt);
  display.println (units == 'm' ? " m" : " ft");
  display.display();
  delay(3000);
}
 
/* -- FUNCTION: clearValues - will "clear" the lower part of the screen so new values can be displayed -- */
void clearValues()
{
  display.fillRect(0, 15, 84, 33, WHITE);   // draw a rectangle that is filled in "white"
  display.setCursor(0, 15);
}
 
/* -- FUNCTION: componentCheck will check to see if the components are operational -- */
void componentCheck()
{
  // Check if BMP180 is connected properly
  if (!bmp.begin())                       // If bmp did not start properly, then...
  {
    BMP_OK = false;                       // change the variable to 'false'
    display.clearDisplay();
    display.println("!!NO  BMP180!!");    // and display this message
    display.print ("check wiring  Continue w/o  BMP180...");
    display.display();
    delay (3000);
    display.clearDisplay();
  }                 
 
  // Initiate DHT sensor and check its connection.
  if (isnan(dht.readHumidity()) || isnan(dht.readTemperature()))    // if the value returned is 'nan' (not a number), then...
  {
    DHT_OK = false;                                                 // change the fariable to 'false'
    display.clearDisplay();
    display.println("!!NO  DHT 11!!");                              // and display this message
    display.print ("check wiring  Continue w/o  DHT 11...");
    display.display();
    delay (3000);
    display.clearDisplay();
  }
 
  // If both sensors are bad, loop forever and display message to user.
  if (!BMP_OK && !DHT_OK)                                           // if neither sensors are working, then...
  {
    display.clearDisplay();
    display.println("!!NO  BMP180!!!!NO  DHT 11!!");                // display this message
    display.print ("check wiring  reset to try     again.    ");
    display.display();
    while (1) {}          // infinite loop - code loops here until reset
  }
}
