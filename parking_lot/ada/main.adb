with Data_Types; use Data_Types;
with IO; use IO;
with Ada.Text_IO; use Ada.Text_IO;

procedure Main is
    entry_request : EntryRequest := False;
    exit_request : ExitRequest := False;
    entry_sensor_state : EntrySensorState := Free;
    exit_sensor_state : ExitSensorState := Free;
    taken_parking_spots : ParkingSpots := 0;

    task EntryController;
    task body EntryController is
    begin
        loop
            Read(entry_request);
            if entry_request = True then -- Wenn Anfrage EntryGateState
                if taken_parking_spots < ParkingSpots'Last then -- Wenn Parkplatz nicht voll
                    taken_parking_spots := taken_parking_spots + 1;
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
                taken_parking_spots := taken_parking_spots - 1;
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
            if taken_parking_spots < ParkingSpots'Last then
                Write(Free);
            else
                Write(Full);
            end if;
            delay 0.1;
        end loop;
    end Signal;
begin
    null;
end Main;
