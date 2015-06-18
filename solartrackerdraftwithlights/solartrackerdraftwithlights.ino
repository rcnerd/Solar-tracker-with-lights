#include <Servo.h>

//manual switch for manual on and off
const int manswitch = 4;
const int autoon = 7;

// porch light switch pin
const int switchpin = 5;

// light value of the sky
int lightval = 0;

// light value that lights should turn on at
const int onlightval = 75;

// persision and anti twitching lower numbers should cut down on twitching but be less persise and vis versa
const int precision = 60;

//Tracking speed
const int trakspeed = precision / 60;

// photorisistors
const int sensorPin = 0;
const int sensorPin1 = 1;

// vcc or power
const int powervolt = 5;
int voltage = 0;

// servo and position in degrees
Servo panel;
int pos = 78;
const int servopin = 9;

//ints for both photoR
int lightLevel, high = 600, low = 600;
int lightLevel1;

void setup()
{
  Serial.begin(1200);
  // servo pin 9
  panel.attach(servopin);

  // set up pin 53 as output for switch
  pinMode(switchpin, OUTPUT);

  // set up pins for manual switch
  pinMode(manswitch, INPUT);
  pinMode(autoon, INPUT);

  //on board led for debug and startup
  pinMode(13, OUTPUT);

  //startup delay/countdown allows for more voltage from solar panels
  for (int time = 175; time > 0; time = time / 1.1)
  {
    digitalWrite(13, HIGH);
    delay(time / 3.6 + 10);
    digitalWrite(13, LOW);
    delay(time + 10);
  }
  //Serial.println(precision);
  delay(125);
  panel.write(pos);
  Serial.println(pos);
}



void loop()
{
  delay(125);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);

  // only continue if voltage is above 6ish volts (180 after reduction)
  voltage = analogRead(powervolt);
  Serial.println("-");
  Serial.println(voltage);
  if (voltage >= 170)
  {
    //Serial.println(">");
    Serial.println(analogRead(sensorPin1));
    //Serial.println("<");
    Serial.println(analogRead(sensorPin));

    track();
    //turn to tracked pos
    panel.write(pos);

    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);

    //if tracked wait
    if (lightLevel == lightLevel1)
    {
      tracked();
    }

    // if night turn on lights
    //take light value
    lightval = (analogRead(0) + analogRead(1)) / 2;
    if (lightval < onlightval)
    {
      high = 600;
      low = 600;

      panel.write(5);

      Serial.println("Night: lights on.");

      // if switch is set to manual off dont turn on light
      if (digitalRead(autoon) == 1)
      {
        digitalWrite(switchpin, HIGH);
      }
      for (int time = 30000; time > 0; time = time / 1.1)
      {
        digitalWrite(13, HIGH);
        delay(time / 3.6 + 10);
        digitalWrite(13, LOW);
        delay(time + 10);
        digitalWrite(13, HIGH);
        delay(time / 3.6 + 10);
        digitalWrite(13, LOW);
        delay(time + 10);

        Serial.println(time);

        if (Serial.available() > 0) {

          // say what you got:
          Serial.print("I received a message:   ");
          Serial.println(Serial.readString());

          Serial.println("Retracking");
          time = 0;

        }
      }
    }
    else
    {
      //other wise turn off and continue
      if (digitalRead(autoon) == 1)
      {
        digitalWrite(switchpin, LOW);
      }

      // if manual switch set to on turn on
      if (digitalRead(manswitch) == 1)
      {
        digitalWrite(switchpin, HIGH);
        {

          // only turn off if manual switch is turned to off or if manual is on and it is day (see above) turn off
          if (digitalRead(manswitch) == 0 && digitalRead(autoon) == 0)
          {
            digitalWrite(switchpin, LOW);
          }

        }
        // if voltage less than good wait 1 min
      }
      else
      {
        Serial.println("low voltage");
        delay(15000);
      }


    }

    void autoTune()
    {

      // update latest low and high values
      if (lightLevel < low)
      {
        low = lightLevel;
      }

      if (lightLevel1 < low)
      {
        low = lightLevel1;
      }



      if (lightLevel > high)
      {
        high = lightLevel;
      }

      if (lightLevel1 > high)
      {
        high = lightLevel1;
      }

      // dilate down then up to avoid twitching then trim
      lightLevel = map(lightLevel, low + 1, high - 1, 0, precision);
      lightLevel = map(lightLevel, 0, precision, 0, 60);
      lightLevel = constrain(lightLevel, 0, 60);

      lightLevel1 = map(lightLevel1, low + 1, high - 1, 0, precision);
      lightLevel1 = map(lightLevel1, 0, precision, 0, 60);
      lightLevel1 = constrain(lightLevel1, 0, 60);
    }

    void tracked()
    {
      Serial.println("paused");
      for (int secs = 600; secs > 0; secs -= 1)
      {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
        delay(100);
        int secs22;
        if (secs22 == 1)
        {

          secs22++;
        }
        else if ( secs22 == 2 || secs22 == 3 || secs22 == 4 || secs22 == 5 || secs22 == 6 || secs22 == 7 || secs22 == 8 || secs22 == 9)
        {
          secs22++;
        }
        else
        {
          //Serial.println((analogRead(0)+analogRead(1))/2);
          Serial.println(secs);
          secs22 = 1;
        }

        if (Serial.available() > 0) {

          // say what you got:
          Serial.print("I received a message:   ");
          Serial.println(Serial.readString());

          Serial.println("Retracking");
          secs = 0;

        }
      }
    }

    void track()
    {
      // read photoRs
      lightLevel = analogRead(sensorPin1);
      lightLevel1 = analogRead(sensorPin);

      //Serial.println((analogRead(0)+analogRead(1))/2);

      //map and constrain values to reacent high and low
      autoTune();

      // calculate new position
      pos = pos + trakspeed * (lightLevel - lightLevel1);

      pos = pos - 2;
      //Serial.println("-");
      Serial.println(pos);

      pos = constrain(pos, 0, 180);
      delay(125);
    }

