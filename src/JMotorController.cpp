#include "JMotorController.h"

bool bForward = 1;
int steps = 0;

// TaskHandle_t moveTask;

AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

TMC2209Stepper driver = TMC2209Stepper(&SERIAL_PORT, R_SENSE, 0b00);

void move(void *pvParameters) { stepper.move(0); }

JMotorController::JMotorController() {}

void JMotorController::Task1code(void *pvParameters) {
  Serial.print("Motor update running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    // if (stepper.distanceToGo() == 0)
    // stepper.disableOutputs();
    stepper.run();
    // if (steps != 0) {
    //   stepper.move(steps);
    //   stepper.enableOutputs();
    //   steps = 0;
    // }
    // vTaskDelay(1);
    // vTaskDelay(1 / portTICK_PERIOD_MS);
    vTaskDelay(pdMS_TO_TICKS(1)); // Delay for 1000 milliseconds
  }
}
void JMotorController::initMotor() {
  // SPI.begin();
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  driver.begin(); // Initiate pins and registeries
  driver.toff();
  driver.rms_current(
      300); // Set stepper current to 600mA. The command is the
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
      0);        // Core 0!?
}
void JMotorController::receiveMotorCommands(const uint8_t *mac_addr,
                                            const uint8_t *data, int data_len,
                                            int id) {
  float val;
  memcpy(&val, data + 6 + 1 + 1, sizeof(float));

  // Serial.println("Motor cmd");
  switch (data[6 + 1]) {
  case 0x01: // move (rel)
    stepper.move(val * steps_per_mm);
    // moveFuncParameters p;
    // steps = val * steps_per_mm;
    // xTaskCreatePinnedToCore(
    //     move,       /* Task function. */
    //     "moveTask", /* name of task. */
    //     10000,      /* Stack size of task */
    //     NULL,       /* parameter of the task */
    //     2,          /* priority of the task */
    //     &moveTask,  /* Task handle to keep track of created task */
    //     1);

    // Serial.println("Move");
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
  case 0x05:
  case 0x06: { // moveN
    char numValues =
        (data_len - 6 - 2) /
        sizeof(float); // Subtract 6 for address, and 1 for motorMsgType
    if (id < numValues) {
      memcpy(&val, data + 8 + (sizeof(float) * id), sizeof(float));
      if (data[7] == 0x05) {
        stepper.move(val * steps_per_mm);
      } else {
        stepper.moveTo(val * steps_per_mm);
      }
      stepper.enableOutputs();
    }
  } break;
  }
}
