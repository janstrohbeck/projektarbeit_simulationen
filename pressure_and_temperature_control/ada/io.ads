with Data_Types; use Data_Types;

package IO is
    -- procedures for data exchange with the environment
    procedure Read(TR : out Temp_Reading); -- from DAC
    procedure Read(PR : out Pressure_Reading); -- from DAC
    procedure Write(HS : Heater_Setting); -- to switch.
    procedure Write(PS : Pressure_Setting); -- to DAC
    procedure Write(TR : Temp_Reading); -- to console
    procedure Write(PR : Pressure_Reading); -- to console
private
    DISPLAY_LOGGER : constant Boolean := True;
    DISPLAY_CONSOLE : constant Boolean := True;
end IO;
