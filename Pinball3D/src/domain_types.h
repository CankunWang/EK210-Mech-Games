#pragma once

#include <Arduino.h>

enum class GameState : uint8_t {
  Idle = 0,
  Ready,
  Playing,
  BallLost,
  GameOver,
};

enum class ButtonId : uint8_t {
  None = 0,
  Btn1,
  Btn2,
  Btn3,
  Btn4,
};

enum class MotorId : uint8_t {
  Motor0 = 0,
  Motor1,
  Motor2,
  Motor3,
};

enum class MotorDirection : uint8_t {
  Stop = 0,
  Forward,
  Reverse,
  Brake,
};

struct MotorCommand {
  MotorDirection direction = MotorDirection::Stop;
  uint8_t speed = 0;  // 0-255
};

struct InputSnapshot {
  bool buttonDown[4] = {false, false, false, false};
  bool buttonPressedEdge[4] = {false, false, false, false};
  bool buttonReleasedEdge[4] = {false, false, false, false};

  uint16_t tcrtRaw[3] = {0, 0, 0};
  bool tcrtDetected[3] = {false, false, false};
  bool tcrtPassEdge[3] = {false, false, false};

  bool breakBeamActive = false;
  bool breakBeamEdge = false;

  uint16_t ultrasonicDistanceCm = 0;
  bool ultrasonicActive = false;
  bool ultrasonicEdge = false;

  bool limitActive[4] = {false, false, false, false};
};

struct ScoreState {
  uint32_t score = 0;
  uint8_t combo = 0;
};
