with Data_Types; use Data_Types;
package Control_Procedures is
    -- procedures for converting a reading into
    -- an appropriate setting for output to
    procedure Temp_Convert(TR : Temp_Reading;
        HS : out Heater_Setting);
    procedure Pressure_Convert(PR : Pressure_Reading;
        PS : out Pressure_Setting);
end Control_Procedures;
