#include <Servo.h>

//terminating charactor for incoming message
char const MSG_TERMINATOR = '\n';

//Motors on left and right side of rover
Servo leftTrack;
Servo rightTrack;

void setup()
{
  Serial.begin(9600);

  leftTrack.attach(2);
  rightTrack.attach(3);
}

void loop()
{
  String message;

  //take action on next message in buffer
  if(Serial.available() > 0 &&
    (message = Serial.readStringUntil(MSG_TERMINATOR)) != NULL &&
    isalpha(message.charAt(0)))
  {
    //character indicating motor to set
    char track = message.charAt(0);

    //integer indicating velocity in percent power
    int vel = message.substring(1).toInt();

    if(vel <= 100 && vel >= -100)
    {
      //microseconds of on time for PPM signal that determines velocity
      int pulseTime = map(vel, -100, 100, 1000, 2000);

      switch(track)
      {
        case 'l':
          leftTrack.writeMicroseconds(pulseTime);
          break;
        case 'r':
          rightTrack.writeMicroseconds(pulseTime);
          break;
      }
    }
  }
}
