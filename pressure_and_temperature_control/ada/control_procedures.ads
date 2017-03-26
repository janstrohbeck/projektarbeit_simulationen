with Data_Types; use Data_Types;

package Control_Procedures is
    -- procedures for converting a reading into
    -- an appropriate setting for regulation
    procedure Temp_Convert(TR : TempReading; HS : out HeaterSetting);
    procedure Pressure_Convert(PR : PressureReading; PS : out PressureSetting);
end Control_Procedures;
