package Data_Types is
    -- necessary type definitions
    type TempReading is new Integer range 0..500;
    type PressureReading is new Integer range 0..1200;
    type HeaterSetting is (On, Off);
    type PressureSetting is new Integer range -4..4;
end Data_Types;
