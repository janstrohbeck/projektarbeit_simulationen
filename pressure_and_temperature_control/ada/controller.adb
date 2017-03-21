with Data_Types; use Data_Types;
with IO; use IO;
with Control_Procedures; use Control_Procedures;

procedure Controller is
    task Temp_Controller;
    task Pressure_Controller;

    task body Temp_Controller is
        TR : Temp_Reading; HS : Heater_Setting;
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
        PR : Pressure_Reading; PS : Pressure_Setting;
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
