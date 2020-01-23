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
    this->warmupEndTime = 0;
    this->buzzerManager.triTone(300, 300, 300);
    this->fireStarted(profile->name);
    clearAllPoints();
    digitalWrite(ssrPin, 1);
    Log.verbose("Warming up" CR);
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
    
    currentDutyTime += now - lastLoopMillis;

    lastLoopMillis = now;

    auto temp = this->temperatureManager.getThermcplTemp();
    auto junctionTemp = this->temperatureManager.getColdjTemp();

    uint32_t ref = (float) (now - this->warmupEndTime) / 1000;

    double expectedValue=calculateTempPoint(ref, temp, now);

    double dutyTime = 1;
    double diff = expectedValue - temp;


    if (solderState == WARMUP || solderState == REFLOW) {
        dutyTime = 1;
    }
    else {
        // only consider positive differential influence
        if (profile->p_temp_correction_range > -1 && diff > 0) {
            dutyTime = diff/profile->p_temp_correction_range;
        }
        else {
            dutyTime = 0;
        }
        double stateEndTime = getStateEndTime(solderState);
        Log.verbose("State end time %D, ref is %i" CR, stateEndTime, ref);

        if (stateEndTime - ref < profile->p_inertial_offset_time) {
            SolderState nextState = getNextState(solderState);
            dutyTime += getDutyCycleForState(nextState);
            Log.verbose("Switching to next state %i duty time %D" CR, nextState, dutyTime);
        }
        else {
            dutyTime += getDutyCycleForState(solderState);
        }
    }
    
    computeNextState(ref, temp, now);

    if (expectedValue == 0 && solderState == REFLOW) {
        this->m_finished = true;
    }

    // let's compute the action (on/off)
    if (!m_finished) {
        // warm up
        uint32_t dutyTimeOffset = currentDutyTime % dutyTimeCycle;
        Log.verbose("Current duty time %i, duty time Cycle %i, duty Time %D" CR, 
                    dutyTimeOffset, dutyTimeCycle, dutyTime);

        if ( (double) dutyTimeOffset / dutyTimeCycle > dutyTime) {
            // cool down
            digitalWrite(ssrPin, 0);
            Log.verbose("Cool down" CR);
        }
        else {
            // cool down
            digitalWrite(ssrPin, 1);
            Log.verbose("Warm up" CR); 
        }
    }
    else {
        // cool down
        digitalWrite(ssrPin, 0);
        Log.verbose("Cool down" CR);
    }

    

    if (now-lastPointMillis>1000) {
        lastPointMillis = now;
        this->expectedTemperature = expectedValue;
        Point *p = new Point();
        p->expected_temp = expectedValue;
        p->mesured_temperature = temp;
        p->state = solderState;
        p->millis = millisSinceStartup;
        p->internal_temp = junctionTemp;
        

        this->m_allPoints.push_back(p);
        this->fireNewPoint(*p);
    }
    if (m_finished) {
        buzzerManager.triTone(900,-300, 300);
        this->fireStopped();
    }
}

double SolderManager::calculateTempPoint(uint16_t ref, double temp, uint32_t now) {
    double expectedValue=0;
    if (solderState == WARMUP) {
        expectedValue = this->profile->p_warmup;
        if (temp>expectedValue) {
            solderState = PREHEAT;
            warmupEndTime = now;
        }
    }
    else if ( solderState != WARMUP && ref < this->profile->p_preheat_time ) {
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

    return expectedValue;
}

void SolderManager::computeNextState(uint16_t ref, double temp, uint32_t now) {
    if (solderState == WARMUP) {
        double expectedValue = this->profile->p_warmup;
        if (temp>=expectedValue) {   
            solderState = PREHEAT;
            warmupEndTime = now;
        }
    }
    else if ( ref < this->profile->p_preheat_time ) {
        solderState = PREHEAT;
    }
    else if ( ref < this->profile->p_soak_time ) {
        solderState = SOACK;
    }
    else if (ref < this->profile->p_reflow_time) {
        solderState = REFLOW;
    }
}

/**
 * Returns the slope for the given state.
 **/ 
double SolderManager::getStateSlope(SolderState state) {
    if (state == WARMUP) {
        return maxSlope;
    }
    else if (state == PREHEAT) {
        return (double) (this->profile->p_preheat_temp - this->profile->p_warmup) / 
                        (this->profile->p_preheat_time);
    }
    else if ( state == SOACK ) {
        return (double) (this->profile->p_soak_temp - this->profile->p_preheat_temp) / 
                        (this->profile->p_soak_time - this->profile->p_preheat_time);
    }
    else if ( state == REFLOW) {
        return (double) (this->profile->p_reflow_temp - this->profile->p_soak_temp) / 
                        (this->profile->p_reflow_time - this->profile->p_soak_time);
    }
}

/**
 * Return the duty cycle for the given state (between 0 and 1)
 **/
double SolderManager::getDutyCycleForState(SolderState state) {
    double slope = getStateSlope(state);
    double dutyCyle = slope / maxSlope;
    Log.verbose("State slope for state %i is %D, duty Cycle %D" CR, state, slope, dutyCyle);
    return dutyCyle;

}

double SolderManager::getStateStartTime(SolderState solderState) {
    if (solderState == PREHEAT) {
        return (warmupEndTime/1000);
    }
    else if (solderState == SOACK) {
        return profile->p_preheat_time + (warmupEndTime/1000);
    }
    else if (solderState == REFLOW) {
        return profile->p_soak_time + (warmupEndTime/1000);
    }
}

double SolderManager::getStateEndTime(SolderState solderState) {
    if (solderState == PREHEAT) {
        return profile->p_preheat_time;
    }
    else if (solderState == SOACK) {
        return profile->p_soak_time;
    }
    else if (solderState == REFLOW) {
        return profile->p_reflow_time;
    }
}

SolderState SolderManager::getNextState(SolderState solderState) {
    if (solderState == WARMUP) {
        return PREHEAT;
    }
    if (solderState == PREHEAT) {
        return SOACK;
    }
    else if (solderState == SOACK) {
        return REFLOW;
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