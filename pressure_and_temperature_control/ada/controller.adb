with Data_Types; use Data_Types;
with IO; use IO;
with Control_Procedures; use Control_Procedures;

procedure Controller is
    task Temp_Controller;
    task Pressure_Controller;

    task body Temp_Controller is
        TR : TempReading; HS : HeaterSetting;
    begin
        loop
            Read(TR);
            Temp_Convert(TR,HS);
            Write(HS);
            Write(TR);
            delay 0.1;
        end loop;
    end Temp_Controller;

    task body Pressure_Controller is
        PR : PressureReading; PS : PressureSetting;
    begin
        loop
            Read(PR);
            Pressure_Convert(PR,PS);
            Write(PS);
            Write(PR);
            delay 0.1;
        end loop;
    end Pressure_Controller;
begin
    null;
    -- Temp_Controller and Pressure_Controller
    -- have started their executions
end Controller;
