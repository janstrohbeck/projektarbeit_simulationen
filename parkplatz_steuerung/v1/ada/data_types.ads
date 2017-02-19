package Data_Types is
    type Parkplaetze is new Integer range 0..10;
    type Einfahrt is (Open, Close);
    type Ausfahrt is (Open, Close);
    type Signal is (Free, Full);
    type EAnfrage is new Boolean;
    type AAnfrage is new Boolean;
    type EDurchfahrt is new Boolean;
    type ADurchfahrt is new Boolean;
end Data_Types;
