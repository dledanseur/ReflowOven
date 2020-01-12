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

struct Point {
  uint32_t millis;
  SolderState state;
  double expected_temp;
  double mesured_temperature;
};

class SolderManagerListener {
  public:
    virtual void started(String profile);
    virtual void newPoint(const Point& point);
    virtual void stopped();
    virtual ~SolderManagerListener();
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

    std::vector<const Point*> m_allPoints;
    std::vector<SolderManagerListener*> m_listeners;

    void fireNewPoint(Point& p);
    void fireStarted(String profile);
    void fireStopped();

    void clearAllPoints();

  public:
    SolderManager(TemperatureManager& temperatureManager, 
                  BuzzerManager& buzzerManager, 
                  uint8_t ssrPin);
    void start(const Profile* profile);
    void stop();
    bool isFinished() const;
    void loop();
    String getSolderStateString() const;
    String getTimeString() const;
    SolderState getSolderState();
    double getExpectedTemperature() const;
    std::vector<const Point*> getAllPoints() const;
    void addListener(SolderManagerListener& listener);

    

    ~SolderManager();
};
#endif