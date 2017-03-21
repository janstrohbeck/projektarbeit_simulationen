#include "control_procedures.h"

void convert_temp(temp_reading_t TR, heater_setting_t *HS) {
    if (TR < 20) {
        *HS = ON;
    } else {
        *HS = OFF;
    }
}

void convert_pressure(pressure_reading_t PR, pressure_setting_t *PS) {
    int diff = 1000 - PR;
    if (diff > MAX_PRESSURE_SETTING) {
        *PS = MAX_PRESSURE_SETTING;
    } else if (diff < MIN_PRESSURE_SETTING) {
        *PS = MIN_PRESSURE_SETTING;
    } else {
        *PS = diff;
    }
}
