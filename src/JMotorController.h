#pragma once
#ifdef ENABLE_MOTOR
#include <TMCStepper.h>

#define EN_PIN 14  // Enable
#define DIR_PIN 13 // Direction
#define STEP_PIN 4 // Step

#define CS_PIN 42           // Chip select
#define SW_MOSI 66          // Software Master Out Slave In (MOSI)
#define SW_MISO 44          // Software Master In Slave Out (MISO)
#define SW_SCK 64           // Software Slave Clock (SCK)
#define SW_RX 63            // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX 40            // TMC2208/TMC2224 SoftwareSerial transmit pin
#define SERIAL_PORT Serial2 // TMC2208/TMC2224 HardwareSerial port

#define R_SENSE                                                                \
  0.075f // Match to your driver
         // SilentStepStick series use 0.11
         // UltiMachine Einsy and Archim2 boards use 0.2
         // Panucatt BSD2660 uses 0.1
         // Watterott TMC5160 uses 0.075

// Select your stepper driver type
// TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE); // Hardware SPI
// TMC2130Stepper driver = TMC2130Stepper(CS_PIN, R_SENSE, SW_MOSI, SW_MISO,
// SW_SCK); // Software SPI
// TMC2209Stepper driver = TMC2209Stepper(
// &SERIAL_PORT, R_SENSE,
// 0b00); // Hardware Serial0 TMC2209Stepper driver = TMC2209Stepper(SW_RX,

// TMC2208Stepper driver =
// TMC2208Stepper(&SERIAL_PORT, R_SENSE); // Hardware Serial0
TMC2209Stepper driver = TMC2209Stepper(&SERIAL_PORT, R_SENSE, 0b00);
// SW_TX, R_SENSE); // Software serial TMC2660Stepper driver =
// SW_TX, R_SENSE); // Software serial TMC2660Stepper driver =
// TMC2660Stepper(CS_PIN, R_SENSE); // Hardware SPI TMC2660Stepper driver =
// TMC2660Stepper(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK); TMC5160Stepper
// driver = TMC5160Stepper(CS_PIN, R_SENSE); TMC5160Stepper driver =
// TMC5160Stepper(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK);

constexpr uint32_t steps_per_mm = 80;

#include <AccelStepper.h>
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

bool bForward = 1;
void Task1code(void *pvParameters) {
  Serial.print("Motor update running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    // if (stepper.distanceToGo() == 0)
    // stepper.disableOutputs();
    stepper.run();
  }
}

class JMotorController {
public:
  JMotorController() {}

  TaskHandle_t Task1;
  void initMotor() {
    // SPI.begin();
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
    driver.begin(); // Initiate pins and registeries
    driver.toff();
    driver.rms_current(
        200); // Set stepper current to 600mA. The command is the
              // same as command TMC2130.setCurrent(600, 0.11, 0.5);
    // driver.en_pwm_mode(1); // Enable extremely quiet stepping
    driver.pwm_autoscale(1);
    driver.en_spreadCycle(true); // Toggle spreadCycle on TMC2208/2209/2224
    driver.microsteps(64);

    stepper.setMaxSpeed(50 * steps_per_mm);      // 100mm/s @ 80 steps/mm
    stepper.setAcceleration(140 * steps_per_mm); // 2000mm/s^2
    stepper.setEnablePin(EN_PIN);
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();
    xTaskCreatePinnedToCore(
        Task1code, /* Task function. */
        "Task1",   /* name of task. */
        10000,     /* Stack size of task */
        NULL,      /* parameter of the task */
        1,         /* priority of the task */
        &Task1,    /* Task handle to keep track of created task */
        1);
  }

  static void receiveMotorCommands(const uint8_t *mac_addr, const uint8_t *data,
                                   int data_len) {
    float val;
    memcpy(&val, data + 6 + 1 + 1, sizeof(float));

    Serial.println("Motor cmd");
    switch (data[6 + 1]) {
    case 0x01: // move (rel)
      stepper.move(val * steps_per_mm);
      Serial.println("Move");
      stepper.enableOutputs();
      break;
    case 0x02: // moveTo (abs)
      stepper.moveTo(val * steps_per_mm);
      stepper.enableOutputs();
      break;
    case 0x03: // setAccell
      stepper.setAcceleration(val * steps_per_mm);
      break;
    case 0x04: // setSpeed
      stepper.setMaxSpeed(val * steps_per_mm);
      break;
    }
  }

  // void updateMotor() {
  // Serial.println("Update motor");
  // Always running
  // int dir = bForward ? 1 : -1;
  // stepper.move(36 * steps_per_mm * dir); // Move 100mm
  // stepper.enableOutputs();
  // bForward = !bForward;
};
#endif
