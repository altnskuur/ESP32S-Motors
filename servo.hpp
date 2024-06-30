// author: altnskuur@gmail.com

#pragma once

// INCLUDES
#include "Arduino.h"
#include <esp32-hal-timer.h>

// GLOBAL VARIABLES
int g_motorPinServo;

auto IRAM_ATTR onTimerServo() -> void {  
  
  REG_WRITE(GPIO_OUT_REG, REG_READ(GPIO_OUT_REG) ^ (1 << motorServo)); 
}

namespace motors
{
  class Servo
  {
    public:
      // CONSTRUCTOR / DESTRUCTOR
      Servo(int dirPin, int motorPin, int timerNum) : m_dirPin{dirPin}, m_motorPin{motorPin}, m_timerNum{timerNum} {

        // Enable Motor Pins
        pinMode(m_motorPin, OUTPUT);
        pinMode(m_dirPin, OUTPUT);

        g_motorPinServo = motorPin;

        float timeInterval = 30 / (m_stepPerRev * m_RPM);
        unsigned long interruptCounter = timeInterval * sc_clockFq;

        // Time Settings
        m_timer = timerBegin(m_timerNum, 2, true); // true param means turns continuously
        timerAttachInterrupt(m_timer, &onTimerServo, true);
        timerAlarmWrite(m_timer, interruptCounter, true);
      }

      ~Servo() = default;

      // FUNCTIONS
      auto turn_start(void) -> void {
        
        timerAlarmEnable(m_timer);
      }

      auto turn_stop(void) -> void {

        timerAlarmDisable(m_timer);
      }

      auto dir_positive(void) -> void {

        digitalWrite(m_dirPin, HIGH);
      }

      auto dir_negative(void) -> void {

        digitalWrite(m_dirPin, LOW);
      }

      auto set_rmp(float RPM) -> void {

        m_RPM = RPM;

        timerAlarmDisable(m_timer);

        float timeInterval = 30 / (m_stepPerRev * m_RPM);
        m_interruptCounter = timeInterval * sc_clockFq;
        timerAlarmWrite(m_timer, m_interruptCounter, true);

        timerAlarmEnable(m_timer);
      }

    private:
      int m_dirPin;
      int m_motorPin;
      int m_timerNum;
      int m_stepPerRev = 1840;
      float m_RPM = 1000;
      hw_timer_t *m_timer = NULL;

      static const int sc_clockFq = 40000000;
  };
} // namespace motors
