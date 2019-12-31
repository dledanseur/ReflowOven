#include "soldermanager.hpp"
#include <ArduinoLog.h>

SolderManager::SolderManager(TemperatureManager& temperatureManager, BuzzerManager& buzzerManager,
                            uint8_t ssrPin):
    temperatureManager(temperatureManager), buzzerManager(buzzerManager), ssrPin(ssrPin) 
{
    pinMode(ssrPin, OUTPUT);
}

bool SolderManager::isFinished() { 
    return m_finished; 
};

void SolderManager::start(const Profile* profile) {
    this->profile = profile;
    this->startTime = millis();
    this->m_finished = false;
    this->solderState = WARMUP;
    this->buzzerManager.triTone(300, 300, 300);

}

void SolderManager::stop() {
    digitalWrite(this->ssrPin, LOW);
    this->m_finished = true;
    buzzerManager.triTone(900,-300, 300);
}

void SolderManager::loop() {
    if (m_finished)
        return;

    uint32_t now = millis();
    
    if (now-lastLoopMillis<1000) {  // don't do anything in less than one second
        return;
    }

    lastLoopMillis = now;

    auto temp = this->temperatureManager.getThermcplTemp();


    uint32_t ref = (float) (now - this->warmupEndTime) / 1000;
    
    double expectedValue;
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

    if (m_finished) {
        buzzerManager.triTone(900,-300, 300);
    }

}

String SolderManager::getSolderStateString() {
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

String SolderManager::getTimeString() {

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