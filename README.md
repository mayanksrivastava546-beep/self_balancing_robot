# Self-Balancing Robot (Inverted Pendulum)

## Overview
This project controls a two-wheeled self-balancing robot using an Arduino, an MPU6050 (Accelerometer + Gyroscope), and a standard L298N Motor Driver. It relies on a PID control loop to calculate the exact motor speed required to counteract gravity.

## Logical Flow

> **Excalidraw Integration:** Copy the Mermaid code block below, go to [excalidraw.com](https://excalidraw.com), click on **Insert** -> **Mermaid**, and paste the code to generate the system architecture diagram.

```mermaid
graph TD
    %% Hardware Inputs
    MPU[MPU6050 IMU Sensor] -->|Raw Accel + Gyro Data| Filter[Complementary Filter]
    Filter -->|Current Angle <br> e.g., 5 Degrees| ErrorCalc{Calculate Error}
    
    %% Target State
    Target[Target Angle = 0°] --> ErrorCalc
    
    %% PID Controller
    ErrorCalc -->|Error Value| P[Proportional<br>Kp * Error]
    ErrorCalc -->|Error Value| I[Integral<br>Ki * Sum of Errors]
    ErrorCalc -->|Error Value| D[Derivative<br>Kd * Rate of Change]
    
    P --> Sum[Sum Output]
    I --> Sum
    D --> Sum
    
    %% Output
    Sum -->|Motor PWM Power| MotorDriver[Motor Driver L298N]
    MotorDriver --> WheelForward[Wheels drive forward<br>to catch falling robot]
    
    %% Feedback Loop
    WheelForward -.->|Changes Robot Physics| MPU
```

## How to Tune the Robot
Tuning a self-balancing robot is notoriously tricky. Follow these steps:
1. **Kp (Proportional) only:** Set Ki and Kd to 0. Increase Kp until the robot starts oscillating back and forth rapidly (it shouldn't fall flat, but it will shake).
2. **Kd (Derivative) next:** Slowly increase Kd. The derivative acts like a "shock absorber." It will dampen the violent shaking from the Kp value and make the robot stand relatively still.
3. **Ki (Integral) last:** If the robot is balancing but slowly drifting in one direction across the floor, add a tiny amount of Ki (e.g., 0.1 to 0.5) to correct the long-term drift.
