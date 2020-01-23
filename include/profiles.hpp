#ifndef PROFILES_HPP
#define PROFILES_HPP

#include <Arduino.h>

struct Profile {
    const char* name;
    uint16_t p_warmup;
    uint16_t p_preheat_temp;
    uint16_t p_preheat_time;
    uint16_t p_soak_temp;
    uint16_t p_soak_time;
    uint16_t p_reflow_temp;
    uint16_t p_reflow_time;
    uint16_t p_inertial_offset_time;
    int16_t p_temp_correction_range;
};

#define PROFILE_NUMBER 2

extern const Profile* ALL_PROFILES[];

#endif
