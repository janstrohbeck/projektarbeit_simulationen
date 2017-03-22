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
    DISPLAY_LOGGER : constant Boolean := True;
    DISPLAY_GATE_STATE : constant Boolean := False;
    DISPLAY_CAR_STATE : constant Boolean := False;
end IO;
