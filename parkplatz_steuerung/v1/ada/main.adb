with Data_Types; use Data_Types;
with MyIO; use MyIO;

procedure Main is
    EA : EAnfrage := False;
    AA : AAnfrage := False;
    ED : EDurchfahrt := False;
    AD : ADurchfahrt := False;
    PP : Parkplaetze := 0; -- Anzahl der belegten Parkplätze

    task TEinfahrt;
    task body TEinfahrt is
    begin
        loop
            Read(EA);
            if EA = True then -- Wenn Anfrage Einfahrt
                if PP<Parkplaetze'Last then -- Wenn Parkplatz nicht voll
                    Write(Einfahrt'(Open)); -- Einfahrt öffnen
                    ED := True; -- Durchfahrt beginnt
                    PP := PP+1; -- Ein Fahrzeug mehr auf dem Parkplatz
                    while (ED = True) loop Read(ED); delay 0.1; end loop; -- Durchfahren lassen
                    Write(Einfahrt'(Close)); -- Einfahrt schließen
                end if;
            end if;
            delay 0.1;
        end loop;
    end TEinfahrt;

    task TAusfahrt;
    task body TAusfahrt is
    begin
        loop
            Read(AA);
            if AA = True then -- Wenn Anfrage Ausfahrt
                Write(Ausfahrt'(Open)); -- Ausfahrt öffnen
                AD := True; -- Durchfahrt beginnt
                PP := PP-1; -- Ein Fahrzeug weniger auf dem Parkplatz
                while (AD = True) loop Read(AD); delay 0.1; end loop; -- Durchfahren lassen
                Write(Ausfahrt'(Close)); -- Ausfahrt schließen
            end if;
            delay 0.1;
        end loop;
    end TAusfahrt;

    task Signal;
    task body Signal is
    begin
        loop
            if PP < Parkplaetze'Last then
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
