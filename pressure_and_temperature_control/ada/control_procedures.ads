with Data_Types; use Data_Types;

package Control_Procedures is
    -- Calculates the correct heater setting if the temperature is to be
    -- regulated to 20 (degrees). Requires the current temperature (TR) and
    -- stores the result in HS.
    procedure Temp_Convert(TR : TempReading; HS : out HeaterSetting);
    -- Calculates the correct valve setting if the pressure is to be regulated
    -- to 1000 (mbar). Requires the current pressure (PR) and stores the result
    -- in PS.
    procedure Pressure_Convert(PR : PressureReading; PS : out PressureSetting);
end Control_Procedures;
