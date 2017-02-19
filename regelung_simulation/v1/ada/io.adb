with Ada.Text_IO; use Ada.Text_IO;

package body IO is
    protected Temp_State is
        function Temp return Temp_Reading;
        procedure SetTemp(TR: Temp_Reading);
        function Heater return Heater_Setting;
        procedure SetHeater(HS: Heater_Setting);
    private
        mTemp : Temp_Reading := 30;
        mHeater : Heater_Setting := Off;
    end Temp_State;

    protected body Temp_State is
        function Temp return Temp_Reading is (mTemp);
        procedure SetTemp(TR: Temp_Reading) is begin mTemp := TR; end SetTemp;
        function Heater return Heater_Setting is (mHeater);
        procedure SetHeater(HS: Heater_Setting) is begin mHeater := HS; end SetHeater;
    end Temp_State;

    task Temp_Simulator;
    task body Temp_Simulator is
    begin
        loop
            if Temp_State.Heater = On then
                Temp_State.SetTemp(Temp_State.Temp+1);
            else
                Temp_State.SetTemp(Temp_State.Temp-2);
            end if;
            delay 0.25;
        end loop;
    end Temp_Simulator;

    protected Pressure_State is
        function Pressure return Pressure_Reading;
        procedure SetPressure(PR: Pressure_Reading);
        function Setting return Pressure_Setting;
        procedure SetSetting(PS: Pressure_Setting);
    private
        mPressure : Pressure_Reading := 30;
        mSetting : Pressure_Setting := 0;
    end Pressure_State;

    protected body Pressure_State is
        function Pressure return Pressure_Reading is (mPressure);
        procedure SetPressure(PR: Pressure_Reading) is begin mPressure := PR; end SetPressure;
        function Setting return Pressure_Setting is (mSetting);
        procedure SetSetting(PS: Pressure_Setting) is begin mSetting := PS; end SetSetting;
    end Pressure_State;

    task Pressure_Simulator;
    task body Pressure_Simulator is
    begin
        loop
            Pressure_State.SetPressure(Pressure_Reading(Integer(Pressure_State.Pressure)+Integer(Pressure_State.Setting)-4));
            delay 0.25;
        end loop;
    end Pressure_Simulator;

    procedure Read(TR : out Temp_Reading) is
    begin
        TR := Temp_State.Temp;
    end Read;

    procedure Read(PR : out Pressure_Reading) is
    begin
        PR := Pressure_State.Pressure;
    end Read;

    procedure Write(HS : Heater_Setting) is
    begin
        Temp_State.SetHeater(HS);
        Put_Line("Writing Heater Setting: " & Heater_Setting'Image(HS));
    end Write;

    procedure Write(PS : Pressure_Setting) is
    begin
        Pressure_State.SetSetting(PS);
        Put_Line("Writing Pressure Setting: " & Pressure_Setting'Image(PS));
    end Write;

    procedure Write(TR : Temp_Reading) is
    begin
        Put_Line("Console: Read Temp: " & Temp_Reading'Image(TR));
    end Write;

    procedure Write(PR : Pressure_Reading) is
    begin
        Put_Line("Console: Read Pressure: " & Pressure_Reading'Image(PR));
    end Write;
end IO;
