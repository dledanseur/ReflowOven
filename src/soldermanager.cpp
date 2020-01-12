#include "soldermanager.hpp"
#include <ArduinoLog.h>

SolderManager::SolderManager(TemperatureManager& temperatureManager, BuzzerManager& buzzerManager,
                            uint8_t ssrPin):
    temperatureManager(temperatureManager), buzzerManager(buzzerManager), ssrPin(ssrPin) 
{
    pinMode(ssrPin, OUTPUT);
}

bool SolderManager::isFinished() const { 
    return m_finished; 
};

void SolderManager::start(const Profile* profile) {
    this->profile = profile;
    this->startTime = millis();
    this->m_finished = false;
    this->solderState = WARMUP;
    this->buzzerManager.triTone(300, 300, 300);
    this->fireStarted(profile->name);
    clearAllPoints();
}

void SolderManager::stop() {
    digitalWrite(this->ssrPin, LOW);
    this->m_finished = true;
    buzzerManager.triTone(900,-300, 300);
    this->fireStopped();
}

void SolderManager::loop() {
    if (m_finished)
        return;

    uint32_t now = millis();
    
    uint32_t millisSinceStartup = now - startTime;

    if (now-lastLoopMillis<1000) {  // don't do anything in less than one second
        return;
    }

    lastLoopMillis = now;

    auto temp = this->temperatureManager.getThermcplTemp();

    uint32_t ref = (float) (now - this->warmupEndTime) / 1000;
    
    double expectedValue=0;
    if (solderState == WARMUP) {
        expectedValue = this->profile->p_warmup;
        if (temp>expectedValue) {
            solderState = PREHEAT;
            warmupEndTime = now;
        }
    }
    else if ( solderState == PREHEAT && ref < this->profile->p_preheat_time ) {
        double slope = (double) (this->profile->p_preheat_temp - this->profile->p_warmup) / 
                                (this->profile->p_preheat_time);
        
        expectedValue = slope * ref + this->profile->p_warmup;
    }
    else if ( ref < this->profile->p_soak_time ) {
        solderState = SOACK;

        double slope = (double) (this->profile->p_soak_temp - this->profile->p_preheat_temp) / 
                        (this->profile->p_soak_time - this->profile->p_preheat_time);
        
        expectedValue = slope * (ref - this->profile->p_preheat_time) + this->profile->p_preheat_temp;
    }
    else if (ref < this->profile->p_reflow_time) {
        solderState = REFLOW;

        double slope = (double) (this->profile->p_reflow_temp - this->profile->p_soak_temp) / (this->profile->p_reflow_time - this->profile->p_soak_time);
        
        expectedValue = slope * (ref - this->profile->p_soak_time) + this->profile->p_soak_temp;
    }
    else {
        this->m_finished = true;
    }
    double diff = expectedValue - temp;
    if (diff > 1 && !m_finished) {
        // warm up
        digitalWrite(ssrPin, 1);
        Log.verbose("Warming up" CR);
    }
    else if (diff < - 1 || m_finished) {
        // cool down
        digitalWrite(ssrPin, 0);
        Log.verbose("Cool down" CR);
    }

    this->expectedTemperature = expectedValue;
    Point *p = new Point();
    p->expected_temp = expectedValue;
    p->mesured_temperature = temp;
    p->state = solderState;
    p->millis = millisSinceStartup;

    this->m_allPoints.push_back(p);
    this->fireNewPoint(*p);

    if (m_finished) {
        buzzerManager.triTone(900,-300, 300);
        this->fireStopped();
    }

}

double SolderManager::getExpectedTemperature() const {
    return expectedTemperature;
}

SolderState SolderManager::getSolderState() {
    return solderState;
}

String SolderManager::getSolderStateString() const {
    if (m_finished) {
        return "Idle";
    }

    switch(solderState) {
        case WARMUP:
        return "Warmup";

        case PREHEAT:
        return "Preheat";

        case SOACK:
        return "Soack";

        case REFLOW:
        return "Reflow";
    }

    return "Unknown";
}

String SolderManager::getTimeString() const {

    if (m_finished) {
        return "00:00";
    }

    uint32_t now = millis();

    uint32_t millis = now - startTime;

    uint8_t minutes = millis/1000/60;

    uint8_t seconds = (millis-(minutes*1000*60))/1000;

    String strMinutes = String(minutes);
    String strSeconds = String(seconds);

    if (strMinutes.length() <= 1) {
        strMinutes = "0" + strMinutes;
    }

    if (strSeconds.length() <= 1) {
        strSeconds = "0" + strSeconds;
    }
    return strMinutes + ":" + strSeconds;

}

void SolderManager::addListener(SolderManagerListener& listener) {
    m_listeners.push_back(&listener);
}

void SolderManager::fireNewPoint(Point& p) {
    for (auto l : m_listeners) {
        l->newPoint(p);
    }
}

void SolderManager::fireStarted(String profile) {
    for (auto l : m_listeners) {
        l->started(profile);
    }
}

void SolderManager::fireStopped() {
    for (auto l : m_listeners) {
        l->stopped();
    }
}

std::vector<const Point*> SolderManager::getAllPoints() const {
    return m_allPoints;
}

void SolderManager::clearAllPoints() {
    auto delPoint = [](const Point* p) { delete p; };

    for_each(this->m_allPoints.begin(), this->m_allPoints.end(), delPoint);

    m_allPoints.clear();
}

SolderManager::~SolderManager() {
    clearAllPoints();
}


SolderManagerListener::~SolderManagerListener() {}