with Data_Types; use Data_Types;
with IO; use IO;

procedure Main is
    taken_parking_spots : ParkingSpots := 0;

    task EntryController;
    task body EntryController is
        entry_request : EntryRequest := False;
    begin
        loop
            Read(entry_request);
            -- If a car wants to enter
            if entry_request = True then
                -- Check if parking lot is not already full
                if taken_parking_spots < ParkingSpots'Last then
                    taken_parking_spots := taken_parking_spots + 1;

                    -- Open gate
                    Write(EntryGateState'(Open));
                    declare
                        entry_sensor_state : EntrySensorState := Blocked;
                    begin
                        -- Wait until the sensor is not blocked any more (car must be through then)
                        while (entry_sensor_state = Blocked) loop Read(entry_sensor_state); delay 0.1; end loop;
                    end;
                    -- Close gate
                    Write(EntryGateState'(Closed));
                end if;
            end if;
            delay 0.1;
        end loop;
    end EntryController;

    task ExitController;
    task body ExitController is
        exit_request : ExitRequest := False;
    begin
        loop
            Read(exit_request);
            -- If a car wants to leave
            if exit_request = True then
                taken_parking_spots := taken_parking_spots - 1;

                -- Open gate
                Write(ExitGateState'(Open));
                declare
                    exit_sensor_state : ExitSensorState := Free;
                begin
                    -- Wait until the sensor is not blocked any more (car must be through then)
                    while (exit_sensor_state = Blocked) loop Read(exit_sensor_state); delay 0.1; end loop;
                end;
                -- Close gate
                Write(ExitGateState'(Closed));
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
