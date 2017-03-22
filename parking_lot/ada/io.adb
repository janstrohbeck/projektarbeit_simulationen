with Ada.Numerics.Discrete_Random;
with Ada.Real_Time; use Ada.Real_Time;
with Ada.Text_IO; use Ada.Text_IO;

package body IO is
    -- Data type for results of the random generator
    subtype RandomResult is Integer range 0 .. 99;
    -- Instantiation of the random generator package
    package RandomGenerator is new Ada.Numerics.Discrete_Random(RandomResult);
    use RandomGenerator;

    protected SimulatorState is
        -- Inititalizes the random generator
        procedure InitRandom;
        -- Gets the next random number from the random generator
        function GetRandom return RandomResult;

        -- Getters
        function GetEntryRequest return EntryRequest;
        function GetExitRequest return ExitRequest;
        function GetEntrySensorState return EntrySensorState;
        function GetExitSensorState return ExitSensorState;
        function GetEntryGateState return EntryGateState;
        function GetExitGateState return ExitGateState;
        function GetSignalState return SignalState;

        -- Setters
        procedure SetEntryGateState(E : EntryGateState);
        procedure SetExitGateState(E : ExitGateState);
        procedure SetSignalState(S : SignalState);
        procedure SetEntryRequest(ER : EntryRequest);
        procedure SetExitRequest(ER : ExitRequest);
        procedure SetEntrySensorState(ESS : EntrySensorState);
        procedure SetExitSensorState(ESS : ExitSensorState);

        -- Entries which can be used to wait for certain conditions
        entry WaitForEntryGateOpen;
        entry WaitForExitGateOpen;
        entry WaitForEntryGateClosed;
        entry WaitForExitGateClosed;

        -- Accessing, incrementing and decrementing the total number of cars in
        -- the parking lot
        procedure IncCarCnt;
        procedure DecCarCnt;
        function GetCarCnt return Natural;

        -- Accessing, incrementing and decrementing the number of cars in the
        -- entry queue
        procedure IncEntryQueueCnt;
        procedure DecEntryQueueCnt;
        function GetEntryQueueCnt return Natural;

        -- Accessing, incrementing and decrementing the ber of cars in the
        -- exit QueueCnt
        function GetExitQueueCnt return Natural;
        procedure IncExitQueueCnt;
        procedure DecExitQueueCnt;
    private 
        -- Random generator seed
        seed: Generator;
        entry_request : EntryRequest := False;
        exit_request : ExitRequest := False;
        entry_sensor_state : EntrySensorState := Free;
        exit_sensor_state : ExitSensorState := Free;
        entry_gate_state : EntryGateState := Closed;
        exit_gate_state : ExitGateState := Closed;
        signal_state : SignalState := Free;
        car_cnt : Natural := 0;
        entry_queue_cnt : Natural := 0;
        exit_queue_cnt : Natural := 0;
    end SimulatorState;

    protected body SimulatorState is
        -- Inititalizes the random generator
        procedure InitRandom is begin Reset(seed); end InitRandom;
        -- Gets the next random number from the random generator
        function GetRandom return RandomResult is (Random(seed));

        -- Getters
        function GetEntryRequest return EntryRequest is (entry_request);
        function GetExitRequest return ExitRequest is (exit_request);
        function GetEntrySensorState return EntrySensorState is (entry_sensor_state);
        function GetExitSensorState return ExitSensorState is (exit_sensor_state);
        function GetEntryGateState return EntryGateState is (entry_gate_state);
        function GetExitGateState return ExitGateState is (exit_gate_state);
        function GetSignalState return SignalState is (signal_state);

        -- Setters
        procedure SetEntryGateState(E : EntryGateState) is
        begin
            entry_gate_state := E;
            if E = Open and entry_request = True then
                entry_sensor_state := Blocked;
            end if;
        end SetEntryGateState;

        procedure SetExitGateState(E : ExitGateState) is
        begin
            exit_gate_state := E;
            if E = Open and exit_request = True then
                exit_sensor_state := Blocked;
            end if;
        end SetExitGateState;

        procedure SetSignalState(S : SignalState) is begin signal_state := S; end SetSignalState;
        procedure SetEntryRequest(ER : EntryRequest) is begin entry_request := ER; end SetEntryRequest;
        procedure SetExitRequest(ER : ExitRequest) is begin exit_request := ER; end SetExitRequest;
        procedure SetEntrySensorState(ESS : EntrySensorState) is begin entry_sensor_state := ESS; end SetEntrySensorState;
        procedure SetExitSensorState(ESS : ExitSensorState) is begin exit_sensor_state := ESS; end SetExitSensorState;

        -- Entries which can be used to wait for certain conditions
        entry WaitForEntryGateOpen when entry_gate_state = Open is begin null; end WaitForEntryGateOpen;
        entry WaitForExitGateOpen when exit_gate_state = Open is begin null; end WaitForExitGateOpen;
        entry WaitForEntryGateClosed when entry_gate_state = Closed is begin null; end WaitForEntryGateClosed;
        entry WaitForExitGateClosed when exit_gate_state = Closed is begin null; end WaitForExitGateClosed;

        -- Accessing, incrementing and decrementing the total number of cars in
        -- the parking lot
        procedure IncCarCnt is begin car_cnt := car_cnt + 1; end IncCarCnt;
        procedure DecCarCnt is begin car_cnt := car_cnt - 1; end DecCarCnt;
        function GetCarCnt return Natural is (car_cnt);

        -- Accessing, incrementing and decrementing the number of cars in the
        -- entry queue
        function GetEntryQueueCnt return Natural is (entry_queue_cnt);
        procedure IncEntryQueueCnt is begin entry_queue_cnt := entry_queue_cnt + 1; end IncEntryQueueCnt;
        procedure DecEntryQueueCnt is begin entry_queue_cnt := entry_queue_cnt - 1; end DecEntryQueueCnt;

        -- Accessing, incrementing and decrementing the ber of cars in the
        -- exit QueueCnt
        function GetExitQueueCnt return Natural is (exit_queue_cnt);
        procedure IncExitQueueCnt is begin exit_queue_cnt := exit_queue_cnt + 1; end IncExitQueueCnt;
        procedure DecExitQueueCnt is begin exit_queue_cnt := exit_queue_cnt - 1; end DecExitQueueCnt;
    end SimulatorState;

    task type Car(id : Integer);
    cars : array(1..20) of access Car;

    task EntryGateSimulator is 
        entry enter;
        entry entered;
    end EntryGateSimulator;

    task ExitGateSimulator is 
        entry leave;
        entry left;
    end ExitGateSimulator;

    task Logger;
    task body Logger is
        start : Boolean := True;

        prevSignalState : SignalState := Free;
        prevCarCnt : Natural := 0;
        prevEntryQueueCnt : Natural := 0;
        prevExitQueueCnt : Natural := 0;

        tmpSignalState : SignalState;
        tmpCarCnt : Natural;
        tmpEntryQueueCnt : Natural;
        tmpExitQueueCnt : Natural;
    begin
        loop
            tmpSignalState := SimulatorState.GetSignalState;
            tmpCarCnt := SimulatorState.GetCarCnt;
            tmpEntryQueueCnt := SimulatorState.GetEntryQueueCnt;
            tmpExitQueueCnt := SimulatorState.GetExitQueueCnt;

            if start or else (
                    tmpSignalState /= prevSignalState or 
                    tmpCarCnt /= prevCarCnt or 
                    tmpEntryQueueCnt /= prevEntryQueueCnt or 
                    tmpExitQueueCnt /= prevExitQueueCnt) then
                Put_Line("Cars inside:" & Natural'Image(tmpCarCnt)
                        & " | Signal: " & SignalState'Image(tmpSignalState)
                        & " | Cars waiting to enter: " & Natural'Image(tmpEntryQueueCnt)
                        & " | Cars waiting to leave: " & Natural'Image(tmpExitQueueCnt));
                start := False;
                prevSignalState := tmpSignalState;
                prevCarCnt := tmpCarCnt;
                prevEntryQueueCnt := tmpEntryQueueCnt;
                prevExitQueueCnt := tmpExitQueueCnt;
            end if;
          --delay 0.1;
        end loop;
    end Logger;

    task body EntryGateSimulator is
    begin
        loop
            accept enter do
                Put_Line("EntryGate: A car wants to enter!");
                SimulatorState.SetEntryRequest(True);
                Put_Line("EntryGate: Waiting for gate to open...");
                SimulatorState.WaitForEntryGateOpen;
            end enter;
            Put_Line("EntryGate: Gate open!");
            SimulatorState.SetEntryRequest(False);
            SimulatorState.IncCarCnt;

            Put_Line("EntryGate: Waiting for car to drive through...");
            accept entered do
                null;
            end entered;

            Put_Line("EntryGate: Car is through!");
            SimulatorState.SetEntrySensorState(Free);
            Put_Line("EntryGate: Waiting for gate to close...");
            SimulatorState.WaitForEntryGateClosed;
            Put_Line("EntryGate: Gate closed!");
        end loop;
    end EntryGateSimulator;

    task body ExitGateSimulator is
    begin
        loop
            accept leave do
                Put_Line("ExitGate: A car wants to leave!");
                SimulatorState.SetExitRequest(True);
                Put_Line("ExitGate: Waiting for gate to open...");
                SimulatorState.WaitForExitGateOpen;
            end leave;
            Put_Line("ExitGate: Gate open!");
            SimulatorState.SetExitRequest(False);
            SimulatorState.DecCarCnt;

            Put_Line("ExitGate: Waiting for car to drive through...");
            accept left do
                null;
            end left;

            Put_Line("ExitGate: Car is through!");
            SimulatorState.SetExitSensorState(Free);
            Put_Line("ExitGate: Waiting for gate to close...");
            SimulatorState.WaitForExitGateClosed;
            Put_Line("ExitGate: Gate closed!");
        end loop;
    end ExitGateSimulator;

    type Car_State is (Driving, Parked);

    task body Car is
        state : Car_State := Driving;
        success : Boolean;
    begin
        loop
            if state = Driving and SimulatorState.GetRandom < 10 then
                Put_Line("Car" & Integer'Image(id) & ": wanting to enter.");
                SimulatorState.IncEntryQueueCnt;
                success := False;
                select 
                    EntryGateSimulator.enter;
                    success := True;
                or 
                    delay 30.0;
                end select;
                SimulatorState.DecEntryQueueCnt;
                if success then
                    Put_Line("Car" & Integer'Image(id) & ": driving through gate!");
                  --delay 2.0;
                    Put_Line("Car" & Integer'Image(id) & ": went through gate.");
                    EntryGateSimulator.entered;
                    Put_Line("Car" & Integer'Image(id) & ": now parked.");
                    state := Parked;
                else
                    Put_Line("Car" & Integer'Image(id) & ": giving up.");
                end if;
            elsif state = Parked and SimulatorState.GetRandom < 3 then
                Put_Line("Car" & Integer'Image(id) & ": wanting to leave.");
                SimulatorState.IncExitQueueCnt;
                ExitGateSimulator.leave;
                SimulatorState.DecExitQueueCnt;
                Put_Line("Car" & Integer'Image(id) & ": driving through gate!");
              --delay 2.0;
                Put_Line("Car" & Integer'Image(id) & ": went through gate.");
                ExitGateSimulator.left;
                Put_Line("Car" & Integer'Image(id) & ": now outside.");
                state := Driving;
            end if;
          --delay 1.0;
        end loop;
    end Car;

    procedure Read(ER: out EntryRequest) is
    begin
        ER := SimulatorState.GetEntryRequest;
    end Read;

    procedure Read(ER: out ExitRequest) is
    begin
        ER := SimulatorState.GetExitRequest;
    end Read;

    procedure Read(ESS: out EntrySensorState) is
    begin
        ESS := SimulatorState.GetEntrySensorState;
    end Read;

    procedure Read(ESS: out ExitSensorState) is
    begin
        ESS := SimulatorState.GetExitSensorState;
    end Read;

    procedure Write(E: EntryGateState) is
    begin
        SimulatorState.SetEntryGateState(E);
    end Write;

    procedure Write(E: ExitGateState) is
    begin
        SimulatorState.SetExitGateState(E);
    end Write;

    procedure Write(S: SignalState) is
    begin
        SimulatorState.SetSignalState(S);
    end Write;
begin
    SimulatorState.InitRandom;
    for x in cars'Range loop
        cars(x) := new Car(x);
    end loop;
end IO;
