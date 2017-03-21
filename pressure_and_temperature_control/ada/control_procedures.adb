package body Control_Procedures is
    procedure Temp_Convert(TR : Temp_Reading; HS : out Heater_Setting) is
    begin
        if TR < 20 then
            HS := On;
        else
            HS := Off;
        end if;
    end Temp_Convert;

    procedure Pressure_Convert(PR : Pressure_Reading; PS : out Pressure_Setting) is
        diff : Integer;
    begin
        diff := 1000 - Integer(PR);
        if diff > Integer(Pressure_Setting'Last) then
            PS := Pressure_Setting'Last;
        elsif diff <= Integer(Pressure_Setting'First) then
            PS := Pressure_Setting'First;
        else
            PS := Pressure_Setting(diff);
        end if;
    end Pressure_Convert;
end Control_Procedures;
