package Data_Types is
    -- necessary type definitions
    type Temp_Reading is new Integer range 0..500;
    type Pressure_Reading is new Integer range 0..1200;
    type Heater_Setting is (On, Off);
    type Pressure_Setting is new Integer range -4..4;
end Data_Types;
