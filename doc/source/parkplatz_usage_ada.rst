Parkplatz-Simulation in Ada
===========================

.. highlight:: ada

Von der Bibliothek werden die folgenden Module bereitgestellt:

 - Data_Types (data_types.ads)
 - MyIO (myio.ads, myio.adb)

Diese können über ``with``/``use`` Statements eingebunden werden::

   with Data_Types; use Data_Types;
   with MyIO; use MyIO;

Datentypen
----------

Im Modul Data_Types sind Typen definiert, welche von der Simulation verwendet
werden, um Zustände und Werte zu beschreiben. Im Detail sind dies:

=============== =========================== ===============================================================
Datentyp        Definition                  Beschreibung
=============== =========================== ===============================================================
``Parkplaetze`` ``new Integer range 0..10`` Gibt eine Anzahl an Parkplätzen an (z.B.  belegte Parkplätze)
``Einfahrt``    ``(Open, Close)``           Gibt den Zustand der Einfahrts-Schranke an (offen, geschlossen)
``Ausfahrt``    ``(Open, Close)``           Gibt den Zustand der Ausfahrts-Schranke an (offen, geschlossen)
``Signal``      ``(Free, Full)``            Gibt den Zustand des Signals an (Plätze frei, Parkhaus voll)
``EAnfrage``    ``new Boolean``             Gibt an, ob ein Auto den Parkplatz betreten will
``AAnfrage``    ``new Boolean``             Gibt an, ob ein Auto den Parkplatz verlassen will
``EDurchfahrt`` ``new Boolean``             Gibt an, ob gerade ein Auto durch die Einfahrts-Schranke fährt
``ADurchfahrt`` ``new Boolean``             Gibt an, ob gerade ein Auto durch die Ausfahrts-Schranke fährt
=============== =========================== ===============================================================

Auslesen des Zustandes
----------------------

Der Zustand des Parkplatzes kann über mehrere Prozeduren, welche im MyIO
Package enthalten sind, ausgelesen werden:

.. ada:procedure:: procedure Read(EA: out EAnfrage)

   Liest aus, ob eine Anfrage zur Einfahrt von einem Auto vorliegt. Das Ergebnis wird in EA gespeichert.

.. ada:procedure:: procedure Read(AA: out AAnfrage)

   Liest aus, ob eine Anfrage zur Ausfahrt von einem Auto vorliegt. Das Ergebnis wird in AA gespeichert.

.. ada:procedure:: procedure Read(ED: out EDurchfahrt)

   Liest aus, ob gerade ein Auto durch die Einfahrts-Schranke fährt. Das Ergebnis wird in ED gespeichert.

.. ada:procedure:: procedure Read(AD: out ADurchfahrt)

   Liest aus, ob gerade ein Auto durch die Ausfahrts-Schranke fährt. Das Ergebnis wird in AD gespeichert.

Manipulation des Zustandes
--------------------------

Zur Steuerung können mehrere Write-Prozeduren des MyIO Packages verwendet
werden:

.. ada:procedure:: procedure Write(E: Einfahrt)

   Setzt den Zustand der Einfahrts-Schranke auf den Wert von E (öffnet/schließt die Schranke).

.. ada:procedure:: procedure Write(A: Ausfahrt)

   Setzt den Zustand der Einfahrts-Schranke auf den Wert von A (öffnet/schließt die Schranke).

.. ada:procedure:: procedure Write(S: Signal)

   Setzt den Zustand des Signals auf den Wert von S.
