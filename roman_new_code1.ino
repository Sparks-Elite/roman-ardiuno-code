#include <Servo.h>

// -------- PIN DEFINITIONS --------
#define trig_Pin D5
#define echo_Pin D6
#define PIR_PIN D7

#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4

#define HEAD_SERVO_PIN D8
#define LEFT_ARM_SERVO D0
#define RIGHT_ARM_SERVO 3

// -------- VARIABLES --------
long duration;
float distance;

int interactionDelay = 4000;

// Servo Objects
Servo headServo;
Servo leftArmServo;
Servo rightArmServo;

// Head scanning
int headAngle = 0;
int direction = 1;   // 1 = forward, -1 = backward
int scanSpeed = 2;   // step size (speed control)

#define ANGLE_FRONT 90

// -------- SETUP --------
void setup()
{
  Serial.begin(9600);

  pinMode(trig_Pin, OUTPUT);
  pinMode(echo_Pin, INPUT);
  pinMode(PIR_PIN, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  headServo.attach(HEAD_SERVO_PIN);
  leftArmServo.attach(LEFT_ARM_SERVO);
  rightArmServo.attach(RIGHT_ARM_SERVO);

  headServo.write(90);
  leftArmServo.write(90);
  rightArmServo.write(90);

  Serial.println("Robot Ready");
}

// -------- HEAD SCANNING --------
void headScan()
{
  headServo.write(headAngle);
  headAngle += direction * scanSpeed;

  if(headAngle >= 180 || headAngle <= 0)
  {
    direction = -direction; // reverse direction
  }

  delay(30); // smooth movement
}

// -------- ARM GREETING (FULL 0–180) --------
void armGesture()
{
  // Raise arms
  for(int i = 0; i <= 180; i += 3)
  {
    leftArmServo.write(i);
    rightArmServo.write(180 - i);
    delay(10);
  }

  delay(300);

  // Lower arms
  for(int i = 180; i >= 0; i -= 3)
  {
    leftArmServo.write(i);
    rightArmServo.write(180 - i);
    delay(10);
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

// -------- DISTANCE --------
float getDistance()
{
  digitalWrite(trig_Pin, LOW);
  delayMicroseconds(2);

  digitalWrite(trig_Pin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig_Pin, LOW);

  duration = pulseIn(echo_Pin, HIGH);

  return duration * 0.034 / 2;
}

// -------- LOOP --------
void loop()
{
  distance = getDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  // Always scan head
  headScan();

  // Object detected
  if(distance < 30)
  {
    Serial.println("Object Detected");

    unsigned long startTime = millis();
    bool humanDetected = false;

    // PIR detection
    while(millis() - startTime < 3000)
    {
      if(digitalRead(PIR_PIN) == HIGH)
      {
        humanDetected = true;
        Serial.println("Human Detected");
        break;
      }
      headScan(); // continue scanning while checking
    }

    if(humanDetected)
    {
      stopMotors();

      // Face human
      headServo.write(ANGLE_FRONT);

      // Greeting
      armGesture();

      delay(interactionDelay);
    }
  }
  else
  {
    moveForward();
  }

  delay(50);
}
