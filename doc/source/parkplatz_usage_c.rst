Parkplatz-Simulation in C
=========================

.. highlight:: c

Von der Bibliothek werden die folgenden Module bereitgestellt:

 - Data_Types (data_types.h)
 - IO (io.c, io.h)

Diese können über ``#include``-Anweisungen eingebunden werden::

   #include "data_types.h"
   #include "io.h"

Datentypen
----------

Im Modul Data_Types sind Typen definiert, welche von der Simulation verwendet
werden, um Zustände und Werte zu beschreiben. Im Detail sind dies:

================= ======================= ===============================================================
Datentyp          Definition              Beschreibung
================= ======================= ===============================================================
``bool``          ``enum {false, true}``  Ermöglicht die Verwendung eines bool'schen Datentyps
``gate_state_``   ``enum {CLOSE, OPEN}``  Gibt den Zustand einer Schranke an (geschlossen, offen)
``signal_state_`` ``enum {FREE, FULL}``   Gibt den Zustand des Signals an (Plätze frei, Parkhaus voll)
================= ======================= ===============================================================

Initialisierung
---------------

Um die Simulation zu starten, muss innerhalb der ``main``-Funktion folgende Funktion aufgerufen werden:

.. c:function:: void start_simulator()

   Startet die Simulation.

Auslesen des Zustandes
----------------------

Der Zustand des Parkplatzes kann über mehrere Funktionen, welche im IO-Modul
enthalten sind, ausgelesen werden:

.. c:function:: void read_einfahrt_anfrage(bool *EA)

   Liest aus, ob eine Anfrage zur Einfahrt von einem Auto vorliegt. Das
   Ergebnis wird in der Variable gespeichert, auf die EA zeigt.

.. c:function:: void read_ausfahrt_anfrage(bool *AA)

   Liest aus, ob eine Anfrage zur Ausfahrt von einem Auto vorliegt. Das
   Ergebnis wird in der Variable gespeichert, auf die AA zeigt.

.. c:function:: void read_einfahrt_durchfahrt(bool *ED)

   Liest aus, ob gerade ein Auto durch die Einfahrts-Schranke fährt. Das
   Ergebnis wird in der Variable gespeichert, auf die ED zeigt.

.. c:function:: void read_ausfahrt_durchfahrt(bool *AD)

   Liest aus, ob gerade ein Auto durch die Ausfahrts-Schranke fährt. Das
   Ergebnis wird in der Variable gespeichert, auf die AD zeigt.

Manipulation des Zustandes
--------------------------

Zur Steuerung können mehrere Funktionen des IO-Moduls verwendet werden:

.. c:function:: void write_einfahrt(gate_state_t E)

   Setzt den Zustand der Einfahrts-Schranke auf den Wert von E (öffnet/schließt die Schranke).

.. c:function:: void write_ausfahrt(gate_state_t A)

   Setzt den Zustand der Einfahrts-Schranke auf den Wert von A (öffnet/schließt die Schranke).

.. c:function:: void write_signal(signal_state_t S)

   Setzt den Zustand des Signals auf den Wert von S.

