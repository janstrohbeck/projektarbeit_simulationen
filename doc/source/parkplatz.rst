Parkplatz-Steuerung
===================

Allgemeines
-----------

Für diese Übung wird eine Steuerung für einen Parkplatz mithilfe mehrerer
Tasks/Threads programmiert.

Der Parkplatz besteht aus einer Eingangs- und einer Ausgangsschranke sowie einem
Signal, welches anzeigt, ob der Parkplatz voll ist oder ob noch Parkplätze frei
sind. Die Schranken sowie das Signal können vom Hauptprogramm gesteuert werden.

Von einer bereitgestellten Simulation wird simuliert, dass Autos den Parkplatz
betreten oder verlassen möchten. Das Hauptprogramm muss diese Anfragen dann
auslesen und beantworten.

Wenn eine Anfrage zur Einfahrt oder Ausfahrt vorliegt, muss vom Hauptprogramm
die entsprechende Schranke geöffnet werden, gewartet werden bis das Auto
hindurchgefahren ist, und anschließend die Schranke wieder geschlossen werden.

Hierzu bieten die Simulationsbibliotheken Methoden, um auszulesen, ob Anfragen
vorliegen und ob gerade noch ein Auto unter einer Schranke hindurchfährt sowie
um Schranken zu öffnen und das Signal zu schalten.

Die Simulation erzeugt eine bestimmte Anzahl von simulierten Autos jeweils als
separate Threads/Tasks, welche zufällig Anfragen stellen können. Außerdem werden
für die Autos, welche das Parkhaus betreten möchten, Warteschlangen verwaltet,
in welche sich die Autos einreihen müssen, falls diese den Parkplatz betreten
oder verlassen möchten. Die entsprechenden Tasks/Threads werden solange
blockiert, bis sie ein- oder ausgelassen werden. Falls sie jedoch nicht
innerhalb von 30 Sekunden eingelassen werden, geben sie auf und verlassen die
Warteschlange wieder.

Das Einbinden der Bibliotheken und deren Verwendung ist je nach
Programmiersprache unterschiedlich. Details sind in den folgenden beiden
Unterkapiteln beschrieben.

Ada
---

:any:`parkplatz_usage_ada`

C
-

:any:`parkplatz_usage_c`

Details zur Implementierung
---------------------------

:any:`parkplatz_implementation`
