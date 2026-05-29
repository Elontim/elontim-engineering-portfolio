# Self Balancing Robot (Modular Robotics System)

## Problem
Maintaining stability in dynamic robotic systems is a fundamental challenge in control engineering. Real-world applications such as delivery robots and autonomous mobility platforms require real-time balancing and feedback control under disturbances.

---

## System Overview
This project is a self-balancing robotic platform designed using embedded control principles and real-time sensor feedback.

The system successfully achieves upright stabilization using closed-loop control. Additional modules for obstacle avoidance and line following were designed as future expansion layers but not fully integrated due to hardware iteration constraints.

---

## System Architecture

### Sensing Layer
- MPU6050 accelerometer and gyroscope for tilt detection

### Processing Layer
- ESP32 microcontroller for real-time computation

### Control Layer
- PID control algorithm for motor correction signals

### Actuation Layer
- DC motors with motor driver (L298N or equivalent)

---

## Components Used
- ESP32 microcontroller
- MPU6050 IMU sensor
- DC motors
- Motor driver module
- Li-ion battery pack
- Chassis frame

---

## My Contribution
- Designed and implemented self-balancing control system
- Tuned PID controller for real-time stability
- Integrated IMU sensor data for tilt correction
- Built motor control logic for bidirectional stabilization
- Developed system architecture for future expansion (line following + obstacle avoidance)

---

## Results
- Achieved stable upright balancing for short duration cycles
- Demonstrated effective real-time PID response to external disturbances
- Validated sensor-to-actuator feedback loop performance
- Established base architecture for autonomous robotic expansion

---

## Limitations / Future Work
- Obstacle avoidance module not fully integrated
- Line following system designed but not implemented
- Future upgrade includes sensor fusion and autonomous navigation layer

---

## Media / Evidence
