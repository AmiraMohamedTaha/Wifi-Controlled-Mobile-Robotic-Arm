#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"

Adafruit_PWMServoDriver pwm =
Adafruit_PWMServoDriver();

// =====================================
// ULTRASONIC
// =====================================

#define TRIG_PIN 2

#define ECHO_PIN 3

// =====================================
// SERVO CHANNELS
// =====================================

const int SERVO2 = 4;

const int SERVO3 = 8;

const int SERVO5 = 6;

const int SERVO6 = 10;

// =====================================
// GRIPPER
// =====================================

int gripperOpen = 180;

int gripperClose = 150;

// =====================================
// SERVO5
// =====================================

// initial = 150
// detect  = 90

int servo5Initial = 150;

int servo5Detect = 90;

// =====================================
// LINK LENGTHS
// =====================================

float L1 = 12.0;

float L2 = 12.0;

// =====================================
// CALIBRATION
// =====================================

float servo2Horizontal = 35.0;

float servo3Horizontal = 88.0;

// =====================================
// CURRENT ANGLES
// =====================================

int currentServo2 = 150;

int currentServo3 = 150;

int currentServo5 = 150;

int currentServo6 = 180;

// =====================================
// THROW POSITION
// =====================================

int throwUpServo2 = 100;

int throwUpServo3 = 90;

// =====================================
// SPEED
// =====================================

int moveDelay = 80;

// =====================================
// STATUS
// =====================================

bool busy = false;

// =====================================
// MOVE SERVO
// =====================================

void moveServo(
  int channel,
  int angle
)
{
  angle =
  constrain(
    angle,
    0,
    180
  );

  int pwmValue =
  map(
    angle,
    0,
    180,
    100,
    650
  );

  pwm.setPWM(
    channel,
    0,
    pwmValue
  );
}

// =====================================
// ULTRASONIC
// =====================================

float getDistance()
{
  digitalWrite(
    TRIG_PIN,
    LOW
  );

  delayMicroseconds(2);

  digitalWrite(
    TRIG_PIN,
    HIGH
  );

  delayMicroseconds(10);

  digitalWrite(
    TRIG_PIN,
    LOW
  );

  long duration =
  pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );

  float distance =
  duration * 0.0343 / 2.0;

  return distance;
}

// =====================================
// MOVE GRIPPER
// =====================================

void moveGripper(int target)
{
  while (currentServo6 != target)
  {
    if (currentServo6 < target)
      currentServo6++;

    else if (currentServo6 > target)
      currentServo6--;

    moveServo(
      SERVO6,
      currentServo6
    );

    delay(15);
  }
}

// =====================================
// MOVE SERVO5
// =====================================

void moveServo5(int target)
{
  while (currentServo5 != target)
  {
    if (currentServo5 < target)
      currentServo5++;

    else if (currentServo5 > target)
      currentServo5--;

    moveServo(
      SERVO5,
      currentServo5
    );

    delay(15);
  }
}

// =====================================
// MOVE SERVO2 ONLY
// =====================================

void moveServo2Only(int target)
{
  while (currentServo2 != target)
  {
    if (currentServo2 < target)
      currentServo2++;

    else if (currentServo2 > target)
      currentServo2--;

    moveServo(
      SERVO2,
      currentServo2
    );

    delay(moveDelay);
  }
}

// =====================================
// MOVE SERVO3 ONLY
// =====================================

void moveServo3Only(int target)
{
  while (currentServo3 != target)
  {
    if (currentServo3 < target)
      currentServo3++;

    else if (currentServo3 > target)
      currentServo3--;

    moveServo(
      SERVO3,
      currentServo3
    );

    delay(moveDelay);
  }
}

// =====================================
// MOVE BOTH
// =====================================

void smoothMoveBoth(
  int target2,
  int target3
)
{
  while (
    currentServo2 != target2
    ||
    currentServo3 != target3
  )
  {
    // SERVO2

    if (currentServo2 < target2)
      currentServo2++;

    else if (currentServo2 > target2)
      currentServo2--;

    // SERVO3

    if (currentServo3 < target3)
      currentServo3++;

    else if (currentServo3 > target3)
      currentServo3--;

    // MOVE

    moveServo(
      SERVO2,
      currentServo2
    );

    moveServo(
      SERVO3,
      currentServo3
    );

    delay(moveDelay);
  }
}

// =====================================
// GET Y FROM X
// =====================================

float getY(float x)
{
  if (x <= 16)
    return 1;

  else if (x <= 17)
    return 2;

  else if (x <= 18)
    return 3;

  else if (x <= 20)
    return 4;

  else if (x <= 22)
    return 5;

  else if (x <= 24)
    return 5;

  else
    return 0;
}

// =====================================

void setup()
{
  Serial.begin(9600);

  pwm.begin();

  pwm.setPWMFreq(60);

  // =====================================
  // ULTRASONIC
  // =====================================

  pinMode(
    TRIG_PIN,
    OUTPUT
  );

  pinMode(
    ECHO_PIN,
    INPUT
  );

  delay(1000);

  // =====================================
  // INITIAL POSITION
  // =====================================

  moveServo(
    SERVO2,
    currentServo2
  );

  moveServo(
    SERVO3,
    currentServo3
  );

  moveServo(
    SERVO5,
    currentServo5
  );

  moveServo(
    SERVO6,
    currentServo6
  );

  Serial.println("ULTRASONIC AUTO PICK READY");
}

// =====================================

void loop()
{
  // =====================================
  // READ ULTRASONIC
  // =====================================

  float sensorDistance =
  getDistance();

  // =====================================
  // PRINT DISTANCE
  // =====================================

  Serial.print("Sensor = ");

  Serial.println(sensorDistance);

  // =====================================
  // SENSOR RANGE
  // SENSOR : 10 -> 18
  // ROBOT  : 16 -> 24
  // =====================================

  if (
    sensorDistance >= 13
    &&
    sensorDistance <= 18
    &&
    busy == false
  )
  {
    busy = true;

    // =====================================
    // SERVO5 DOWN
    // =====================================

    moveServo5(
      servo5Detect
    );

    delay(500);

    // =====================================
    // SHIFT
    // =====================================

    float x =
    sensorDistance + 8;

    // =====================================
    // AUTO Y
    // =====================================

    float y =
    getY(x);

    // =====================================
    // IK
    // =====================================

    float D =
    (
      x * x +
      y * y -
      L1 * L1 -
      L2 * L2
    )
    /
    (
      2 * L1 * L2
    );

    D =
    constrain(
      D,
      -1,
      1
    );

    // =====================================
    // ELBOW-UP
    // =====================================

    float theta2 =
    -acos(D);

    float theta1 =
    atan2(y, x)
    -
    atan2(
      L2 * sin(theta2),
      L1 + L2 * cos(theta2)
    );

    // =====================================
    // DEGREE
    // =====================================

    theta1 =
    theta1 * 180.0 / PI;

    theta2 =
    theta2 * 180.0 / PI;

    theta2 =
    abs(theta2);

    // =====================================
    // SERVO ANGLES
    // =====================================

    int servo2Angle =
    servo2Horizontal
    +
    theta1;

    int servo3Angle =
    servo3Horizontal
    +
    theta2;

    // =====================================
    // LIMITS
    // =====================================

    servo2Angle =
    constrain(
      servo2Angle,
      0,
      180
    );

    servo3Angle =
    constrain(
      servo3Angle,
      88,
      163
    );

    // =====================================
    // PRINT
    // =====================================

    Serial.println("OBJECT DETECTED");

    Serial.print("Sensor Distance = ");

    Serial.println(sensorDistance);

    Serial.print("Robot X = ");

    Serial.println(x);

    Serial.print("Y = ");

    Serial.println(y);

    Serial.print("Servo2 = ");

    Serial.println(servo2Angle);

    Serial.print("Servo3 = ");

    Serial.println(servo3Angle);

    // =====================================
    // OPEN GRIPPER
    // =====================================

    moveGripper(
      gripperOpen
    );

    delay(500);

    // =====================================
    // GO TO OBJECT
    // =====================================

    smoothMoveBoth(
      servo2Angle,
      servo3Angle
    );

    delay(1000);

    // =====================================
    // CLOSE GRIPPER
    // =====================================

    moveGripper(
      gripperClose
    );

    delay(500);

    // =====================================
    // LIFT SERVO2
    // =====================================

    moveServo2Only(
      throwUpServo2
    );

    delay(500);

    // =====================================
    // LIFT SERVO3
    // =====================================

    moveServo3Only(
      throwUpServo3
    );

    delay(500);

    // =====================================
    // THROW
    // =====================================
    delay(15000);

    moveGripper(
      gripperOpen
    );

    delay(1000);

    // =====================================
    // SERVO5 BACK
    // =====================================

    moveServo5(
      servo5Initial
    );

    delay(2000);
    
// =====================================
// RETURN TO INITIAL POSITION
// =====================================

smoothMoveBoth(
  150,
  150
);

delay(1000);

// =====================================
// OPEN GRIPPER
// =====================================

moveGripper(
  gripperOpen
);

delay(500);

    // =====================================
    // READY AGAIN
    // =====================================

    busy = false;
  }

  delay(200);
}
