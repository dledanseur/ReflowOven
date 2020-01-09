#include "profiles.hpp"

const Profile Sn42Bi58 = {"Sn42-Bi58", 50, 105, 80, 140, 185, 170, 215};
const Profile Sn63Pb37 = {"Sn63-Pb37", 50, 100, 60, 160, 240, 215, 300};     // °C, °C, s, °C, s, °C, s, °C, s, °C, s

const Profile* ALL_PROFILES[] = {&Sn42Bi58, &Sn63Pb37};
