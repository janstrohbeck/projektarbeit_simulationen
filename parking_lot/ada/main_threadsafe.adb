with Data_Types; use Data_Types;
with IO; use IO;
with Ada.Text_IO; use Ada.Text_IO;

procedure Main is
    entry_request : EntryRequest := False;
    exit_request : ExitRequest := False;
    entry_sensor_state : EntrySensorState := Free;
    exit_sensor_state : ExitSensorState := Free;

    protected Wrapper is
        procedure IncSpots;
        procedure DecSpots;
        function Spots return ParkingSpots;
    private
        taken_spots : ParkingSpots := 0;
    end Wrapper;
    protected body Wrapper is
        procedure IncSpots is begin taken_spots := taken_spots + 1; end IncSpots;
        procedure DecSpots is begin taken_spots := taken_spots - 1; end DecSpots;
        function Spots return ParkingSpots is (taken_spots);
    end Wrapper;

    task EntryController;
    task body EntryController is
    begin
        loop
            Read(entry_request);
            if entry_request = True then -- Wenn Anfrage EntryGateState
                if Wrapper.Spots < ParkingSpots'Last then -- Wenn Parkplatz nicht voll
                    Wrapper.IncSpots;
                    Write(EntryGateState'(Open)); -- EntryGateState öffnen
                    entry_sensor_state := Blocked; -- Durchfahrt beginnt
                    while (entry_sensor_state = Blocked) loop Read(entry_sensor_state); delay 0.1; end loop; -- Durchfahren lassen
                    Write(EntryGateState'(Closed)); -- EntryGateState schließen
                end if;
            end if;
            delay 0.1;
        end loop;
    end EntryController;

    task ExitController;
    task body ExitController is
    begin
        loop
            Read(exit_request);
            if exit_request = True then -- Wenn Anfrage ExitGateState
                Wrapper.DecSpots;
                Write(ExitGateState'(Open)); -- ExitGateState öffnen
                exit_sensor_state := Blocked; -- Durchfahrt beginnt
                while (exit_sensor_state = Blocked) loop Read(exit_sensor_state); delay 0.1; end loop; -- Durchfahren lassen
                Write(ExitGateState'(Closed)); -- ExitGateState schließen
            end if;
            delay 0.1;
        end loop;
    end ExitController;

    task Signal;
    task body Signal is
    begin
        loop
            if Wrapper.Spots < ParkingSpots'Last then
                Write(Free);
            else
                Write(Full);
            end if;
        end loop;
    end Signal;
begin
    null;
end Main;
