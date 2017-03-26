Pressure- and Temperature-Control
=================================

Overview
--------

For this exercise, a regulation for a simple embedded system is to be programmed
using multiple threads or tasks.

The embedded system is contained in an environment, of which the temperature and
pressure can be measured and manipulated using a heater and a valve. As
temperature and pressure are completely independent, the embedded system shall
be implemented using two concurrent threads, which measure temperature and
control the heater (``TempController``) respectively measure the pressure and
control the valve (``PressureController``). The system can also write to a
display, in order to display the current temperature and pressure values.

This can be seen in the following diagram:

.. image:: ../../uml/build/simple_embedded_system_overview.*

The provided simulation simulates changes of temperature and pressure according
to the heater and valve settings, as well as random changes due to external
influences. 

The main program (which has to be provided by the students) should implement
``TempController`` and ``PressureController``, so that temperature and pressure
are regulated to a given reference value. The simulation therefore provides an
interface, with which these actions can be performed. The interface is shown in
the following diagram:

.. image:: ../../uml/build/simple_embedded_system_interface.*

The ``DataTypes`` module contains type definitions for values used in
the IO functions. The ``IO`` module contains the functions which can be used
read the current temperature and pressure, as well as write settings for the
heater and valve. One can also write a read temperature value or pressure value
to the console. The functions are named differently in Ada and C. The names and
usage description can be found in the respective subsections (see below). 

The simulation library also has to be included differently in each programming
language. Details can be found in the following sections :ref:`parking_lot_ada`
and :ref:`parking_lot_c`. The section :ref:`parking_lot_implementation` contains
further details about the implementation of the simulation.

The Simulation also includes some output, which allows the user to track the
current values of pressure, temperature and the corresponding settings. The
output can be customized by global constants in both implementations (see
following sections).



.. _ptc_ada:

Ada
---

.. highlight:: ada

The simulation library provides the following modules and files:

 - ``Data_Types`` (data_types.ads)
 - ``IO`` (io.ads, io.adb)
 - ``Control_Procedures`` (``control_procedures.ads``, ``control_procedures.adb``)

These can be included using ``with``/``use`` statements::

   with Data_Types; use Data_Types;
   with IO; use IO;

Data types
++++++++++

The module ``Data_Types`` defines types which are used by the procedures in the
IO package. The definition of these data types allows specifying multiple
``Read``/``Write`` procedures which are overloaded by the different data types.
As they are all defined as new types, they can not be accidentally converted
into each other, which can help to prevent errors. The provided types are:

.. ada:type:: type Data_Types.TempReading is new Integer range 0..500;

   defined as::

      new Integer range 0..500

   Can be used to save a temperature value (range is the sensor's utilizable
   temperature range, 0 to 500 degrees Celsius, for instance)

.. ada:type:: type Data_Types.PressureReading is new Integer range 0..1200;

   defined as::

      new Integer range 0..1200

   Can be used to save a pressure value (range is the sensor's utilizable
   pressure range, 0 to 1200 mbar, for instance)

.. ada:type:: type Data_Types.HeaterSetting is (On, Off);

   defined as::

      (On, Off)

   Can be used to save the state of the heater. As it is controlled by a switch,
   the only two states are ``On`` and ``Off``.

.. ada:type:: type Data_Types.PressureSetting is new Integer range -4..4;

   defined as::

      new Integer range -4..4

   Can be used to save the setting of the valve. With negative values, the
   pressure is decreased and increased with positive values. Higher values
   increase/decrease the pressure more.

Reading the current temperature and pressure values
+++++++++++++++++++++++++++++++++++++++++++++++++++

.. ada:procedure:: procedure IO.Read(TR : out TempReading);

   Reads the current temperature value and stores it in TR.

.. ada:procedure:: procedure IO.Read(PR : out PressureReading);

   Reads the current pressure value and stores it in PR.

Writing heater and valve settings
+++++++++++++++++++++++++++++++++

.. ada:procedure:: procedure IO.Write(HS : HeaterSetting);

   Sets a new setting for the heater.

.. ada:procedure:: procedure IO.Write(PS : PressureSetting);

   Sets a new setting for the valve.

Writing read values to console
++++++++++++++++++++++++++++++

.. ada:procedure:: procedure IO.Write(TR : TempReading);

   Prints a temperature value on the console.

.. ada:procedure:: procedure IO.Write(PR : PressureReading);

   Prints a pressure value on the console.

Calculating settings values
+++++++++++++++++++++++++++

The simulation also contains procedures which allow the user to calculate the
correct settings for a given temperature or pressure value. Using these
procedures should allow easy regulation of the temperature and pressure.

.. ada:procedure:: procedure Control_Procedures.Temp_Convert(TR : TempReading; HS : out HeaterSetting);

   Calculates the correct heater setting if the temperature is to be regulated
   to 20 (degrees). Requires the current temperature (TR) and stores the result in HS. 

.. ada:procedure:: procedure Control_Procedures.Pressure_Convert(PR : PressureReading; PS : out PressureSetting);

   Calculates the correct valve setting if the pressure is to be regulated to
   1000 (mbar). Requires the current pressure (PR) and stores the result in PS.


Log output customization
++++++++++++++++++++++++

The simulation library allows customization of which output is printed. The
configuration can be performed in ``io.ads``::

    -- The logger prints the current temperature and pressure values, as well as
    -- the current heater and valve settings.
    DISPLAY_LOGGER : constant Boolean := True;
    -- If set to True, the messages sent to the display are printed in the
    -- console
    DISPLAY_CONSOLE : constant Boolean := True;


.. _ptc_c:

C
-

.. highlight:: c

The simulation library provides the following modules and files:

 - ``Data_Types`` (``data_types.h``)
 - ``IO`` (``io.c``, ``io.h``)
 - ``Control_Procedures`` (``control_procedures.c``, ``control_procedures.h``)

These can be included using ``#include``-statements::

   #include "data_types.h"
   #include "io.h"

Data types
++++++++++

The module ``Data_Types`` defines types which are used by the functions in the
IO package. The types are:

.. c:type:: temp_reading_t

   defined as::

      int

   Can be used to represent temperature values. This definition is mainly because
   of readability, as no type-checks are performed by C (as opposed to Ada).

.. c:type:: pressure_reading_t

   defined as::

      int

   Can be used to represent pressure values. This definition is mainly because
   of readability, as no type-checks are performed by C (as opposed to Ada).

.. c:type:: heater_setting_t

   defined as::

      enum {OFF, ON}

   Can be used to represent a setting of the heater. As it is controlled by a
   simple switch, the heater can only be on or off.

.. c:type:: pressure_setting_t

   defined as::

      int

   Can be used to represent a setting for the valve. The allowed range for this
   setting is -4 to +4.


Initialization
++++++++++++++

In order to start the simulation, the following function has to be called inside
the ``main`` function:

.. c:function:: void init_simulator()

    Starts the simulation.

Reading the current temperature and pressure values
+++++++++++++++++++++++++++++++++++++++++++++++++++

.. c:function:: void read_temp(temp_reading_t *TR)

   Reads the current temperature value and stores it in the variable pointed to by TR.

.. c:function:: void read_pressure(pressure_reading_t *PR)

   Reads the current pressure value and stores it in the variable pointed to by PR.

Writing heater and valve settings
+++++++++++++++++++++++++++++++++

.. c:function:: void write_heater_setting(heater_setting_t HS)

   Sets a new setting for the heater.

.. c:function:: void write_pressure_setting(pressure_setting_t PS)

   Sets a new setting for the valve.

Writing read values to console
++++++++++++++++++++++++++++++

.. c:function:: void write_temp_reading(temp_reading_t TR)

   Prints a temperature value on the console.

.. c:function:: void write_pressure_reading(pressure_reading_t PR)

   Prints a pressure value on the console.

Calculating settings values
+++++++++++++++++++++++++++

The simulation also contains procedures which allow the user to calculate the
correct settings for a given temperature or pressure value. Using these
procedures should allow easy regulation of the temperature and pressure.

.. c:function:: void convert_temp(temp_reading_t TR, heater_setting_t *HS)

   Calculates the correct heater setting if the temperature is to be regulated
   to 20 (degrees). Requires the current temperature (TR) and stores the result
   in the variable pointed to by HS.

.. c:function:: void convert_pressure(pressure_reading_t PR, pressure_setting_t *PS)

   Calculates the correct valve setting if the pressure is to be regulated to
   1000 (mbar). Requires the current pressure (PR) and stores the result
   in the variable pointed to by PS.

Delay
+++++

The IO module also provides a function to halt a thread for a specified number
of milliseconds:

.. c:function:: void delay_ms(int ms)

   Halts the current thread for a given number of milliseconds (``ms``).

Log output customization
++++++++++++++++++++++++

The simulation library allows customization of which output is printed. The
configuration can be performed in ``io.h``::

    // The logger prints the current temperature and pressure values, as well as
    // the current heater and valve settings.
    #define DISPLAY_LOGGER 1
    // If set to 1, the messages sent to the display are printed in the console
    #define DISPLAY_CONSOLE 1


.. _ptc_implementation:

Implementation details
----------------------

The simulation takes part inside the ``IO`` package. Two threads are responsible
for calculate the next values for the temperature and pressure
(``TempSimulator`` and ``PressureSimulator``). The current values and settings
are kept inside two monitor objects, in order to ensure that only one thread at
a time can access the values inside the monitor. There also exists a ``Logger``
thread, which periodically prints the current values of temperature, pressure
and the corresponding settings. In order to allow random changes of the
temperature and pressure, the simulator threads use a shared ``RandomState``
object, which contains the state of the random generator and features a
method to calculate the next random number. The RandomState is also a
monitor, so that its methods can safely be used by multiple threads.

This can be seen in the following diagram:

.. image:: ../../uml/build/simple_embedded_system_simulators.*

The monitor objects have getter and setter methods, which can be used to access
and manipulate the values and settings.

Simulation of temperature
+++++++++++++++++++++++++

Basically, the ``TempSimulator`` increments the temperature by one if the heater
is on, or decrements it by two if it is off. Random changes in temperature occur
with a probability of 6 percent, the temperature then rises or falls by 4
(degrees). 

A regulation for this environment then has to permanently switch the heater on
and off, if a constant temperature shall be maintained. A duty-cycle of 66% is
required then. If the temperature is already too high, the heater has to be off,
so that the temperature cools down automatically. If the temperature is way too
low, the heater has to be on for a longer time.

The procedure :ada:proc:`Control_Procedures.Temp_Convert` implements such a
simple regulation.

Simulation of pressure
++++++++++++++++++++++

The ``PressureSimulator`` simulates that pressure can be controlled by a valve.
The pressure can increase, decrease or stay constant depending on the value
of the valve setting. Basically, the simulator just adds the valve setting
(which can be positive or negative) to the previous pressure value in order to
generate the next value. Also, random errors may occur with a probabilty of 10
percent. The pressure may then increase or decrease by 30 (mbar).

A regulation for this environment should set the valve setting positive or
negative depending on whether the pressure is too low or too high, or to zero if
it is already at the required value. Higher values for the setting can lead to
reaching the required pressure faster.

The procedure :ada:proc:`Control_Procedures.Pressure_Convert` implements a
simple regulation for this environment.

Ada
+++

State monitors
^^^^^^^^^^^^^^

In Ada, the monitor objects are implemented using the built-in feature of
``protected`` objects, which ensure that only one thread at a time can use
variables which are inside a protected object. For example, the definition of
the ``TempState`` looks as follows::

    protected TempState is
        function GetTemp return TempReading;
        procedure SetTemp(TR: TempReading);
        function GetHeaterSetting return HeaterSetting;
        procedure SetHeaterSetting(HS: HeaterSetting);
    private
        temp : TempReading := 0;
        heater_setting : HeaterSetting := Off;
    end TempState;

    protected body TempState is
        function GetTemp return TempReading is (temp);
        procedure SetTemp(TR: TempReading) is begin temp := TR; end SetTemp;
        function GetHeaterSetting return HeaterSetting is (heater_setting);
        procedure SetHeaterSetting(HS: HeaterSetting) is begin heater_setting := HS; end SetHeaterSetting;
    end TempState;

Threads
^^^^^^^

The simulator and logger threads are implemented as Ada tasks, for example::

    task PressureSimulator;
    task body PressureSimulator is
        // ...
    begin
        loop
            // ...
        end loop;
    end PressureSimulator;

C
+

State monitors
^^^^^^^^^^^^^^

In C, the monitor objects are implemented using ``structs`` and POSIX mutexes.
For example::

    static pthread_mutex_t temp_mutex;
    static struct {
        temp_reading_t temp;
        heater_setting_t setting;
    } temp_state;

The properties can then be accessed and manipulated using getter and setter
functions, for example::

    static temp_reading_t get_temp() {
        pthread_mutex_lock(&temp_mutex);
        temp_reading_t TR = temp_state.temp;
        pthread_mutex_unlock(&temp_mutex);
        return TR;
    }

    static void set_temp(temp_reading_t TR) {
        pthread_mutex_lock(&temp_mutex);
        temp_state.temp = TR;
        pthread_mutex_unlock(&temp_mutex);
    }

Using these functions ensures that only one thread at a time can access the
properties of the struct.

Threads
^^^^^^^

The simulator and logger threads are implemented using POSIX threads, for
example::

    static void *temp_simulator(void *args) {
        while (1) {
            // ...
        }
        return NULL;
    }

    void init_simulator() {
        // ...

        pthread_t temp_simulator_thread;
        if (pthread_create(&temp_simulator_thread, NULL, temp_simulator, NULL) != 0) exit(EXIT_FAILURE);

        // ...
    }
