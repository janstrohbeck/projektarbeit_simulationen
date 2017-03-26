with Data_Types; use Data_Types;
with IO; use IO;
with Control_Procedures; use Control_Procedures;

procedure Main is
    task TempController;
    task PressureController;

    task body TempController is
        TR : TempReading; HS : HeaterSetting;
    begin
        loop
            Read(TR);
            Temp_Convert(TR,HS);
            Write(HS);
            Write(TR);
            delay 0.1;
        end loop;
    end TempController;

    task body PressureController is
        PR : PressureReading; PS : PressureSetting;
    begin
        loop
            Read(PR);
            Pressure_Convert(PR,PS);
            Write(PS);
            Write(PR);
            delay 0.1;
        end loop;
    end PressureController;
begin
    null;
    -- TempController and PressureController
    -- have started their executions
end Main;
