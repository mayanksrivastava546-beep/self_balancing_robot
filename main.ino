#include <Wire.h>

// ==========================================
// 1. HARDWARE PINS & VARIABLES
// ==========================================
const int IN1 = 5;  // Motor A direction
const int IN2 = 6;  // Motor A direction
const int ENA = 9;  // Motor A PWM (Speed)
const int IN3 = 7;  // Motor B direction
const int IN4 = 8;  // Motor B direction
const int ENB = 10; // Motor B PWM (Speed)

// PID Variables
float Kp = 25.0; // Proportional: Reacts to current angle
float Ki = 0.5;  // Integral: Reacts to accumulated past errors
float Kd = 1.2;  // Derivative: Reacts to the speed of falling (future prediction)

float currentAngle = 0;
float targetAngle = 0; // 0 degrees = perfectly upright
float error = 0, previousError = 0, integral = 0, derivative = 0;
float motorPower = 0;

unsigned long previousTime;

// ==========================================
// 2. SENSOR MODULE (Mock MPU6050 Reading)
// ==========================================
float readIMUAngle() {
    // In a real project, you use the MPU6050 library to read the Y-axis accelerometer 
    // and gyroscope, and combine them using a Complementary or Kalman filter.
    // For this structural example, we return a simulated angle.
    return 2.5; // Simulating the robot leaning forward by 2.5 degrees
}

// ==========================================
// 3. MOTOR CONTROL MODULE
// ==========================================
void driveMotors(float power) {
    // Constrain power to PWM limits (0 to 255)
    int speed = min(abs((int)power), 255);
    
    if (power > 0) {
        // Fall forward -> Drive forward to catch itself
        digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    } else if (power < 0) {
        // Fall backward -> Drive backward to catch itself
        digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    } else {
        // Stop
        digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    }
    
    // Apply calculated speed to both motors
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);
}

// ==========================================
// 4. MAIN PROGRAM EXECTUTION
// ==========================================
void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Initialize Motor Pins
    pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
    pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
    
    previousTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - previousTime) / 1000.0; // Convert to seconds
    previousTime = currentTime;

    // STEP 1: Read actual angle from IMU
    currentAngle = readIMUAngle();
    
    // STEP 2: Calculate PID Math
    error = targetAngle - currentAngle;
    integral += (error * deltaTime);
    derivative = (error - previousError) / deltaTime;
    
    // Calculate required motor power
    motorPower = (Kp * error) + (Ki * integral) + (Kd * derivative);
    
    // STEP 3: Drive Motors
    // If the robot falls beyond 45 degrees, it can't recover. Kill motors for safety.
    if (abs(currentAngle) > 45) {
        driveMotors(0);
        integral = 0; // Reset integral windup
    } else {
        driveMotors(motorPower);
    }

    previousError = error;
    
    // Run at a fixed frequency (e.g., ~100Hz)
    delay(10); 
}
