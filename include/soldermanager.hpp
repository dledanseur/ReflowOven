#ifndef SOLDERMANAGER_HPP
#define SOLDERMANAGER_HPP

#include "profiles.hpp"
#include "temperaturemanager.hpp"
#include "buzzermanager.hpp"

enum SolderState {
  WARMUP,
  PREHEAT,
  SOACK,
  REFLOW
};

class SolderManagerListener {
  virtual void newPoint(uint32_t millis, SolderState state, 
                        double expected_temp, double mesured_temperature);
};

class SolderManager {
  private:
    const Profile* profile;
    uint32_t startTime;
    uint32_t warmupEndTime;
    TemperatureManager& temperatureManager;
    BuzzerManager& buzzerManager;
    bool m_finished=true;
    uint8_t ssrPin;
    uint32_t lastLoopMillis=0;
    SolderState solderState;
    double expectedTemperature=0;
  public:
    SolderManager(TemperatureManager& temperatureManager, 
                  BuzzerManager& buzzerManager, uint8_t ssrPin);
    void start(const Profile* profile);
    void stop();
    bool isFinished();
    void loop();
    String getSolderStateString();
    String getTimeString();
    double getExpectedTemperature();
};
#endif