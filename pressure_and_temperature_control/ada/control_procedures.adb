package body Control_Procedures is
    procedure Temp_Convert(TR : TempReading; HS : out HeaterSetting) is
    begin
        if TR < 20 then
            HS := On;
        else
            HS := Off;
        end if;
    end Temp_Convert;

    procedure Pressure_Convert(PR : PressureReading; PS : out PressureSetting) is
        diff : Integer;
    begin
        diff := 1000 - Integer(PR);
        if diff > Integer(PressureSetting'Last) then
            PS := PressureSetting'Last;
        elsif diff <= Integer(PressureSetting'First) then
            PS := PressureSetting'First;
        else
            PS := PressureSetting(diff);
        end if;
    end Pressure_Convert;
end Control_Procedures;
