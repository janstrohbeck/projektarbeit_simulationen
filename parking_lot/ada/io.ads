with Data_Types; use Data_Types;
package IO is
    procedure Read(ER: out EntryRequest);
    procedure Read(ER: out ExitRequest);
    procedure Read(ESS: out EntrySensorState);
    procedure Read(ESS: out ExitSensorState);
    procedure Write(E: EntryGateState);
    procedure Write(E: ExitGateState);
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
