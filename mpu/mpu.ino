#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize MPU6050
  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  Serial.println("MPU6050 ready!");
}

void loop() {
  // Variables for raw data
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  // Get motion data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convert raw accelerometer values to 'g'
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  // Calculate angles using scaled accelerometer data
  float accelAngleX = atan2(ay_g, az_g) * 180 / PI; // Roll
  float accelAngleY = atan2(-ax_g, sqrt(ay_g * ay_g + az_g * az_g + 1e-6)) * 180 / PI; // Pitch

  // Debug: Print values
  Serial.print("Roll: ");
  Serial.print(accelAngleX);
  Serial.print("°, Pitch: ");
  Serial.print(accelAngleY);
  Serial.println("°");

  delay(500);
}
