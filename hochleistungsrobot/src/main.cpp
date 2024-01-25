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

int side = 0;          // last side
int aux_direction = 0; // auxiliar variable for direction
int aux_climb = 0;     // auxiliar variable for climb

int servoCal[] = {-3, -4, 2, 9, -3, 0, 0, 0};  // Servo calibration data
int servoPos[] = {0, 0, 0, 0, 0, 0, 0, 0};     // Servo current position
int servoPrevPrg[] = {0, 0, 0, 0, 0, 0, 0, 0}; // Servo previous prg
int servoPrgPeriod = 20;                       // 50 ms

#define N_SERVOS 8
Servo servo[N_SERVOS]; // Servo object
Movements movements;   // Movements object

/////////////////////////////  Functions  /////////////////////////////////

// runServoPrg
void runServoPrg(const int servoPrg[][Movements::N_ACTIONS], const int step) {
  for (int i = 0; i < step; i++) { // Loop for step

    int totalTime =
        servoPrg[i][Movements::N_ACTIONS - 1]; // Total time of this step

    // Get servo start position
    for (int s = 0; s < N_SERVOS; s++) {
      servoPos[s] = servo[s].read() - servoCal[s];
    }

    // outer loop: time step
    // inner loop: servo
    for (int j = 0; j < totalTime / servoPrgPeriod; j++) {
      for (int k = 0; k < N_SERVOS; k++) {
        servo[k].write((map(j, 0, totalTime / servoPrgPeriod, servoPos[k],
                            servoPrg[i][k])) +
                       servoCal[k]);
      }
      delay(servoPrgPeriod);
    }
  }
}

// runServoPrg vector mode
void runServoProgV(const int servoPrg[][Movements::N_ACTIONS], const int step) {
  for (int i = 0; i < step; i++) { // Loop for step

    int totalTime =
        servoPrg[i][Movements::N_ACTIONS - 1]; // Total time of this step

    // Get servo start position
    for (int s = 0; s < N_SERVOS; s++) {
      servoPos[s] = servo[s].read() - servoCal[s];
    }

    for (int p = 0; p < N_SERVOS; p++) {
      if (i == 0) {
        servoPrevPrg[p] = servoPrg[i][p];
      } else {
        servoPrevPrg[p] = servoPrevPrg[p] + servoPrg[i][p];
      }
    }

    // outer loop: time step
    // inner loop: servo
    for (int j = 0; j < totalTime / servoPrgPeriod; j++) {
      for (int k = 0; k < N_SERVOS; k++) {
        servo[k].write((map(j, 0, totalTime / servoPrgPeriod, servoPos[k],
                            servoPrevPrg[k]) +
                        servoCal[k]));
      }
      delay(servoPrgPeriod);
    }
  }
}

/////////////////////////////  PID Controlers ///////////////////////////////

// PID direction controller
double setpoint, mpu_directionAngle, Output;
const double Kp = 0.8, Ki = 5, Kd = 0;
PID pid_walking(&mpu_directionAngle, &Output, &setpoint, Kp, Ki, Kd, DIRECT);

// PID climb controller
double Setpoint2, mpu_climbAngle, Output2;
const double Kp2 = 1.4, Ki2 = 0.5, Kd2 = 0;
PID pid_climbing(&mpu_climbAngle, &Output2, &Setpoint2, Kp2, Ki2, Kd2, DIRECT);

// PID Direction Setup
void pid_setup() {
  // turn the PID on
  pid_walking.SetMode(AUTOMATIC);
  pid_walking.SetOutputLimits(-30, 30); // set the output limits
  pid_walking.SetSampleTime(10);        // refresh rate
  pid_walking.SetTunings(Kp, Ki, Kd);   // set PID gains
  setpoint = 0;                         // setpoint

  pid_climbing.SetMode(AUTOMATIC);
  pid_climbing.SetOutputLimits(-20, 20);  // set the output limits
  pid_climbing.SetSampleTime(10);         // refresh rate
  pid_climbing.SetTunings(Kp2, Ki2, Kd2); // set PID gains
  Setpoint2 = 0;
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
  Serial.print(" mm");
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
void mpu_getValues() {
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
  mpu_getValues(); // get values from mpu
  Serial.print("Direction angle : ");
  Serial.println(mpu_directionAngle);
  Serial.print("Climb angle : ");
  Serial.println(mpu_climbAngle);
}

/////////////////////////////  Servo  ////////////////////////////////////

// servoSetup
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

  servo[0].write(90 + servoCal[0]);
  servo[1].write(90 + servoCal[1]);
  servo[2].write(90 + servoCal[2]);
  servo[3].write(90 + servoCal[3]);
  servo[4].write(90 + servoCal[4]);
  servo[5].write(90 + servoCal[5]);
  servo[6].write(90 + servoCal[6]);
  servo[7].write(90 + servoCal[7]);
}

/////////////////////////////  Main  /////////////////////////////////

// Direction Update
void compensateDirection() {
  aux_direction = Output;
  movements.forward[4][1] = -45 + aux_direction;
  movements.forward[6][1] = 45 - aux_direction;
  movements.forward[4][2] = 45 - aux_direction;
  movements.forward[7][2] = -45 + aux_direction;
  movements.forward[7][5] = 45 + aux_direction;
  movements.forward[10][5] = -45 - aux_direction;
  movements.forward[1][6] = -45 - aux_direction;
  movements.forward[4][6] = 45 + aux_direction;
}

// Climb Update
void compensateElevation() {
  aux_climb = Output2;
  movements.forward[0][0] = 30 + aux_climb;
  movements.forward[0][3] = 150 + aux_climb;
  movements.forward[0][4] = 150 - aux_climb;
  movements.forward[0][7] = 30 - aux_climb;
}

void pid_compensate() {

  mpu_getValues(); // get values from mpu

  pid_walking.Compute();  // compute PID
  pid_climbing.Compute(); // compute PID2

  compensateElevation(); // update climb
  compensateDirection(); // update move
}

/////////////////////////////  Setup  /////////////////////////////////

void setup() {
  Wire.begin();          // join i2c bus
  Wire.setClock(400000); // 400kHz I2C clock.
  Serial.begin(115200);  // initialize serial communication

  servoSetup(); // servo setup
  Serial.println("Servo Zero");
  runServoPrg(movements.zero, Movements::ZERO_N_STEPS); // zero position

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
    setpoint = 90;
    direction = RIGHT;
    return;

  case RIGHT:
    setpoint = 180;
    direction = BACK;
    return;

  case BACK:
    setpoint = -90;
    direction = LEFT;
    return;

  case LEFT:
    setpoint = 0;
    direction = FRONT;
    return;
  }
}

bool stair = true;
void run() {
  int numberOfSteps = Movements::FORWARD_N_STEPS;
  auto program = movements.forward;

  bool obstacle = vl53_checkForObstacle();
  if (obstacle && stair) {
    runServoProgV(movements.checkUp, Movements::CHECK_UP_N_STEPS);
    if (!vl53_checkForObstacle()) {
      runServoProgV(movements.climb, Movements::CLIMB_N_STEPS);
      return;
    } else {
      stair = false;
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
    stair = true;
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
    pid_compensate();
  }

  runServoProgV(program, numberOfSteps); // move forward
}

void loop() {
  // main
  run();
}

/////////////////////////////  End  /////////////////////////////////
