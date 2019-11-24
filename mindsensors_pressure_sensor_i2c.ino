#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int PressureSensorAddress = 0x0C; // Device address in which is also included the 8th bit for selecting the mode, read in this case.

#define Command_Register 0x41 // Power Control Register
#define AbsolutePressure_Register 0x43 // Power Control Register

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  Wire.begin(); // Initiate the Wire library
  Serial.begin(9600);
  delay(100);
  // init set to PSI
  Wire.beginTransmission(PressureSensorAddress);
  Wire.write(Command_Register);
  Wire.write('P'); // set to PSI
  Wire.endTransmission();
//  Wire.beginTransmission(PressureSensorAddress);
//  Wire.write(Command_Register);
//  Wire.write('D'); // set reference pressure
//  Wire.endTransmission();
}

int pressure = -1;

void loop(){
  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(PressureSensorAddress); // transmit to device #112
  Wire.write(byte(AbsolutePressure_Register));      // sets register pointer to echo #1 register (0x02)
  Wire.endTransmission();      // stop transmitting

  // step 4: request reading from sensor
  Wire.requestFrom(PressureSensorAddress, 2);    // request 2 bytes from sensor

  // step 5: receive reading from sensor
  if (2 <= Wire.available()) { // if two bytes were received
    int byte1 = Wire.read();  // receive high byte (overwrites previous reading)
    Serial.println(byte1);   // print the reading
    int byte2 = Wire.read(); // receive low byte as lower 8 bits
    Serial.println(byte2);   // print the reading
    pressure = byte2 << 8;    // shift high byte to be high 8 bits
    pressure |= byte1;
    Serial.print("pressure:");
    Serial.println(pressure);   // print the reading
  }
  
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Hello, World!");
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print("Pressure:");
  lcd.print(pressure);
  delay(300);
//  lcd.clear(); 
}
