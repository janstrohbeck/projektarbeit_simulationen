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
        function Temp return Temp_Reading;
        procedure SetTemp(TR: Temp_Reading);
        function Heater return Heater_Setting;
        procedure SetHeater(HS: Heater_Setting);
    private
        mTemp : Temp_Reading := 0;
        mHeater : Heater_Setting := Off;
    end TempState;

    protected body TempState is
        function Temp return Temp_Reading is (mTemp);
        procedure SetTemp(TR: Temp_Reading) is begin mTemp := TR; end SetTemp;
        function Heater return Heater_Setting is (mHeater);
        procedure SetHeater(HS: Heater_Setting) is begin mHeater := HS; end SetHeater;
    end TempState;

    task TempSimulator;
    task body TempSimulator is
        newTemp : Integer;
    begin
        loop
            newTemp := Integer(TempState.Temp);

            if RandomState.GetRandom < 6 then
                if RandomState.GetRandom < 50 then
                    newTemp := newTemp + 4;
                else
                    newTemp := newTemp - 4;
                end if;
            end if;

            if TempState.Heater = On then
                newTemp := newTemp + 1;
            else
                newTemp := newTemp - 2;
            end if;

            if newTemp < Integer(Temp_Reading'First) then
                newTemp := Integer(Temp_Reading'First);
            elsif newTemp > Integer(Temp_Reading'Last) then
                newTemp := Integer(Temp_Reading'Last);
            end if;

            TempState.SetTemp(Temp_Reading(newTemp));
            delay 0.25;
        end loop;
    end TempSimulator;

    protected PressureState is
        function Pressure return Pressure_Reading;
        procedure SetPressure(PR: Pressure_Reading);
        function Setting return Pressure_Setting;
        procedure SetSetting(PS: Pressure_Setting);
    private
        mPressure : Pressure_Reading := 800;
        mSetting : Pressure_Setting := 0;
    end PressureState;

    protected body PressureState is
        function Pressure return Pressure_Reading is (mPressure);
        procedure SetPressure(PR: Pressure_Reading) is begin mPressure := PR; end SetPressure;
        function Setting return Pressure_Setting is (mSetting);
        procedure SetSetting(PS: Pressure_Setting) is begin mSetting := PS; end SetSetting;
    end PressureState;

    task PressureSimulator;
    task body PressureSimulator is
        newPressure : Integer;
    begin
        loop
            newPressure := Integer(PressureState.Pressure);
            if RandomState.GetRandom < 10 then
                if RandomState.GetRandom < 50 then
                    newPressure := newPressure + 30;
                else
                    newPressure := newPressure - 30;
                end if;
            else
                newPressure := newPressure + Integer(PressureState.Setting);
            end if;

            if newPressure < Integer(Pressure_Reading'First) then
                newPressure := Integer(Pressure_Reading'First);
            elsif newPressure > Integer(Pressure_Reading'Last) then
                newPressure := Integer(Pressure_Reading'Last);
            end if;

            PressureState.SetPressure(Pressure_Reading(newPressure));
            delay 0.25;
        end loop;
    end PressureSimulator;

    task Logger;
    task body Logger is
    begin
        loop
            Put_Line("Temp: " & Temp_Reading'Image(TempState.Temp) &
                    " (Heater: " & Heater_Setting'Image(TempState.Heater) & ")" &
                    " | Pressure: " & Pressure_Reading'Image(PressureState.Pressure) &
                    " (Setting: " & Pressure_Setting'Image(PressureState.Setting) & ")");
            delay 0.25;
        end loop;
    end Logger;

    procedure Read(TR : out Temp_Reading) is
    begin
        TR := TempState.Temp;
    end Read;

    procedure Read(PR : out Pressure_Reading) is
    begin
        PR := PressureState.Pressure;
    end Read;

    procedure Write(HS : Heater_Setting) is
    begin
        TempState.SetHeater(HS);
    end Write;

    procedure Write(PS : Pressure_Setting) is
    begin
        PressureState.SetSetting(PS);
    end Write;

    procedure Write(TR : Temp_Reading) is
    begin
        --Put_Line("Console: Read Temp: " & Temp_Reading'Image(TR));
        null;
    end Write;

    procedure Write(PR : Pressure_Reading) is
    begin
        --Put_Line("Console: Read Pressure: " & Pressure_Reading'Image(PR));
        null;
    end Write;
begin
    RandomState.InitRandom;
end IO;
