// author: altnskuur@gmail.com

#pragma once

// INCLUDES
#include "Arduino.h"
#include <esp32-hal-timer.h>

// GLOBAL VARIABLES
int g_motorPinStep;

auto IRAM_ATTR stepPulseISR() -> void {  
  
  REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) ^ (1 << motorP)); 
}

namespace motors {

  class Step {

    public:
      // CONSTRUCTOR - DESTRUCTOR
      Step(int enablePin, int dirPin, int motorPin, int timerNum) : m_enablePin{enablePin}, m_dirPin{dirPin}, m_motorPin{motorPin}, m_timerNum{timerNum} {

        // Enable Motor Pins
        pinMode(m_motorPin, OUTPUT);
        pinMode(m_dirPin, OUTPUT);
        pinMode(m_enablePin, OUTPUT);

        g_motorPinStep = motorPin;

        float timeInterval = 30 / (m_stepPerRev * m_RPM);
        unsigned long interruptCounter = timeInterval * sc_clockFq;

        // Time Settings
        m_timer = timerBegin(m_timerNum, 2, true); // true param means turns continuously
        timerAttachInterrupt(m_timer, &stepPulseISR, true);
        timerAlarmWrite(m_timer, interruptCounter, true);

        digitalWrite(m_enablePin, HIGH);
      }

      ~Step() = default;

      // FUNCTIONS
      auto turn_start(void) -> void {

        timerAlarmEnable(m_timer);
        digitalWrite(m_enablePin, LOW);
      }

      auto turn_stop(void) -> void {

        timerAlarmDisable(m_timer);
        digitalWrite(m_enablePin, HIGH);
      }

      auto turn_cont(void) -> void {

        digitalWrite(m_enablePin, LOW);
      }

      auto turn_pause(void) -> void {

        digitalWrite(m_enablePin, HIGH);
      }

      auto dir_positive(void) -> void {

        digitalWrite(m_dirPin, HIGH);
      }

      auto dir_negative(void) -> void {

        digitalWrite(m_dirPin, LOW);
      }

      auto set_RPM(float RPM) -> void {

        m_RPM = RPM;
      }

    private:
      int m_enablePin;
      int m_dirPin;
      int m_motorPin;
      int m_timerNum;
      int m_stepPerRev = 800; // default value 800
      float m_RPM = 50; // default value 50
      hw_timer_t *m_timer = NULL;

      static const int sc_clockFq = 40000000;
  };
} // namespace motors
