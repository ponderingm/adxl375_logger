#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <math.h>
int Raw_Axel_X = 0;
int Raw_Axel_Y = 0;
int Raw_Axel_Z = 0;
float Axel_X = 0;
float Axel_Y = 0;
float Axel_Z = 0;
float Axel_N = 0;
const float OFFSET_X = 0.098;
const float OFFSET_Y = 0.588;
const float OFFSET_Z = 0.784;
File logFile;
char fileName[16];
int fileNum = 0;
int writecount = 0;
unsigned long nowtime = 0;
unsigned long oldtime = 0;
boolean SD_FLAG = true;
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(38400);
  Wire.beginTransmission(0x1d);
  Wire.write(byte(0x00));
  Wire.endTransmission();
  Wire.requestFrom(0x1d,1);
  if(Wire.read() ==0xE5 )
  {
//    Serial.println("ADXL375 is Detected!");
  }
  else
  {
    Serial.println("ERROR");
    delay(5000);
    return;
  }

  if(!SD.begin(4)){
    // SDカードがないとき
    Serial.println("SD is not detected. FULL SPEED MODE !");
    SD_FLAG = false;
    
  }

  // ファイル名決定
  String s;

  while(1){
    s = "LOG";
    if (fileNum < 10) {
      s += "00";
    } else if(fileNum < 100) {
      s += "0";
    }
    s += fileNum;
    s += ".csv";
    s.toCharArray(fileName, 16);
    if(!SD.exists(fileName)) break;
    fileNum++;
  }

  Serial.print("logFile number is ");
  Serial.println(fileNum);
    
  Wire.beginTransmission(0x1d);
  Wire.write(byte(0x2D));
  Wire.write(byte(0x08));
  Wire.write(byte(0x31));
  Wire.write(byte(0x0b));
  Wire.write(byte(0x2c));
  Wire.write(byte(0x0b));
  Wire.endTransmission();

}

void loop() {
  while((oldtime + 4) >= millis())
  {
    
  }
  unsigned long nowtime = millis();
  
  Serial.print(nowtime);
  Serial.print(",");
  Wire.beginTransmission(0x1d);
  Wire.write(byte(0x32));
  Wire.endTransmission();
  Wire.requestFrom(0x1d,6);
  Raw_Axel_X = Wire.read();
  Raw_Axel_X += (Wire.read() << 8);
  Axel_X = Raw_Axel_X * 0.049- OFFSET_X;
//  Serial.print(Axel_X , 3);
  Raw_Axel_Y = Wire.read();
  Raw_Axel_Y += (Wire.read() << 8);
  Axel_Y = Raw_Axel_Y * 0.049- OFFSET_Y;
//  Serial.print(",");
//  Serial.print(Axel_Y, 3);
  Raw_Axel_Z = Wire.read();
  Raw_Axel_Z += (Wire.read() << 8);
  Axel_Z = Raw_Axel_Z * 0.049- OFFSET_Z;
//  Serial.print(",");
//  Serial.print(Axel_Z, 3);
  Axel_N = sqrt(Axel_X * Axel_X + Axel_Y * Axel_Y + Axel_Z * Axel_Z);
//  Serial.print(",");
  Serial.print(Axel_N, 3);
//    Serial.print(",");
//  if(Axel_N >= 25 )
//  {
//    Serial.print("1");
//  }
//  else
//  {
//    Serial.print("0");
//  }
  
  Serial.println("");

  if(SD_FLAG == true)  // ログファイルに書き込む
  {
    logFile = SD.open(fileName, FILE_WRITE);
    
    logFile.print(nowtime);
    logFile.print(",");
    logFile.print(Axel_X, 3);
    logFile.print(",");
    logFile.print(Axel_Y, 3);
    logFile.print(",");
    logFile.print(Axel_Z, 3);
    logFile.print(",");
    logFile.println(Axel_N, 3);
    logFile.close();
  }



  while(Wire.available())
  {
    Wire.read();
  }
  oldtime = nowtime;
//  delay(1);
}
