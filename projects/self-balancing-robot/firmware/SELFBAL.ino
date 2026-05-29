#include <Wire.h>

const int TRIG = 9;
const int ECHO = 8;
const int IR_PIN = 7;

const int L_PWM = 5;
const int L_DIR = 4;
const int R_PWM = 6;
const int R_DIR = 3;

const int MPU = 0x68;

float Kp = 30.0;   // Start low (~15–40), increase until oscillation
float Ki = 0.02;   // Usually very small (0.01–0.2)
float Kd = 1.0;    // 0.5–3.0

float angle = 0.0;           // fused angle
float previousAngle = 0.0;
float gyroAngle = 0.0;
float accAngle = 0.0;

float integral = 0.0;
float lastError = 0.0;
float setpoint = 0.0;        // upright = ~0° (adjust if sensor offset)

unsigned long prevTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Wake up MPU6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Basic gyro calibration (better to do proper one)
  Serial.println("Keep sensor still for calibration...");
  delay(2000);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(IR_PIN, INPUT);

  pinMode(L_PWM, OUTPUT);
  pinMode(L_DIR, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(R_DIR, OUTPUT);

  prevTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  float dt = (currentTime - prevTime) / 1000.0; // in seconds
  prevTime = currentTime;

  if (dt <= 0) dt = 0.01; // safety

  // ───────────── Read raw accel + gyro ─────────────
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // start with ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // 6 accel + 2 temp + 6 gyro

  int16_t ax = Wire.read() << 8 | Wire.read();
  int16_t ay = Wire.read() << 8 | Wire.read();
  int16_t az = Wire.read() << 8 | Wire.read();

  // skip temp
  Wire.read(); Wire.read();

  int16_t gx = Wire.read() << 8 | Wire.read();
  int16_t gy = Wire.read() << 8 | Wire.read();
  int16_t gz = Wire.read() << 8 | Wire.read();

  // ───────────── Accel angle (pitch) ─────────────
  // Use AY for forward/back tilt (most common orientation)
  accAngle = atan2(-ay, sqrt(ax * ax + az * az)) * 180.0 / PI;

  // ───────────── Gyro rate (use GY for pitch) ─────────────
  float gyroRate = gy / 131.0;  // ±250°/s default → 131 LSB/(°/s)

  // Simple complementary filter
  gyroAngle = gyroAngle + gyroRate * dt;
  angle = 0.98 * (angle + gyroRate * dt) + 0.02 * accAngle;
  // OR better: angle = 0.98 * gyroAngle + 0.02 * accAngle;

  // ───────────── Obstacle avoidance override ─────────────
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 25000);
  float distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 15) {
    setpoint = 3.0;   // lean backward a bit when obstacle close
  } else {
    setpoint = 0.0;
  }

  // ───────────── PID ─────────────
  float error = setpoint - angle;
  integral += error * dt;
  float derivative = (error - lastError) / dt;
  lastError = error;

  float output = Kp * error + Ki * integral + Kd * derivative;

  // Limit integral windup
  integral = constrain(integral, -50, 50);

  // Motor output
  int motorSpeed = constrain(abs(output), 0, 255);

  // Both motors same direction for forward/back correction
  bool forward = (output > 0);

  digitalWrite(L_DIR, forward ? HIGH : LOW);
  digitalWrite(R_DIR, forward ? HIGH : LOW);

  analogWrite(L_PWM, motorSpeed);
  analogWrite(R_PWM, motorSpeed);

  // Debug – very useful!
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) {
    Serial.print("Angle: "); Serial.print(angle, 2);
    Serial.print(" | Err: "); Serial.print(error, 2);
    Serial.print(" | Out: "); Serial.print(output, 0);
    Serial.print(" | Dist: "); Serial.println(distance);
    lastPrint = millis();
  }

  delay(10);
}