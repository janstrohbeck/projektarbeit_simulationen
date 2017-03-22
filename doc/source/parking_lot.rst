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
tasks: One handling requests at each gate (``EntryController``,
``ExitController``) and one setting the signal state (``SignalController``).

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

The Simulation also includes some output, which allows the user to track the
state of the simulation. This can especially be useful in case the user's
program contains errors, as the output may show which actions the simulation
might be waiting for. The output can be customized by global constants in both
implementations (see following sections).



.. _parking_lot_ada:

Ada
---

.. highlight:: ada

The simulation library provides the following modules and files:

 - ``Data_Types`` (``data_types.ads``)
 - ``IO`` (``io.ads``, ``io.adb``)

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

Log output customization
++++++++++++++++++++++++

The simulation library allows customization of which output is printed. The
configuration can be performed in ``io.ads``::

    -- The logger prints the current number of cars in the parking lot, the
    -- signal state and the number of cars in the queues, whenever any of these
    -- values change.
    DISPLAY_LOGGER : constant Boolean := True;
    -- If set to True, this displays additional messages about the state of the
    -- gate simulators, e.g. which user events they are waiting for.
    DISPLAY_GATE_STATE : constant Boolean := False;
    -- If set to True, cars print which actions they are going to take
    DISPLAY_CAR_STATE : constant Boolean := False;



.. _parking_lot_c:

C
-

.. highlight:: c

The simulation library provides the following modules and files:

 - ``Data_Types`` (``data_types.h``)
 - ``IO`` (``io.c``, ``io.h``)
 - ``RequestQueue`` (``request_queue.c``, ``request_queue.h``)

These can be included using ``#include``-statements::

   #include "data_types.h"
   #include "io.h"

The RequestQueue is only used internally by the simulation and does not need to
be included (see :ref:`parking_lot_implementation`).

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

      enum {GATE_CLOSED, GATE_OPEN}

   Used to specify the state of a gate (open or closed).

.. c:type:: sensor_state_t

   defined as::

      enum {SENSOR_FREE, SENSOR_BLOCKED}

   Used to specify the state of a gate sensor (free or blocked [by a car]).

.. c:type:: signal_state_t

   defined as::

      enum {SIGNAL_FREE, SIGNAL_FULL}

   Used to specify the state of the signal (free or full).

Initialization
++++++++++++++

In order to start the simulation, the following function has to be called inside
the ``main`` function:

.. c:function:: void init_simulator()

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

Log output customization
++++++++++++++++++++++++

The log output can be customized, just like with the Ada version. Users can
alter the following defines in ``io.h``::

    // The logger prints the current number of cars in the parking lot, the
    // signal state and the number of cars in the queues, whenever any of these
    // values change.
    #define DISPLAY_LOGGER 1
    // If set to 1, this displays additional messages about the state of the
    // gate simulators, e.g. which user events they are waiting for.
    #define DISPLAY_GATE_STATE 0
    // If set to 1, cars print which actions they are going to take
    #define DISPLAY_CAR_STATE 0

Synchronous console output
++++++++++++++++++++++++++

The simulation library internally uses a synchronized ``printf`` function, so
that the console output of multiple threads does not get intermingled. It
internally uses a mutex to allow access to this function only for one thread at
a time. The function can also be used in user programs if the ``io.h`` header is
included.

.. c:function:: int sync_printf(const char *format, ...)

   A synchronized version of ``printf``.



.. _parking_lot_implementation:

Implementation details
----------------------

The core of the simulation takes part inside the IO package. Each car is
simulated by a separate thread. Also, two threads simulate the gates
(``EntryGateSimulator``, ``ExitGateSimulator``). Two queues are used to track
which cars are currently in the queues. There also exists a ``Logger`` thread,
which prints the current number of cars in the parking lot as well as the signal
state and how many cars are currently in the queues.

This can be seen in the following diagram:

.. image:: ../../uml/build/parking_lot_simulators.*

The current state of the simulation is contained inside the ``SimulatorState``
monitor. As by the definition of monitors, only one thread at a time can access
the attributes and methods of the monitor. This ensures that no race conditions
can occur, as multiple threads want to access this state object
(``EntryGateSimulator``, ``ExitGateSimulator``, ``Cars`` and user-defined
threads using the IO module). The ``SimulatorState`` monitor has several
attributes, for which accessor and manipulation methods exist:

========================= ================================================================== ====================================================================================
Attribute                 Data type (Ada/C)                                                  Description 
========================= ================================================================== ====================================================================================
``random_seed``           ``Random`` / ``unsigned int``                                      Contains a number specifying the current state of the pseudo-random number generator
``entry_request``         ``Boolean`` / :c:type:`bool`                                       States whether there currently is a request from a car to enter the parking lot.
``exit_request``          ``Boolean`` / :c:type:`bool`                                       States whether there currently is a request from a car to leave the parking lot.
``entry_sensor_state``    :ada:type:`Data_Types.EntrySensorState` / :c:type:`sensor_state_t` Saves the current state of the entry gate sensor.
``exit_sensor_state``     :ada:type:`Data_Types.ExitSensorState` / :c:type:`sensor_state_t`  Saves the current state of the exit gate sensor.
``entry_gate_state``      :ada:type:`Data_Types.EntryGateState` / :c:type:`gate_state_t`     Saves the current state of the entry gate.
``exit_gate_state``       :ada:type:`Data_Types.ExitGateState` / :c:type:`gate_state_t`      Saves the current state of the exit gate.
``signal_state``          :ada:type:`Data_Types.SignalState` / :c:type:`signal_state_t`      Saves the current state of the signal.
``car_cnt``               ``Natural`` / ``int``                                              Saves the current number of cars which are in the parking lot.
``entry_queue_cnt``       ``Natural`` / ``int``                                              Saves the current number of cars which are enqueued in the entry queue.
``exit_queue_cnt``        ``Natural`` / ``int``                                              Saves the current number of cars which are enqueued in the exit queue.
========================= ================================================================== ====================================================================================

The access and manipulation methods include simple getter and setters, as well
as methods to increment or decrement some values and wait methods. These wait
methods block the calling thread until a condition is met, e.g.\ that the entry
gate is open or closed.

The RequestQueue allows cars to enqueue themselves into a queue of waiting cars.
The gate simulator threads can call the ``receive``-method, which blocks them
until a car is in the queue. This is shown in the following sequence diagram:

.. image:: ../../uml/build/parking_lot_requestqueue.*
   :scale: 60%
   :align: center

In the C implementation, the cars are resumed as soon as the gate simulator
thread has received their request, but then block again until they have been
processed by the gate simulator. The Ada implementation uses native rendezvous
between tasks, which always block until the request has been processed. For
details, see the following two sections.

Cars may also specify a timeout duration, after which they are automatically
removed from the queue (here: 30 seconds):

.. image:: ../../uml/build/parking_lot_requestqueue_timeout.*
   :scale: 60%
   :align: center

This can happen if the parking lot is full or if the queue is so large, that the
processing of the cars in the queue takes more than 30 seconds. As a car needs
two seconds to drive through a gate, the queue has to have 15 cars in it so that
any additional car will not be processed in time.

Ada
+++

.. highlight:: ada

Simulator State
^^^^^^^^^^^^^^^

In Ada, the built-in construct of a ``protected`` object is used to implement
the ``SimulatorState`` monitor. The language runtime then ensures that only one
thread at a time can call the object's procedures. The wait methods are
implemented as entries with guard that specify the condition. These entries
block the current thread until the guard condition is met.

.. ada:type:: type IO.SimulatorState is ...;

   defined as::

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
            -- exit queue
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

Threads, Request Queues
^^^^^^^^^^^^^^^^^^^^^^^

The cars, simulator and logger threads are implemented as Ada tasks::

    task EntryGateSimulator is 
        entry enter;
        entry entered;
    end EntryGateSimulator;

    task ExitGateSimulator is 
        entry leave;
        entry left;
    end ExitGateSimulator;

    task type Car(id : Integer);
    cars : array(1..20) of access Car;

The entries of the simulator tasks are used by the ``Car`` tasks, in order to
signal the simulators when they want to enter or leave the parking lot, and when
they have finished entering or leaving (i.e. finished driving through the gate).
As Ada tasks already maintain a queue for each entry, no further queue needs to
be implemented. The tasks which call the entries are queued and processed in
first-in-first-out-order. They are resumed after the call has been processed by
the called task (equivalent to a remote procedure call).

To remove the request from the queue after a given timeout duration, the
following Ada construct is used::

    success := False;
    select 
        EntryGateSimulator.enter;
        success := True;
    or 
        delay 30.0;
    end select;

After this block of code, the ``success`` variable can be checked in order to
see if the request was handled or not. In the event of a timeout, the Ada
runtime removes the request from the entry queue.

An array of twenty cars is created in the package body, so that they start as
soon as the program is launched, but after the random generator has been
initialized.

C
+

.. highlight:: c

Simulator state monitor
^^^^^^^^^^^^^^^^^^^^^^^

.. c:type:: simulator_state_t

   defined as::

        static struct {
            // random generator seed
            unsigned int rand_seed;

            gate_state_t entry_gate_state;
            gate_state_t exit_gate_state;
            signal_state_t signal_state;
            bool entry_request;
            bool exit_request;
            bool entry_sensor_state;
            bool exit_sensor_state;
            int car_cnt;
            int entry_queue_cnt;
            int exit_queue_cnt;
        } simulator_state;

        static pthread_mutex_t mutex;

   The attributes of this variable should not be accessed directly. There exist
   setter and getter methods for all attributes, for example::

       static entry_request_t simulator_state_get_entry_request() {
           pthread_mutex_lock(&mutex);
           entry_request_t ER = simulator_state.entry_request;
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

   The wait methods are implemented using condition variables, for example::

        static pthread_cond_t cond_entry_gate_open;

        static void simulator_state_wait_entry_gate_open() {
            pthread_mutex_lock(&mutex);
            while (simulator_state.entry_gate_state != GATE_OPEN) pthread_cond_wait(&cond_entry_gate_open, &mutex);
            pthread_mutex_unlock(&mutex);
        }

   The condition variables are signaled inside the setter methods for the ``entry_gate_state`` and ``exit_gate_state`` attributes of the simulation state::

        static void simulator_state_set_entry_gate_state(gate_state_t E) {
            pthread_mutex_lock(&mutex);
            simulator_state.entry_gate_state = E;
            if (E == GATE_OPEN) {
                // ...
                pthread_cond_signal(&cond_entry_gate_open);
            } else {
                pthread_cond_signal(&cond_entry_gate_closed);
            }
            pthread_mutex_unlock(&mutex);
        }

Threads
^^^^^^^

The cars, simulator and logger threads are implemented using POSIX threads::

    static void *entry_gate_simulator(void *args) {
        while (1) {
            // ...
        }
        return NULL;
    }

    void init_simulator() {
        pthread_mutex_init(&mutex, NULL);

        pthread_t entry_gate_simulator_thread;
        if (pthread_create(&entry_gate_simulator_thread, NULL, entry_gate_simulator, NULL) != 0) exit(EXIT_FAILURE);

        // ...
    }

Request Queue
^^^^^^^^^^^^^

The ``RequestQueue`` was implemented as a simple doubly-linked list, which
is protected by a mutex. The following data structures are used:

.. c:type:: request_queue_entry_t

   An entry of the request queue. Defined as a struct with the following
   members:

   .. c:member:: request_queue_entry_t* next

      Pointer to the next element in the queue (or ``NULL``).

   .. c:member:: request_queue_entry_t* prev

      Pointer to the previous element in the queue (or ``NULL``).

   .. c:member:: pthread_cond_t* cond_rendezvous

      Pointer to a condition variable that is signaled by the queue when the
      entry was received by a thread. The requesting thread will then be
      resumed.

   .. c:member:: sem_t* cond_complete

      A pointer to a semaphore that can be signaled by the receiving thread to
      notify the requesting thread that its request has been processed. A
      semaphore is used instead of a condition variable as the two threads do
      not have access to the queue's mutex.

   .. c:member:: int received

      A flag that will be set by the queue when the entry has been received by a
      thread. It is used to check if a request was received or if a timeout was
      the cause why a thread was resumed.

.. c:type:: request_queue_t

   The basic structure of the request queue. Defined as a struct with the
   following members:

   .. c:member:: request_queue_entry_t* first

      Pointer to the first entry in the queue (or ``NULL``).

   .. c:member:: request_queue_entry_t* last

      Pointer to the last entry in the queue (or ``NULL``).

   .. c:member:: pthread_mutex_t mutex

      A mutex which is used to only allow one thread at a time to use the queue.

   .. c:member:: pthread_cond_t cond_not_empty

      A condition variable which is signaled when a request is placed in the
      queue. This allows receiving threads to be resumed when new entries are in
      the queue.

The request queue provides the following functions:

.. c:function:: void request_queue_init(request_queue_t *queue)

   Inititalizes a request queue structure.

.. c:function:: sem_t *request_queue_receive(request_queue_t *queue)

   Blocks the calling thread until a request can be received from the queue.

.. c:function:: void request_queue_enqueue(request_queue_t *queue, sem_t *cond_complete)

   Adds a request to the queue and blocks the calling thread until the request
   was received by a thread. The calling thread has to provide a pointer to a
   semaphore which can be signaled by the receiving thread when the request was
   processed.

.. c:function:: int request_queue_tryenqueue(request_queue_t *queue, sem_t *cond_complete, int timeout_ms)

   Adds a request to the queue and blocks the calling thread until the request
   was received by a thread or the specified timeout duration was exceeded
   (given in milliseconds). The calling thread has to provide a pointer to a
   semaphore which can be signaled by the receiving thread when the request was
   processed.

