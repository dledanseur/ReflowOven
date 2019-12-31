#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "touchmanager/button.hpp"
#include "touchmanager/touch_manager.hpp"
#include "profiles.hpp"
#include "soldermanager.hpp"



#define RELAIS_UP 32
#define RELAIS_DOWN 33
#define STOP_CMD_TAG 1
#define AUTOSTOP_TAG 1
// Time after which all commands will be canceled
#define AUTOSTOP_DELAY 15000

struct State {
    int currentProfileIndex = 0;
    const Profile* currentProfile = ALL_PROFILES[0];
    bool started;
    
};

class StopCommand : public Command {
    private:
    SolderManager& solderManager;
    public:
    StopCommand(SolderManager& solderManager);
    virtual void execute(TouchManager& manager);
};

class UpCommand : public Command {
    private:
    State& m_state;

    public:
    UpCommand(State& state);
    virtual void execute(TouchManager& manager);
};

class DownCommand : public Command {
    private:
    State& m_state;

    public:
    DownCommand(State& state);
    virtual void execute(TouchManager& manager);
};

class StartCommand : public Command {
    private:
    SolderManager& solderManager;
    State& m_state;

    public:
    StartCommand(SolderManager& solderManager, State& state);
    virtual void execute(TouchManager& manager);
};

#endif