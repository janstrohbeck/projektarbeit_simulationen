package Data_Types is
    type ParkingSpots is new Integer range 0..10;
    type EntryGateState is (Open, Closed);
    type ExitGateState is (Open, Closed);
    type SignalState is (Free, Full);
    type EntryRequest is new Boolean;
    type ExitRequest is new Boolean;
    type EntrySensorState is (Free, Blocked);
    type ExitSensorState is (Free, Blocked);
end Data_Types;
