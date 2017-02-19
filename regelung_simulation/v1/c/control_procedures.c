#include "control_procedures.h"

void convert_temp(temp_reading_t TR, heater_setting_t *HS) {
    if (TR < 30) {
        *HS = ON;
    } else {
        *HS = OFF;
    }
}

void convert_pressure(pressure_reading_t PR, pressure_setting_t *PS) {
    int diff = 100 - PR;
    if (diff > 4) {
        *PS = 9;
    } else if (diff <= -4) {
        *PS = 0;
    } else {
        *PS = diff+4;
    }
}
