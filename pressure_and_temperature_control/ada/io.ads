with Data_Types; use Data_Types;

package IO is
    -- Reads the current temperature value and stores it in TR.
    procedure Read(TR : out TempReading); -- from DAC
    -- Reads the current pressure value and stores it in PR.
    procedure Read(PR : out PressureReading); -- from DAC
    -- Sets a new setting for the heater.
    procedure Write(HS : HeaterSetting); -- to switch.
    -- Sets a new setting for the valve
    procedure Write(PS : PressureSetting); -- to DAC
    -- Prints a temperature value on the console.
    procedure Write(TR : TempReading); -- to console
    -- Prints a pressure value on the console.
    procedure Write(PR : PressureReading); -- to console
private
    -- The logger prints the current temperature and pressure values, as well as
    -- the current heater and valve settings.
    DISPLAY_LOGGER : constant Boolean := True;
    -- If set to True, the messages sent to the display are printed in the
    -- console
    DISPLAY_CONSOLE : constant Boolean := True;
end IO;
