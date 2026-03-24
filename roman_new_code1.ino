// -------- SERVO LIBRARY --------
#include <Servo.h>

// -------- PIN DEFINITIONS --------

// Ultrasonic
#define TRIG D5
#define ECHO D6

// PIR Sensor
#define PIR_PIN D7

// Motor Driver (L298N)
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4

// Servo Pins
#define HEAD D8
#define ARM D0

// -------- VARIABLES --------
long duration;
float distance;

int interactionDelay = 20000;

// Servo Objects
Servo M_HEAD;
Servo M_ARM;

// Head swing variables
int angle = 0;
int swingAngle = 60;
int stepDelay = 50;

//Arm angle

int armAngle = 0;
int armswingAngle = 90;
int armstepDelay = 20;



#define ANGLE_FRONT 90

// -------- SETUP --------
void setup()
{
  Serial.begin(9600);

  // Ultrasonic
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // PIR
  pinMode(PIR_PIN, INPUT);

  // Motors
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Attach Servos
  M_HEAD.attach(HEAD);
  M_ARM.attach(ARM);

  // Initial Positions
  M_HEAD.write(0);
  M_ARM.write(0);
}

// -------- HEAD SWING FUNCTION --------
void headSwing()
{
  for(angle = 0; angle <= swingAngle; angle++)
  {
    M_HEAD.write(angle);
    delay(stepDelay);
  }

  delay(2000);

  for(angle = swingAngle; angle >= 0; angle--)
  {
    M_HEAD.write(angle);
    delay(stepDelay);
  }
}

// -------- ARM GESTURE --------
void armGesture()
{
  for(int armAngle = 70; armAngle <= 110; armAngle++)
  {
    M_ARM.write(armAngle);
    delay(40);
  }

  delay(1000);

  for(int armAngle = 110; armAngle >= 70; armAngle--)
  {
    M_ARM.write(armAngle);
    delay(40);
  }
}

// -------- MOTOR CONTROL --------
void stopMotors()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// -------- DISTANCE MEASURE --------
float getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000);

  distance = duration * 0.034 / 2;

  return distance;
}

// -------- LOOP --------
void loop()
{
  distance = getDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  // -------- OBJECT DETECTED --------
  if(distance < 30)
  {
    Serial.println("Object Detected");

    delay(5000);

    unsigned long startTime = millis();
    bool humanDetected = false;

    // PIR CHECK
    while(millis() - startTime < 10000)
    {
      if(digitalRead(PIR_PIN) == HIGH)
      {
        humanDetected = true;
        Serial.println("Human Motion Detected");
        break;
      }
    }

    // -------- HUMAN CONFIRMED --------
    if(humanDetected)
    {
      stopMotors();

      // Face visitor
      M_HEAD.write(ANGLE_FRONT);

      // Greeting
      armGesture();

      // Wait interaction
      delay(interactionDelay);
    }
  }

  // -------- NO OBJECT --------
  else
  {
    moveForward();

    // Idle animation
    headSwing();
  }

  delay(500);
}
