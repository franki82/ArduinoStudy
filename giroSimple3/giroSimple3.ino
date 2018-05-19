#include <SPI.h>

#include <Wire.h>

#define MPU 0x68  // I2C address of the MPU-6050


double AcX,AcY,AcZ;

void setup(){
  Serial.begin(9600);
  init_MPU(); // Inizializzazione MPU6050
}

 

void loop()

{

  FunctionsMPU(); // Acquisisco assi AcX, AcY, AcZ.

  Serial.print(AcX);
  Serial.print("\t");
  Serial.print(AcY);
  Serial.print("\t");
  Serial.print(AcZ);
  Serial.print("\t");
  Serial.println("");
}


void init_MPU(){

  Wire.begin();

  Wire.beginTransmission(MPU);

  Wire.write(0x6B);  // PWR_MGMT_1 register

  Wire.write(0);     // set to zero (wakes up the MPU-6050)

  Wire.endTransmission(true);

  delay(1000);

}



//Funzione per l'acquisizione degli assi X,Y,Z del MPU6050

void FunctionsMPU(){

  Wire.beginTransmission(MPU);

  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)

  Wire.endTransmission(false);

  Wire.requestFrom(MPU,6,true);  // request a total of 14 registers

  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     

  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)

  AcZ=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)

}
