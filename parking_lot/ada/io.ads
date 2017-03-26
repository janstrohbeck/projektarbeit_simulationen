with Data_Types; use Data_Types;

-- This package provided procedures for interaction with the parking lot simulation.
package IO is
    -- Reads if a request to enter was made by a car. The result will be stored in ER.
    procedure Read(ER: out EntryRequest);
    -- Reads if a request to leave was made by a car. The result will be stored in ER.
    procedure Read(ER: out ExitRequest);
    -- Reads if a car is currently driving through the entry gate. The result will be stored in ESS.
    procedure Read(ESS: out EntrySensorState);
    -- Reads if a car is currently driving through the exit gate. The result will be stored in ESS.
    procedure Read(ESS: out ExitSensorState);
    -- Sets the state of the entry gate to the value of E (opens/closes the gate).
    procedure Write(E: EntryGateState);
    -- Sets the state of the exit gate to the value of E (opens/closes the gate).
    procedure Write(E: ExitGateState);
    -- Sets the state of the signal to the value of S (free/full).
    procedure Write(S: SignalState);
private
    -- The logger prints the current number of cars in the parking lot, the
    -- signal state and the number of cars in the queues, whenever any of these
    -- values change.
    DISPLAY_LOGGER : constant Boolean := True;
    -- If set to True, this displays additional messages about the state of the
    -- gate simulators, e.g. which user events they are waiting for.
    DISPLAY_GATE_STATE : constant Boolean := False;
    -- If set to True, cars print which actions they are going to take
    DISPLAY_CAR_STATE : constant Boolean := False;
end IO;
