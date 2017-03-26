-- This package contains data types which are used in the rest of the project
package Data_Types is
    -- Can be used to count a number of parking spots (e.g. taken parking spots, available parking spots).
    type ParkingSpots is new Integer range 0..10;
    -- Used to specify the state of the entry gate (open or closed).
    type EntryGateState is (Open, Closed);
    -- Used to specify the state of the exit gate (open or closed).
    type ExitGateState is (Open, Closed);
    -- Used to specify the state of the signal (free or full).
    type SignalState is (Free, Full);
    -- Used to indicate if a car requests to be let in at the entry gate.
    type EntryRequest is new Boolean;
    -- Used to indicate if a car requests to be let out at the exit gate.
    type ExitRequest is new Boolean;
    -- Used to indicate if a car is currently driving through the entry gate.
    type EntrySensorState is (Free, Blocked);
    -- Used to indicate if a car is currently driving through the exit gate.
    type ExitSensorState is (Free, Blocked);
end Data_Types;
