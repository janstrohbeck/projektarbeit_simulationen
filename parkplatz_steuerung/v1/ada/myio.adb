with Ada.Numerics.Discrete_Random;
with Ada.Real_Time; use Ada.Real_Time;
with Ada.Text_IO; use Ada.Text_IO;

package body MyIO is
    subtype RandomResult is Integer range 0 .. 99;
    package RandomGenerator is new Ada.Numerics.Discrete_Random(RandomResult);
    use RandomGenerator;

    protected type Simulator_State is
        procedure InitRandom;
        function GetRandom return RandomResult;
        function EA return EAnfrage;
        function AA return AAnfrage;
        function ED return EDurchfahrt;
        function AD return ADurchfahrt;
        procedure SetState(E : Einfahrt);
        procedure SetState(A : Ausfahrt);
        procedure SetState(S : Signal);
        function E return Einfahrt;
        function A return Ausfahrt;
        function S return Signal;
        procedure SetEA(EA : EAnfrage);
        procedure SetAA(AA : AAnfrage);
        procedure SetED(ED : EDurchfahrt);
        procedure SetAD(AD : ADurchfahrt);
        entry WaitForE;
        entry WaitForA;
        entry WaitForNoE;
        entry WaitForNoA;
        procedure incCarCnt;
        procedure decCarCnt;
        function CarCnt return Natural;
    private 
        Seed: Generator;
        mEA : EAnfrage := False;
        mAA : AAnfrage := False;
        mED : EDurchfahrt := False;
        mAD : ADurchfahrt := False;
        mE : Einfahrt := Close;
        mA : Ausfahrt := Close;
        mS : Signal := Free;
        mCarCnt : Natural := 0;
    end Simulator_State;

    protected body Simulator_State is
        procedure InitRandom is begin Reset(Seed); end InitRandom;
        function GetRandom return RandomResult is (Random(Seed));
        function EA return EAnfrage is (mEA);
        function AA return AAnfrage is (mAA);
        function ED return EDurchfahrt is (mED);
        function AD return ADurchfahrt is (mAD);
        procedure SetState(E : Einfahrt) is begin mE := E; end SetState;
        procedure SetState(A : Ausfahrt) is begin mA := A; end SetState;
        procedure SetState(S : Signal) is begin mS := S; end SetState;
        function E return Einfahrt is (mE);
        function A return Ausfahrt is (mA);
        function S return Signal is (mS);
        procedure SetEA(EA : EAnfrage) is begin mEA := EA; end SetEA;
        procedure SetAA(AA : AAnfrage) is begin mAA := AA; end SetAA;
        procedure SetED(ED : EDurchfahrt) is begin mED := ED; end SetED;
        procedure SetAD(AD : ADurchfahrt) is begin mAD := AD; end SetAD;
        entry WaitForE when mE = Open is begin null; end WaitForE;
        entry WaitForA when mA = Open is begin null; end WaitForA;
        entry WaitForNoE when mE = Close is begin null; end WaitForNoE;
        entry WaitForNoA when mA = Close is begin null; end WaitForNoA;
        procedure incCarCnt is begin mCarCnt := mCarCnt + 1; end;
        procedure decCarCnt is begin mCarCnt := mCarCnt - 1; end;
        function CarCnt return Natural is (mCarCnt);
    end Simulator_State;

    sim_state : Simulator_State;

    task type Car(id : Integer);
    cars : array(1..20) of access Car;

    task ESimulator is 
        entry enter;
        entry entered;
    end ESimulator;

    task ASimulator is 
        entry leave;
        entry left;
    end ASimulator;

    task Simulator;
    task body Simulator is
    begin
        loop
            Put_Line("Cars inside:" & Natural'Image(sim_state.CarCnt) & " | " & "Signal: " & Signal'Image(sim_state.S));
            delay 0.1;
        end loop;
    end Simulator;

    task body ESimulator is
    begin
        loop
            accept enter do
                Put_Line("E: A car wants to enter!");
                sim_state.SetEA(True);
                Put_Line("E: Waiting for gate to open...");
                sim_state.WaitForE;
            end enter;
            Put_Line("E: Gate open!");
            sim_state.SetEA(False);

            Put_Line("E: Waiting for car to drive through...");
            accept entered do
                null;
            end entered;

            Put_Line("E: Car is through!");
            sim_state.SetED(False);
            sim_state.incCarCnt;
            Put_Line("E: Waiting for gate to close...");
            sim_state.WaitForNoE;
            Put_Line("E: Gate closed!");
        end loop;
    end ESimulator;

    task body ASimulator is
    begin
        loop
            accept leave do
                Put_Line("A: A car wants to leave!");
                sim_state.SetAA(True);
                Put_Line("A: Waiting for gate to open...");
                sim_state.WaitForA;
            end leave;
            Put_Line("A: Gate open!");
            sim_state.SetAA(False);

            Put_Line("A: Waiting for car to drive through...");
            accept left do
                null;
            end left;

            Put_Line("A: Car is through!");
            sim_state.SetAD(False);
            sim_state.decCarCnt;
            Put_Line("A: Waiting for gate to close...");
            sim_state.WaitForNoA;
            Put_Line("A: Gate closed!");
        end loop;
    end ASimulator;

    type Car_State is (Driving, Parked);

    task body Car is
        state : Car_State := Driving;
        success : Boolean;
    begin
        loop
            if state = Driving and sim_state.GetRandom < 10 then
                Put_Line("Car" & Integer'Image(id) & ": wanting to enter.");
                success := False;
                select 
                    ESimulator.enter;
                    success := True;
                or 
                    delay 30.0;
                end select;
                if success then
                    Put_Line("Car" & Integer'Image(id) & ": driving through gate!");
                    delay 2.0;
                    Put_Line("Car" & Integer'Image(id) & ": went through gate.");
                    ESimulator.entered;
                    Put_Line("Car" & Integer'Image(id) & ": now parked.");
                    state := Parked;
                else
                    Put_Line("Car" & Integer'Image(id) & ": giving up.");
                end if;
            elsif state = Parked and sim_state.GetRandom < 3 then
                Put_Line("Car" & Integer'Image(id) & ": wanting to leave.");
                ASimulator.leave;
                Put_Line("Car" & Integer'Image(id) & ": driving through gate!");
                delay 2.0;
                Put_Line("Car" & Integer'Image(id) & ": went through gate.");
                ASimulator.left;
                Put_Line("Car" & Integer'Image(id) & ": now outside.");
                state := Driving;
            end if;
            delay 1.0;
        end loop;
    end Car;

    procedure Read(EA: out EAnfrage) is
    begin
        EA := sim_state.EA;
    end Read;

    procedure Read(AA: out AAnfrage) is
    begin
        AA := sim_state.AA;
    end Read;

    procedure Read(ED: out EDurchfahrt) is
    begin
        ED := sim_state.ED;
    end Read;

    procedure Read(AD: out ADurchfahrt) is
    begin
        AD := sim_state.AD;
    end Read;

    procedure Write(E: Einfahrt) is
    begin
        sim_state.SetState(E);
        if E = Open and sim_state.EA = True then
            sim_state.SetED(True);
        end if;
    end Write;

    procedure Write(A: Ausfahrt) is
    begin
        sim_state.SetState(A);
        if A = Open and sim_state.AA = True then
            sim_state.SetAD(True);
        end if;
    end Write;

    procedure Write(S: Signal) is
    begin
        sim_state.SetState(S);
    end Write;
begin
    sim_state.InitRandom;
    for x in cars'Range loop
        cars(x) := new Car(x);
    end loop;
end MyIO;
