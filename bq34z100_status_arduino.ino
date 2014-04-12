// Read Values from BQ34Z100 Gas Gauge Chip
// by Samuel J. Vanderwaal

// This code creates functions to read different status values from the BQ34Z100 gas gauge chip, it interprets that data and stores it into variables.


#include <Wire.h>                                                                          // Wire library for communicating over I2C
#define BQ34Z100 0x55                                                                      // This is the I2C address of the BQ34Z100


unsigned int soc, voltage, remain_cap;
int avg_current, batt_temp, cntrl_stat1, cntrl_stat2;
float power_draw;
byte instCurrentLSB[] = {0x00, 0x18};
byte instCurrentMSB[] = {0x01, 0x00};
unsigned int inst_current_lsb, inst_current_msb;
int inst_current;

// ~~BQ34Z100 Status Functions~~

// State of Charge, a percent value of the battery's total charge
void readSOC()
{
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x02);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int low = Wire.read();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x03);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int high = Wire.read();
  
  unsigned int high1 = high<<8;
  
  soc = high1 + low;
}

// The battery's remaining capacity in mAh
void readRemainingCapacity()
{
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x04);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int low = Wire.read();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x05);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int high = Wire.read();
  
  unsigned int high1 = high<<8;
  
  remain_cap = high1 + low;
}

void readVoltage()
{
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x08);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int low = Wire.read();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x09);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int high = Wire.read();
  
  unsigned int high1 = high<<8;
  
  voltage = high1 + low;

}

void readAverageCurrent()
{
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x0a);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int low = Wire.read();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x0b);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int high = Wire.read();
  
  unsigned int high1 = high<<8;
  
  avg_current = high1 + low;
}

void readInstCurrent()
{
  // Write Control command bytes to gas gauge chip: 0x00/0x01
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x18);
  Wire.endTransmission();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x01);
  Wire.endTransmission();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  inst_current_lsb = Wire.read();
 
  Wire.requestFrom(BQ34Z100,1);
  inst_current_msb = Wire.read();
  
  unsigned int temp = inst_current_msb << 8;
  
  inst_current = temp + inst_current_lsb;
}


void readBattTemp()
{
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x0c);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int low = Wire.read();
  
  Wire.beginTransmission(BQ34Z100);
  Wire.write(0x0d);
  Wire.endTransmission();
  
  Wire.requestFrom(BQ34Z100,1);
  
  unsigned int high = Wire.read();
  
  unsigned int high1 = high<<8;
  
  batt_temp = high1 + low;
  
  batt_temp = 0.1*batt_temp;          // Each bit is 0.1K, so we have a value in Kelvins
  batt_temp = batt_temp - 273.15;      // Convert to degrees Celsius
}

float PowerDraw(float volt, float current)
{
  volt = volt/1000.0;
  current = current/1000.0;
  float power = volt*current;
  return power;
}

void setup()
{
  Serial.begin(9600);
  Serial3.begin(57600);
  Wire.begin();
}

void loop()
{
 readSOC();
 Serial.print("Battery Charge: ");
 Serial.print(soc);
 Serial.println("%");
 
 readRemainingCapacity();
 Serial.print("Remaining Capacity: ");
 Serial.print(remain_cap);
 Serial.println(" mAh");
 
 readVoltage();
 Serial.print("Battery Pack Voltage: ");
 Serial.print(voltage);
 Serial.println(" mV");

 readAverageCurrent();
 Serial.print("Average Current Draw: ");
 Serial.print(avg_current);
 Serial.println(" mA");
 
 readInstCurrent();
 Serial.print("Instantaneous Current Draw: ");
 Serial.println(inst_current);
 
 Serial.print("Instantaneous Current Draw LSB: ");
 Serial.println(inst_current_lsb);
 
 Serial.print("Instantantous Current MSB: ");
 Serial.println(inst_current_msb);
 
 readBattTemp();
 Serial.print("Battery Temperature: ");
 Serial.print(batt_temp);
 Serial.println(" C");

 power_draw = PowerDraw(voltage,avg_current);
 
 Serial.print("Power Draw: ");
 Serial.print(power_draw);
 Serial.println(" W");
 
 Serial.print("\r\n");
  
 delay(5000);
}
