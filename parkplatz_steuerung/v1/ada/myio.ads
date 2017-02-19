with Data_Types; use Data_Types;
package MyIO is
    procedure Read(EA: out EAnfrage);
    procedure Read(AA: out AAnfrage);
    Procedure Read(ED: out EDurchfahrt);
    Procedure Read(AD: out ADurchfahrt);
    procedure Write(E: Einfahrt);
    procedure Write(A: Ausfahrt);
    procedure Write(S: Signal);
end MyIO;
