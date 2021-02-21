#include <ResponsiveAnalogRead.h>
#include <VarSpeedServo.h>
#include "ListLib.h"

VarSpeedServo servoBase;
VarSpeedServo servoTop;
List<int> baseServoList;
List<int> topServoList;
List<bool> directionServoList; // 0 for base , 1 for top

const int servoPinBase = 9; //Servo base on pin 9
const int servoPinTop = 10;
const int potBase = A0;
const int potTop = A1;

ResponsiveAnalogRead analogBase(potBase, true);
ResponsiveAnalogRead analogTop(potTop, true);

int potBaseValue;
int potTopValue;
int prevPotTopValue;
int prevPotBaseValue;
bool mode = false;      //false = read data , true = move the arm
int timeInterval = 100; //peripou 5 deuterolepta
int timeCount = 0;

void setup()
{
  pinMode(potBase, INPUT);
  pinMode(potTop, INPUT);
  servoBase.attach(servoPinBase);
  servoTop.attach(servoPinTop);
  analogBase.update();
  analogTop.update();
  prevPotBaseValue = map(analogBase.getValue(), 0, 1023, 0, 180);
  prevPotTopValue = map(analogTop.getValue(), 0, 1023, 0, 180);
  Serial.begin(9600);
}

void loop()
{

  if (mode == false)
  {
    //read data
    readDataMode();
  }
  else
  {
    //move the arm
    int baseCounter = 0;
    int topCounter = 0;
    for (int i = 0; i < directionServoList.Count(); i++)
    {
      if (directionServoList[i] == 0)
      {
        servoBase.write(baseServoList[baseCounter], 30, true);
        baseCounter++;
      }
      else
      {
        servoTop.write(topServoList[topCounter], 30, true);
        topCounter++;
      }
    }
    delay(10000);
  }

  delay(100);
}

void readDataMode()
{
  readDataFromPot();

  if (abs(potBaseValue - prevPotBaseValue) > 3 || abs(potTopValue - prevPotTopValue) > 3)
  {
    moveServosWithDataRead();
    timeCount = 0;
  }
  else
  {
    timeCount++;
    if (timeCount > timeInterval)
    {
      mode = true;
    }
  }

  prevPotBaseValue = potBaseValue;
  prevPotTopValue = potTopValue;
}

void readDataFromPot(void)
{

  int potBaseSum = 0;
  int potTopSum = 0;

  for (int i = 0; i < 10; i++)
  {
    analogBase.update();
    analogTop.update();
    potBaseSum = potBaseSum + analogBase.getValue();
    potTopSum = potTopSum + analogTop.getValue();
  }
  potBaseValue = potBaseSum / 10;
  potTopValue = potTopSum / 10;

  potBaseValue = map(potBaseValue, 0, 1023, 0, 180);
  potTopValue = map(potTopValue, 0, 1023, 0, 180);
}

void moveServosWithDataRead()
{

  servoBase.write(potBaseValue);
  if (baseServoList[baseServoList.Count() - 1] != servoBase.read())
  {
    directionServoList.Add(0);
    baseServoList.Add(servoBase.read());
  }

  servoTop.write(potTopValue);
  if (topServoList[topServoList.Count() - 1] != servoTop.read())
  {
    directionServoList.Add(1);
    topServoList.Add(servoTop.read());
  }
}
