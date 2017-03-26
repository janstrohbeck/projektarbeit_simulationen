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
        function GetCarCnt return Natural;
        function GetEntryQueueCnt return Natural;
        function GetExitQueueCnt return Natural;

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

        -- Incrementing and decrementing the total number of cars in
        -- the parking lot
        procedure IncCarCnt;
        procedure DecCarCnt;

        -- Incrementing and decrementing the number of cars in the
        -- entry queue
        procedure IncEntryQueueCnt;
        procedure DecEntryQueueCnt;

        -- Incrementing and decrementing the ber of cars in the
        -- exit QueueCnt
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
            -- Set entry sensor to blocked immediately after the gate is opened.
            -- This allows the main program to block immediately after opening the
            -- gate, waiting for the sensor to be free again.
            if E = Open and entry_request = True then
                entry_sensor_state := Blocked;
            end if;
        end SetEntryGateState;

        procedure SetExitGateState(E : ExitGateState) is
        begin
            exit_gate_state := E;
            -- Set exit sensor to blocked immediately after the gate is opened.
            -- This allows the main program to block immediately after opening the
            -- gate, waiting for the sensor to be free again.
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

    -- Declare Car task. Each car has a unique ID.
    task type Car(id : Integer);
    -- We will instantiate 20 cars in the package body.
    cars : array(1..20) of access Car;

    -- The task simulating the entry gate.
    task EntryGateSimulator is 
        -- Cars can call this entry when they want to enter the parking lot.
        -- They will be queued and blocked until this thread lets them in.
        entry enter;
        -- After the 'enter' entry returns, cars must call this entry in order
        -- to signal that they have finished entering the parking lot through
        -- the gate.
        entry entered;
    end EntryGateSimulator;

    -- The task simulating the exit gate.
    task ExitGateSimulator is 
        -- Cars can call this entry when they want to leave the parking lot.
        -- They will be queued and blocked until this thread lets them out.
        entry leave;
        -- After the 'leave' entry returns, cars must call this entry in order
        -- to signal that they have finished entering the parking lot through
        -- the gate.
        entry left;
    end ExitGateSimulator;

    -- A task that prints the current state of the simulation when the state changes.
    task Logger;
    task body Logger is
        -- Flag that is used to print the state on first start of the task.
        start : Boolean := True;

        -- Variables to save previous values of the state members in order to
        -- check if something changed.
        prevSignalState : SignalState := Free;
        prevCarCnt : Natural := 0;
        prevEntryQueueCnt : Natural := 0;
        prevExitQueueCnt : Natural := 0;

    begin
        if DISPLAY_LOGGER then
            loop
                declare 
                    -- The current values of the state members
                    tmpSignalState : SignalState := SimulatorState.GetSignalState;
                    tmpCarCnt : Natural := SimulatorState.GetCarCnt;
                    tmpEntryQueueCnt : Natural := SimulatorState.GetEntryQueueCnt;
                    tmpExitQueueCnt : Natural := SimulatorState.GetExitQueueCnt;
                begin
                    -- Check if something changed
                    if start or else (
                            tmpSignalState /= prevSignalState or 
                            tmpCarCnt /= prevCarCnt or 
                            tmpEntryQueueCnt /= prevEntryQueueCnt or 
                            tmpExitQueueCnt /= prevExitQueueCnt) then
                        -- print current state
                        Put_Line("Cars inside:" & Natural'Image(tmpCarCnt)
                                & " | Signal: " & SignalState'Image(tmpSignalState)
                                & " | Cars waiting to enter: " & Natural'Image(tmpEntryQueueCnt)
                                & " | Cars waiting to leave: " & Natural'Image(tmpExitQueueCnt));
                        start := False;
                        -- save current state values for comparison later
                        prevSignalState := tmpSignalState;
                        prevCarCnt := tmpCarCnt;
                        prevEntryQueueCnt := tmpEntryQueueCnt;
                        prevExitQueueCnt := tmpExitQueueCnt;
                    end if;
                end;
                delay 0.1;
            end loop;
        end if;
    end Logger;

    task body EntryGateSimulator is
        -- Log only if it is configured, prepend everything with "EntryGate: "
        procedure Log(message: String) is
        begin
            if DISPLAY_GATE_STATE then
                Put_Line("EntryGate: " & message);
            end if;
        end Log;
    begin
        loop
            -- Wait for a car to call enter
            accept enter do
                Log("A car wants to enter!");
                SimulatorState.SetEntryRequest(True);
                Log("Waiting for gate to open...");
                SimulatorState.WaitForEntryGateOpen;
            end enter;
            Log("Gate open!");
            SimulatorState.SetEntryRequest(False);
            SimulatorState.IncCarCnt;

            Log("Waiting for car to drive through...");
            -- Wait for car to call entered, signaling it has finished entering
            accept entered do
                null;
            end entered;

            Log("Car is through!");
            SimulatorState.SetEntrySensorState(Free);
            Log("Waiting for gate to close...");
            SimulatorState.WaitForEntryGateClosed;
            Log("Gate closed!");
        end loop;
    end EntryGateSimulator;

    task body ExitGateSimulator is
        -- Log only if it is configured, prepend everything with "ExitGate: "
        procedure Log(message: String) is
        begin
            if DISPLAY_GATE_STATE then
                Put_Line("ExitGate: " & message);
            end if;
        end Log;
    begin
        loop
            -- Wait for a car to call leave
            accept leave do
                Log("A car wants to leave!");
                SimulatorState.SetExitRequest(True);
                Log("Waiting for gate to open...");
                SimulatorState.WaitForExitGateOpen;
            end leave;
            Log("Gate open!");
            SimulatorState.SetExitRequest(False);
            SimulatorState.DecCarCnt;

            Log("Waiting for car to drive through...");
            -- Wait for car to call left, signaling it has finished leaving
            accept left do
                null;
            end left;

            Log("Car is through!");
            SimulatorState.SetExitSensorState(Free);
            Log("Waiting for gate to close...");
            SimulatorState.WaitForExitGateClosed;
            Log("Gate closed!");
        end loop;
    end ExitGateSimulator;

    type CarState is (Driving, Parked);

    task body Car is
        state : CarState := Driving;
        success : Boolean;

        -- Log only if it is configured, prepend everything with "Car [id]: "
        procedure Log(message: String) is
        begin
            if DISPLAY_CAR_STATE then
                Put_Line("Car" & Integer'Image(id) & ": " & message);
            end if;
        end Log;
    begin
        loop
            -- With 10% probability, try to enter the parking lot
            if state = Driving and SimulatorState.GetRandom < 10 then
                Log("wanting to enter.");
                SimulatorState.IncEntryQueueCnt;
                success := False;
                -- Try to enter, but abort if the simulator task did not accept
                -- it in 30 seconds
                select 
                    EntryGateSimulator.enter;
                    success := True;
                or 
                    delay 30.0;
                end select;
                SimulatorState.DecEntryQueueCnt;

                if success then
                    Log("driving through gate!");
                    delay 2.0;
                    Log("went through gate.");
                    EntryGateSimulator.entered;
                    Log("now parked.");
                    state := Parked;
                else
                    Log("giving up.");
                end if;
            -- With 3% probability, try to leave the parking lot
            elsif state = Parked and SimulatorState.GetRandom < 3 then
                Log("wanting to leave.");
                SimulatorState.IncExitQueueCnt;
                -- We don't abort after 30s in this case
                ExitGateSimulator.leave;
                SimulatorState.DecExitQueueCnt;
                Log("driving through gate!");
                delay 2.0;
                Log("went through gate.");
                ExitGateSimulator.left;
                Log("now outside.");
                state := Driving;
            end if;
            delay 1.0;
        end loop;
    end Car;

    -- The following procedures just read from the state

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
    -- Create the cars
    for x in cars'Range loop
        cars(x) := new Car(x);
    end loop;
end IO;
