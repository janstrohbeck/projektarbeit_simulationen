with Ada.Numerics.Discrete_Random;
with Ada.Text_IO; use Ada.Text_IO;

package body IO is
    -- Data type for results of the random generator
    subtype RandomResult is Integer range 0 .. 99;
    -- Instantiation of the random generator package
    package RandomGenerator is new Ada.Numerics.Discrete_Random(RandomResult);
    use RandomGenerator;

    protected RandomState is
        -- Inititalizes the random generator
        procedure InitRandom;
        -- Gets the next random number from the random generator
        function GetRandom return RandomResult;
    private
        -- Random generator seed
        seed: Generator;
    end RandomState;

    protected body RandomState is
        -- Inititalizes the random generator
        procedure InitRandom is begin Reset(seed); end InitRandom;
        -- Gets the next random number from the random generator
        function GetRandom return RandomResult is (Random(seed));
    end RandomState;

    protected TempState is
        function GetTemp return TempReading;
        procedure SetTemp(TR: TempReading);
        function GetHeaterSetting return HeaterSetting;
        procedure SetHeaterSetting(HS: HeaterSetting);
    private
        temp : TempReading := 0;
        heater_setting : HeaterSetting := Off;
    end TempState;

    protected body TempState is
        function GetTemp return TempReading is (temp);
        procedure SetTemp(TR: TempReading) is begin temp := TR; end SetTemp;
        function GetHeaterSetting return HeaterSetting is (heater_setting);
        procedure SetHeaterSetting(HS: HeaterSetting) is begin heater_setting := HS; end SetHeaterSetting;
    end TempState;

    task TempSimulator;
    task body TempSimulator is
        newTemp : Integer;
    begin
        loop
            newTemp := Integer(TempState.GetTemp);

            if RandomState.GetRandom < 6 then
                if RandomState.GetRandom < 50 then
                    newTemp := newTemp + 4;
                else
                    newTemp := newTemp - 4;
                end if;
            end if;

            if TempState.GetHeaterSetting = On then
                newTemp := newTemp + 1;
            else
                newTemp := newTemp - 2;
            end if;

            if newTemp < Integer(TempReading'First) then
                newTemp := Integer(TempReading'First);
            elsif newTemp > Integer(TempReading'Last) then
                newTemp := Integer(TempReading'Last);
            end if;

            TempState.SetTemp(TempReading(newTemp));
            delay 0.25;
        end loop;
    end TempSimulator;

    protected PressureState is
        function GetPressure return PressureReading;
        procedure SetPressure(PR: PressureReading);
        function GetPressureSetting return PressureSetting;
        procedure SetPressureSetting(PS: PressureSetting);
    private
        pressure : PressureReading := 800;
        setting : PressureSetting := 0;
    end PressureState;

    protected body PressureState is
        function GetPressure return PressureReading is (pressure);
        procedure SetPressure(PR: PressureReading) is begin pressure := PR; end SetPressure;
        function GetPressureSetting return PressureSetting is (setting);
        procedure SetPressureSetting(PS: PressureSetting) is begin setting := PS; end SetPressureSetting;
    end PressureState;

    task PressureSimulator;
    task body PressureSimulator is
        newPressure : Integer;
    begin
        loop
            newPressure := Integer(PressureState.GetPressure);
            if RandomState.GetRandom < 10 then
                if RandomState.GetRandom < 50 then
                    newPressure := newPressure + 30;
                else
                    newPressure := newPressure - 30;
                end if;
            else
                newPressure := newPressure + Integer(PressureState.GetPressureSetting);
            end if;

            if newPressure < Integer(PressureReading'First) then
                newPressure := Integer(PressureReading'First);
            elsif newPressure > Integer(PressureReading'Last) then
                newPressure := Integer(PressureReading'Last);
            end if;

            PressureState.SetPressure(PressureReading(newPressure));
            delay 0.25;
        end loop;
    end PressureSimulator;

    task Logger;
    task body Logger is
    begin
        if DISPLAY_LOGGER then
            loop
                Put_Line("Temp: " & TempReading'Image(TempState.GetTemp) &
                        " (Heater: " & HeaterSetting'Image(TempState.GetHeaterSetting) & ")" &
                        " | Pressure: " & PressureReading'Image(PressureState.GetPressure) &
                        " (Setting: " & PressureSetting'Image(PressureState.GetPressureSetting) & ")");
                delay 0.25;
            end loop;
        end if;
    end Logger;

    procedure Read(TR : out TempReading) is
    begin
        TR := TempState.GetTemp;
    end Read;

    procedure Read(PR : out PressureReading) is
    begin
        PR := PressureState.GetPressure;
    end Read;

    procedure Write(HS : HeaterSetting) is
    begin
        TempState.SetHeaterSetting(HS);
    end Write;

    procedure Write(PS : PressureSetting) is
    begin
        PressureState.SetPressureSetting(PS);
    end Write;

    procedure Write(TR : TempReading) is
    begin
        if DISPLAY_CONSOLE then
            Put_Line("Console: Read Temp: " & TempReading'Image(TR));
        end if;
        null;
    end Write;

    procedure Write(PR : PressureReading) is
    begin
        if DISPLAY_CONSOLE then
            Put_Line("Console: Read Pressure: " & PressureReading'Image(PR));
        end if;
    end Write;
begin
    RandomState.InitRandom;
end IO;
