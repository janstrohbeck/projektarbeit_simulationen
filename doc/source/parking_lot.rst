Parking Lot Regulation
======================

Overview
--------

.. Für diese Übung wird eine Steuerung für einen Parkplatz mithilfe mehrerer
.. Tasks/Threads programmiert.

For this exercise, a regulation for a parking lot is to be programmed using
multiple threads or tasks.

.. Der Parkplatz besteht aus einer Eingangs- und einer Ausgangsschranke sowie einem
.. Signal, welches anzeigt, ob der Parkplatz voll ist oder ob noch Parkplätze frei
.. sind. Die Schranken sowie das Signal können vom Hauptprogramm gesteuert werden.

The parking lot consists of an entry- and an exit gate as well as a signal
light, which shall display whether the parking lot is already full or if there
are still parking spots available. The provided simulation simulates that cars
want to enter and leave a parking lot. Each gate has its own queue of cars that
are currently waiting to be let in or out. The first car in the queue can
request to be served with a button.

The main program (which has to be provided by the students) then should read and
respond to these requests, open and close the gates and set the signal state.
Using a photo sensor, the main program can detect if a car is currently driving
through a gate. It should then keep the gate open until it has driven through.

An overview of this system can be seen in the following diagram:

.. image:: ../../uml/build/parking_lot_overview.*

The simulation creates a given number of cars as separate threads, which can
randomly enqueue themselves at the entry gate or exit gate (only if they have
entered successfully previously). They are also going to give up and leave the
queue if they have not been let in after 30 seconds.

The main program is probably also going to be divided into three separate threads or
tasks: One handling requests at each gate (EntryController, ExitController) and
one setting the signal state (SignalController).

The interface for interaction with the simulation library can be seen in the
following diagram:

.. image:: ../../uml/build/parking_lot_interface.*

The ``DataTypes`` module contains type definitions for values used in
the IO functions. The ``IO`` module contains the functions which can be used to
read the current state of the simulation and to set the state of the gates and
the signal. The functions are named differently in Ada and C. The names and
usage description can be found in the respective subsections (see below).

The simulation library also has to be included differently in each programming
language. Details can be found in the following sections :ref:`parking_lot_ada`
and :ref:`parking_lot_c`. The section :ref:`parking_lot_implementation` contains
further details about the implementation of the simulation.

.. _parking_lot_ada:

Ada
---

.. highlight:: ada

The simulation library provides the following modules and files:

 - ``Data_Types`` (data_types.ads)
 - ``IO`` (io.ads, io.adb)

These can be included using ``with``/``use`` statements::

   with Data_Types; use Data_Types;
   with IO; use IO;

Data types
++++++++++

The module ``Data_Types`` defines types which are used by the procedures which
can be used to read and manipulate the simulation state. The definition of these
data types allows specifying multiple ``Read``/``Write`` procedures which are
overloaded by the different data types. As they are all defined as new types,
they can not be accidentally converted into each other, which can help to
prevent errors. The provided types are:

.. ==================== =========================== ======================================================================
.. Data type            Definition                  Decription
.. ==================== =========================== ======================================================================
.. ``ParkingSpots``     ``new Integer range 0..10`` Can be used to count a number of parking spots (e.g. taken parking spots, available parking spots).
.. ``EntryGateState``   ``(Open, Closed)``          Used to specify the state of the entry gate (open or closed).
.. ``ExitGateState``    ``(Open, Closed)``          Used to specify the state of the exit gate (open or closed).
.. ``SignalState``      ``(Free, Full)``            Used to specify the state of the signal (free or full).
.. ``EntryRequest``     ``new Boolean``             Used to indicate if a car requests to be let in at the entry gate.
.. ``ExitRequest``      ``new Boolean``             Used to indicate if a car requests to be let out at the exit gate.
.. ``EntrySensorState`` ``(Free, Blocked)``         Used to indicate if a car is currently driving through the entry gate.
.. ``ExitSensorState``  ``(Free, Blocked)``         Used to indicate if a car is currently driving through the exit gate.
.. ==================== =========================== ======================================================================

.. ada:type:: type Data_Types.ParkingSpots is new Integer range 0..10;

   defined as::

      new Integer range 0..10

   Can be used to count a number of parking spots (e.g. taken parking spots, available parking spots).

.. ada:type:: type Data_Types.EntryGateState is (Open, Closed);

   defined as::

      (Open, Closed)

   Used to specify the state of the entry gate (open or closed).

.. ada:type:: type Data_Types.ExitGateState is (Open, Closed);

   defined as::

      (Open, Closed)

   Used to specify the state of the exit gate (open or closed).

.. ada:type:: type Data_Types.SignalState is (Free, Full);

   defined as::

      (Free, Full)

   Used to specify the state of the signal (free or full).

.. ada:type:: type Data_Types.EntryRequest is new Boolean;

   defined as::

      new Boolean

   Used to indicate if a car requests to be let in at the entry gate.

.. ada:type:: type Data_Types.ExitRequest is new Boolean;

   defined as::

      new Boolean

   Used to indicate if a car requests to be let out at the exit gate.

.. ada:type:: type Data_Types.EntrySensorState is new Boolean;

   defined as::

      (Free, Blocked)

   Used to indicate if a car is currently driving through the entry gate.

.. ada:type:: type Data_Types.ExitSensorState is new Boolean;

   defined as::

      (Free, Blocked)

   Used to indicate if a car is currently driving through the exit gate.

Reading the simulation state
++++++++++++++++++++++++++++

The current state of the simulation can be read via procedures, which are
provided by the IO package:

.. ada:procedure:: procedure IO.Read(ER: out Data_Types.EntryRequest);

   Reads if a request to enter was made by a car. The result will be stored in
   ``ER``.

.. ada:procedure:: procedure IO.Read(ER: out Data_Types.ExitRequest);

   Reads if a request to leave was made by a car. The result will be stored in
   ``ER``.

.. ada:procedure:: procedure IO.Read(ESS: out Data_Types.EntrySensorState);

   Reads if a car is currently driving through the entry gate. The result will
   be stored in ``ESS``.

.. ada:procedure:: procedure IO.Read(ESS: out Data_Types.ExitSensorState);

   Reads if a car is currently driving through the exit gate. The result will be
   stored in ``ESS``.

Manipulation of the simulation state
++++++++++++++++++++++++++++++++++++

The following procedures from the IO package can be used in order to regulate
the parking lot:

.. ada:procedure:: procedure IO.Write(E: Data_Types.EntryGateState);

   Sets the state of the entry gate to the value of ``E`` (opens/closes the gate).

.. ada:procedure:: procedure IO.Write(E: Data_Types.ExitGateState);

   Sets the state of the exit gate to the value of ``E`` (opens/closes the gate).

.. ada:procedure:: procedure IO.Write(S: Data_Types.SignalState);

   Sets the state of the signal to the value of ``S`` (free/full).

.. _parking_lot_c:

C
-

.. highlight:: c

The simulation library provides the following modules and files:

 - ``Data_Types`` (data_types.h)
 - ``IO`` (io.c, io.h)

These can be included using ``#include``-Statements::

   #include "data_types.h"
   #include "io.h"

Data types
++++++++++

The module ``Data_Types`` defines types which are used by the functions which
can be used to read and manipulate the simulation state. As C represents enum
values as integers and does not run further type-checks, defining all types from
the corresponding Ada module would not provide any benefits. Therefore, more
general types are defined:

.. ================== ======================= ===============================================================
.. Data type          Definition              Description 
.. ================== ======================= ===============================================================
.. ``bool``           ``enum {false, true}``  Allows usage of boolean values.
.. ``gate_state_t``   ``enum {CLOSE, OPEN}``  Used to specify the state of a gate (open or closed).
.. ``signal_state_t`` ``enum {FREE, FULL}``   Used to specify the state of the signal (free or full).
.. ================== ======================= ===============================================================

.. c:type:: bool

   defined as::

      enum {false, true}

   Allows usage of boolean values.

.. c:type:: gate_state_t

   defined as::

      enum {CLOSE, OPEN}

   Used to specify the state of a gate (open or closed).

.. c:type:: signal_state_t

   defined as::

      enum {FREE, FULL}

   Used to specify the state of the signal (free or full).

Initialization
++++++++++++++

In order to start the simulation, the following function has to be called inside
the ``main`` function:

.. c:function:: void start_simulator()

    Starts the simulation.

Reading the simulation state
++++++++++++++++++++++++++++

The current state of the simulation can be read via functions, which are
provided by the IO package:

.. c:function:: void read_entry_request(bool *ER)

   Reads if a request to enter was made by a car. The result will be stored in
   the variable pointed to by ``ER``.

.. c:function:: void read_exit_request(bool *ER)

   Reads if a request to enter was made by a car. The result will be stored in
   the variable pointed to by ``ER``.

.. c:function:: void read_entry_sensor_state(bool *ESS)

   Reads if a car is currently driving through the entry gate. The result will
   be stored in the variable pointed to by ``ESS``.

.. c:function:: void read_exit_sensor_state(bool *ESS)

   Reads if a car is currently driving through the exit gate. The result will
   be stored in the variable pointed to by ``ESS``.

Manipulation of the simulation state
++++++++++++++++++++++++++++++++++++

The following functions from the IO package can be used in order to regulate
the parking lot:

.. c:function:: void write_entry_gate_state(gate_state_t E)

   Sets the state of the entry gate to the value of ``E`` (opens/closes the gate).

.. c:function:: void write_exit_gate_state(gate_state_t E)

   Sets the state of the exit gate to the value of ``E`` (opens/closes the gate).

.. c:function:: void write_signal_state(signal_state_t S)

   Sets the state of the signal to the value of ``S`` (free/full).

.. _parking_lot_implementation:

Implementation details
----------------------

The core of the simulation takes part inside the IO package. Each car is
simulated by a separate thread. Also, two threads simulate the gates
(``EntryGateSimulator``, ``ExitGateSimulator``). Two queues are used to track
which cars are currently in the queues.

This can be seen in the following diagram:

.. image:: ../../uml/build/parking_lot_simulators.*

The current state of the simulation is contained inside the ``SimulatorState``
monitor. As by the definition of monitors, only one thread at a time can access
the attributes and methods of the monitor. This ensures that no race conditions
can occur, as multiple threads want to access this state object
(``EntryGateSimulator``, ``ExitGateSimulator``, user-defined threads using the
IO module). The ``SimulatorState monitor`` has methods to access and manipulate
its attributes.

Ada
+++

.. highlight:: ada

In Ada, the built-in construct of a ``protected`` object is used to implement
the ``SimulatorState`` monitor. The language runtime then ensures that only one
thread at a time can call the object's procedures. There also exist three
entries, which use guards, so that they can be used to block a calling thread
until a condition is met.

.. ada:type:: type IO.SimulatorState is ...;

   defined as::

       protected Simulator_State is
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
           procedure incNumEntryQueue;
           procedure decNumEntryQueue;
           function NumEntryQueue return Natural;
           procedure incNumExitQueue;
           procedure decNumExitQueue;
           function NumExitQueue return Natural;
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
           mNumEntryQueue : Natural := 0;
           mNumExitQueue : Natural := 0;
       end Simulator_State;

C
+

.. highlight:: c

.. c:type:: simulator_state_t

   defined as::

       static struct {
           unsigned int rand_seed;
           gate_state_t E;
           gate_state_t A;
           signal_state_t S;
           bool EA;
           bool AA;
           bool ED;
           bool AD;
           int car_cnt;
       } simulator_state;
       static pthread_mutex_t mutex;

   The attributes of this variable should not be accessed directly. There exist
   setter and getter methods for all attributes, for example::

       static entry_request_t simulator_state_get_entry_request() {
           entry_request_t ER;
           pthread_mutex_lock(&mutex);
           ER = simulator_state.entry_request;
           pthread_mutex_unlock(&mutex);
           return ER;
       }

       static void simulator_state_set_entry_request(entry_request_t entry_request) {
           pthread_mutex_lock(&mutex);
           simulator_state.entry_request = entry_request;
           pthread_mutex_unlock(&mutex);
       }

   These methods always have to lock the mutex in order to ensure that the state
   object is only accessed by one thread at a time.
