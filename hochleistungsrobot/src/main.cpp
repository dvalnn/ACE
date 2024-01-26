#include <Arduino.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <PID_v1.h>
#include <Servo.h>
#include <VL53L0X.h>
#include <Wire.h>
#include <api/Common.h>
#include <cstdint>
#include <elapsedMillis.h>

#include "movements.h"

///////////////////////////// Movement /////////////////////////////////

#define N_SERVOS 8
int servoCalib[N_SERVOS] = {-3, -4, 2, 9, -3, 0, 0, 0}; // calibration data
Servo servo[N_SERVOS];
Movements movements;

// runServoPrg vector mode
void runServoProgV(const int servoProg[][Movements::N_ACTIONS],
                   const int nSteps) {

  int servoPos[N_SERVOS] = {0, 0, 0, 0, 0, 0, 0, 0};
  int servoPrevProg[N_SERVOS] = {0, 0, 0, 0, 0, 0, 0, 0};
  int servoPrgPeriodMS = 20;

  for (int i = 0; i < nSteps; i++) { // Loop for step
    // Get servo start position
    for (int s = 0; s < N_SERVOS; s++) {
      servoPos[s] = servo[s].read() - servoCalib[s];
    }

    for (int p = 0; p < N_SERVOS; p++) {
      if (i == 0) {
        servoPrevProg[p] = servoProg[i][p];
      } else {
        servoPrevProg[p] = servoPrevProg[p] + servoProg[i][p];
      }
    }

    int totalTime = servoProg[i][Movements::N_ACTIONS - 1];
    // outer loop: time step
    for (int j = 0; j < totalTime / servoPrgPeriodMS; j++) {
      // inner loop: servo
      for (int k = 0; k < N_SERVOS; k++) {
        servo[k].write((map(j, 0, totalTime / servoPrgPeriodMS, servoPos[k],
                            servoPrevProg[k]) +
                        servoCalib[k]));
      }

      delay(servoPrgPeriodMS);
    }
  }
}

/////////////////////////////  PID Controlers ///////////////////////////////

// PID direction controller
double directionSetpoint, mpu_directionAngle, directionAdjust;
const double Kp = 0.8, Ki = 5, Kd = 0;
PID directionPID(&mpu_directionAngle, &directionAdjust, &directionSetpoint, Kp,
                 Ki, Kd, DIRECT);

// PID climb controller
double elevationSetpoint, mpu_climbAngle, elevationAdjust;
const double Kp2 = 1.4, Ki2 = 0.5, Kd2 = 0;
PID elevationPID(&mpu_climbAngle, &elevationAdjust, &elevationSetpoint, Kp2,
                 Ki2, Kd2, DIRECT);

// PID Direction Setup
void pid_setup() {
  // turn the PID on
  directionPID.SetMode(AUTOMATIC);
  directionPID.SetOutputLimits(-30, 30); // set the output limits
  directionPID.SetSampleTime(10);        // refresh rate
  directionPID.SetTunings(Kp, Ki, Kd);   // set PID gains
  directionSetpoint = 0;                 // setpoint

  elevationPID.SetMode(AUTOMATIC);
  elevationPID.SetOutputLimits(-20, 20);  // set the output limits
  elevationPID.SetSampleTime(10);         // refresh rate
  elevationPID.SetTunings(Kp2, Ki2, Kd2); // set PID gains
  elevationSetpoint = 0;
}

// Direction Update
void compensateDirection() {
  movements.forward[4][1] = -45 + directionAdjust;
  movements.forward[6][1] = 45 - directionAdjust;
  movements.forward[4][2] = 45 - directionAdjust;
  movements.forward[7][2] = -45 + directionAdjust;
  movements.forward[7][5] = 45 + directionAdjust;
  movements.forward[10][5] = -45 - directionAdjust;
  movements.forward[1][6] = -45 - directionAdjust;
  movements.forward[4][6] = 45 + directionAdjust;
}

// Climb Update
void compensateElevation() {
  movements.forward[0][0] = 15 + elevationAdjust;
  movements.forward[0][3] = 165 + elevationAdjust;
  movements.forward[0][4] = 165 - elevationAdjust;
  movements.forward[0][7] = 15 - elevationAdjust;
}

void pid_compensate() {
  directionPID.Compute(); // compute PID
  elevationPID.Compute(); // compute PID2

  compensateElevation(); // update climb
  compensateDirection(); // update move
}

/////////////////////////////  MPU6050  ////////////////////////////////////

// MPU control/status vars
MPU6050 mpu(0x68);     // MPU6050 object
uint16_t packetSize;   // expected DMP packet size (default is 42 bytes)
bool dmpReady = false; // set true if DMP init was successful

// mpuSetup
void mpu_setup() {
  mpu.initialize(); // initialize MPU6050
  if (!mpu.testConnection()) {
    Serial.println("Failed to find MPU6050 chip");
    return;
  }

  Serial.println("MPU6050 Found!");

  delay(300);

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  uint8_t devStatus = mpu.dmpInitialize();

  // make sure it worked (returns 0 if so)
  if (devStatus != 0) {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
    return;
  }

  // Calibration Time: generate offsets and calibrate our MPU6050
  mpu.CalibrateAccel(20);
  mpu.CalibrateGyro(20);
  mpu.PrintActiveOffsets();
  // turn on the DMP, now that it's ready
  Serial.println(F("Enabling DMP..."));
  mpu.setDMPEnabled(true);
  dmpReady = true;

  packetSize = mpu.dmpGetFIFOPacketSize();
}

// read mpu values
void mpu_update() {
  uint8_t fifoBuffer[64]; // FIFO storage buffer
  Quaternion q;           // [w, x, y, z]
  VectorFloat gravity;    // [x, y, z]
  float ypr[3];           // [yaw, pitch, roll]

  mpu.dmpGetCurrentFIFOPacket(fifoBuffer); // read a packet from FIFO
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  mpu_directionAngle = ypr[0] * 180 / M_PI;
  mpu_climbAngle = ypr[2] * 180 / M_PI;
}

void mpu_testDebug() {
  mpu_update(); // get values from mpu
  Serial.print("Direction angle : ");
  Serial.println(mpu_directionAngle);
  Serial.print("Climb angle : ");
  Serial.println(mpu_climbAngle);
  delay(100);
}

/////////////////////////////  VL53L0X Sensor ///////////////////////////////

VL53L0X vl53_sensor; // VL53L0X object
elapsedMillis vl53_lastSampleTime;

const long vl53_SAMPLE_PERIOD_MS = 500;
const int vl53_DET_THRESH = 135;

// sensorSetup
void vl53_setup() {
  // Initialize the sensor
  if (!vl53_sensor.init(0x29)) {
    Serial.println("Failed to detect and initialize sensor!");
    return;
  }
  Serial.println("VL53L0X sensor detected!");
  vl53_sensor.setTimeout(500);
}

// check sensor
bool vl53_checkForObstacle() {
  if (vl53_lastSampleTime < vl53_SAMPLE_PERIOD_MS)
    return false;

  vl53_lastSampleTime = 0;

  uint16_t distance = vl53_sensor.readRangeSingleMillimeters();
  if (distance > 0 && distance < vl53_DET_THRESH) {
    return true;
  }

  return false;
}

void vl53_testDebug() {
  uint16_t distance = vl53_sensor.readRangeSingleMillimeters();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" mm");
  delay(100);
}

/////////////////////////////  Setup  /////////////////////////////////

void servoSetup() {
  // Servo Pin Set
  servo[0].attach(0);
  servo[1].attach(1);
  servo[2].attach(2);
  servo[3].attach(3);
  servo[4].attach(6);
  servo[5].attach(7);
  servo[6].attach(8);
  servo[7].attach(9);
}

void setup() {
  Wire.begin();          // join i2c bus
  Wire.setClock(400000); // 400kHz I2C clock.
  Serial.begin(115200);  // initialize serial communication

  servoSetup();
  runServoProgV(movements.zero, Movements::ZERO_N_STEPS);

  delay(2000);

  vl53_setup(); // vl53 setup
  mpu_setup();  // mpu setup
  pid_setup();  // PID setup
}

/////////////////////////////  Loop  ////////////////////////////////

typedef enum { // State machine states
  FRONT,
  RIGHT,
  LEFT,
  BACK
} Directions;

Directions direction = FRONT; // Initial states
int stepsRight = 0;
int stepsRightAcc = 0;
int stepsBack = 0;

void switchDirection() {
  switch (direction) {

  case FRONT:
    directionSetpoint = 90;
    direction = RIGHT;
    return;

  case RIGHT:
    directionSetpoint = 180;
    direction = BACK;
    return;

  case BACK:
    directionSetpoint = -90;
    direction = LEFT;
    return;

  case LEFT:
    directionSetpoint = 0;
    direction = FRONT;
    return;
  }
}

bool mightBeClimbable = true;
void run() {
  int numberOfSteps = Movements::FORWARD_N_STEPS;
  auto program = movements.forward;

  bool obstacle = vl53_checkForObstacle();
  if (obstacle && mightBeClimbable) {
    runServoProgV(movements.checkUp, Movements::CHECK_UP_N_STEPS);
    if (!vl53_checkForObstacle()) {
      runServoProgV(movements.forward, Movements::FORWARD_N_STEPS);
      runServoProgV(movements.climb, Movements::CLIMB_N_STEPS);
      return;
    } else {
      mightBeClimbable = false;
    }
  }

  if (obstacle && !stepsRight && !stepsBack) {
    stepsRight += 5; // minimum required steps to clear an obstacle
  } else if (obstacle && stepsRightAcc > 14) {
    switchDirection();
    stepsBack = 5;
  } else if (obstacle) {
    stepsRight++;
  } else {
    stepsRightAcc = 0;
    mightBeClimbable = true;
  }

  if (stepsBack) {
    program = movements.back;
    numberOfSteps = Movements::BACK_N_STEPS;
    stepsBack--;
  } else if (stepsRight) {
    program = movements.crabRight;
    numberOfSteps = Movements::CRAB_RIGHT_N_STEPS;
    stepsRight--;
    stepsRightAcc++;
  } else {
    // only when moving forward
    mpu_update();
    pid_compensate();
  }

  runServoProgV(program, numberOfSteps); // move forward
}

void loop() {
  // main
  /* vl53_testDebug(); */
  run();
}

/////////////////////////////  End  /////////////////////////////////
