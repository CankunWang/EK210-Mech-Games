#include "src/game_fsm.h"
#include "src/input_manager.h"
#include "src/launcher_servo.h"
#include "src/motor_driver.h"
#include "src/pins_config.h"
#include "src/score_rules.h"
#include "src/ui_lcd.h"

const AppConfig kConfig = makeDefaultConfig();

InputManager gInputs(kConfig);
MotorDriver gMotors(kConfig);
LauncherServo gLauncher(kConfig);
ScoreRules gScore;
UiLcd gUi(kConfig);
GameFsm gGame(kConfig, gInputs, gMotors, gLauncher, gScore, gUi);

void setup() {
  gInputs.begin();
  gMotors.begin();
  gLauncher.begin();
  gUi.begin();
  gGame.begin();
}

void loop() {
  const uint32_t nowMs = millis();
  gGame.tick(nowMs);
}

