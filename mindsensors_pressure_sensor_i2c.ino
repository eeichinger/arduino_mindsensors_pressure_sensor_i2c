#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// monitors pressure using http://www.mindsensors.com/ev3-and-nxt/127-digital-pneumatic-pressure-sensor-for-nxt-or-ev3
// depending on pressure sets motor speed

// Pressure Sensor (I2C)
const int PressureSensorAddress = 0x0C; // Device address in which is also included the 8th bit for selecting the mode, read in this case.
#define Command_Register 0x41 // Power Control Register
#define AbsolutePressure_Register 0x43 // Power Control Register

// LiquidCrystal LCD (I2C)
// set the LCD number of columns and rows
const int lcdAddress = 0x27;
const int lcdColumns = 16;
const int lcdRows = 2;
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows);

// Motor Control
const int motorPin = 9;
// Setpoint Pot
const int setpointPin = A3;

int readAbsolutePressureValue() {
  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(PressureSensorAddress); // transmit to device #112
  Wire.write(byte(AbsolutePressure_Register));      // sets register pointer to echo #1 register (0x02)
  Wire.endTransmission();      // stop transmitting

  // step 4: request reading from sensor
  Wire.requestFrom(PressureSensorAddress, 2);    // request 2 bytes from sensor

  int pressure = -1;
  // step 5: receive reading from sensor
  if (2 <= Wire.available()) { // if two bytes were received
    int byte1 = Wire.read();  // receive high byte (overwrites previous reading)
//    Serial.println(byte1);   // print the reading
    int byte2 = Wire.read(); // receive low byte as lower 8 bits
//    Serial.println(byte2);   // print the reading
    pressure = byte2 << 8;    // shift high byte to be high 8 bits
    pressure |= byte1;
//    Serial.print("pressure:");
//    Serial.println(pressure);   // print the reading
  }
  return pressure;
}

void usePSI() {
  // init set to PSI
  Wire.beginTransmission(PressureSensorAddress);
  Wire.write(Command_Register);
  Wire.write('P'); // set to PSI
  Wire.endTransmission();
}

void setReferencePressure() {
  Wire.beginTransmission(PressureSensorAddress);
  Wire.write(Command_Register);
  Wire.write('D'); // set reference pressure
  Wire.endTransmission();
}

int referencePressure = -1;
int lastPressure = -1;
//int setpointHigh = 20;
//int setpoint = 15;
//int setpointLow = 10;
boolean wasHigh = false;

void setup() {
  pinMode(motorPin, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(setpointPin, INPUT);     // Initialize the LED_BUILTIN pin as an output
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  Wire.begin(); // Initiate the Wire library
  Serial.begin(115200);
  delay(100);
  usePSI();
  delay(1000);
  referencePressure = readAbsolutePressureValue();
  lastPressure = 0;
}

void loop() {
  int setpoint = map(analogRead(setpointPin), 0, 1023, 10, 40);
  int setpointHigh = setpoint+5;
  int setpointLow = setpoint-5;
  Serial.print("SP:");Serial.println(setpoint);
  
  int currentPressure = readAbsolutePressureValue() - referencePressure;
  Serial.print("currentPressure:"); Serial.println(currentPressure);
  if (!wasHigh) {
    if (currentPressure < setpointLow) {
      analogWrite(motorPin, 255); // run at full 12V
      Serial.println("!wasHigh, < setpointLow => motor:255");
    } else if (currentPressure < setpointHigh) {
      analogWrite(motorPin, 120); // run slower at 9V
      Serial.println("!wasHigh, < setpointHigh => motor:190");
    } else {
      Serial.println("!wasHigh, >= setpointHigh => motor:off");
      analogWrite(motorPin, 0); // turn off
      wasHigh = true;
    }
  } else {
    Serial.println("wasHigh, > setpointLow => no action");
    if (currentPressure < setpointLow) {
      Serial.println("wasHigh, < setpointLow => wasHigh:=false");
      wasHigh = false; // dropped below lower threshold
    }
  }
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("SP:");
  lcd.print(setpoint);
  lcd.print(", ");  
  // set cursor to first column, second row
//  lcd.setCursor(0, 1);
  lcd.print("PSI:");
  if (currentPressure<10) {
    lcd.print("0");
  }
  lcd.print(currentPressure);
  delay(300);
  //  lcd.clear();
}
