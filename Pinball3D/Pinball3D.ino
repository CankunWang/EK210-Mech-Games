#include "src/actuator_link.h"
#include "src/game_fsm.h"
#include "src/input_manager.h"
#include "src/pins_config.h"
#include "src/score_rules.h"
#include "src/ui_lcd.h"

const AppConfig kConfig = makeDefaultConfig();

ActuatorLink gActuatorLink(kConfig, Serial1);
InputManager gInputs(kConfig);
ScoreRules gScore(kConfig);
UiLcd gUi(kConfig);
GameFsm gGame(kConfig, gInputs, gActuatorLink, gScore, gUi);

void setup() {
  gActuatorLink.begin();
  gInputs.begin();
  gUi.begin();
  gGame.begin();
}

void loop() {
  const uint32_t nowMs = millis();
  gActuatorLink.update(nowMs);
  gGame.tick(nowMs);
}
