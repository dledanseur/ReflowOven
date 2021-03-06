#include "commands.hpp"
#include "Arduino.h"

void cancelTimer(TouchManager& manager) {
    manager.cancelScheduledCommand(AUTOSTOP_TAG);
    //esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
}

UpCommand::UpCommand(State& state): m_state(state) {}

void UpCommand::execute(TouchManager& manager) {
    m_state.currentProfileIndex--;
    if (m_state.currentProfileIndex<0){
        m_state.currentProfileIndex = PROFILE_NUMBER - 1;
    }
    m_state.currentProfile = ALL_PROFILES[m_state.currentProfileIndex];
}

uint8_t UpCommand::getTag() {
    return UpCommand::TAG;
}

DownCommand::DownCommand(State& state): m_state(state) {}

void DownCommand::execute(TouchManager& manager) {
    m_state.currentProfileIndex++;
    if (m_state.currentProfileIndex>=PROFILE_NUMBER){
        m_state.currentProfileIndex = 0;
    }
    m_state.currentProfile = ALL_PROFILES[m_state.currentProfileIndex];
}

uint8_t DownCommand::getTag() {
    return DownCommand::TAG;
}

StartCommand::StartCommand(SolderManager& solderManager, State& state): 
    solderManager(solderManager), m_state(state)
{   
}
void StartCommand::execute(TouchManager& manager) {
    solderManager.start(m_state.currentProfile);
}

uint8_t StartCommand::getTag() {
    return StartCommand::TAG;
}

StopCommand::StopCommand(SolderManager& solderManager): solderManager(solderManager)
{   
}
void StopCommand::execute(TouchManager& manager) {
    solderManager.stop();
}
uint8_t StopCommand::getTag() {
    return StopCommand::TAG;
}
