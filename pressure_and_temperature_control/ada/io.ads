with Data_Types; use Data_Types;

package IO is
    -- procedures for data exchange with the environment
    procedure Read(TR : out TempReading); -- from DAC
    procedure Read(PR : out PressureReading); -- from DAC
    procedure Write(HS : HeaterSetting); -- to switch.
    procedure Write(PS : PressureSetting); -- to DAC
    procedure Write(TR : TempReading); -- to console
    procedure Write(PR : PressureReading); -- to console
private
    -- The logger prints the current temperature and pressure values, as well as
    -- the current heater and valve settings.
    DISPLAY_LOGGER : constant Boolean := True;
    -- If set to True, the messages sent to the display are printed in the
    -- console
    DISPLAY_CONSOLE : constant Boolean := True;
end IO;
