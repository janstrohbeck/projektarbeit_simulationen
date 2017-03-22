Pressure- and Temperature-Control
=================================

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

.. ada:type:: type Temp_Reading is new Integer range 0..500;

   defined as::

      new Integer range 0..500

   Can be used to save a temperature value (range is the sensor's utilizable
   temperature range, 0 to 500 degrees Celsius, for instance)

.. ada:type:: type Pressure_Reading is new Integer range 0..1200;

   defined as::

      new Integer range 0..1200

   Can be used to save a pressure value (range is the sensor's utilizable
   pressure range, 0 to 1200 mbar, for instance)

.. ada:type:: type Heater_Setting is (On, Off);

   defined as::

      (On, Off)

   Can be used to save the state of the heater. As it is controlled by a switch,
   the only two states are ``On`` and ``Off``.

.. ada:type:: type Pressure_Setting is new Integer range -4..4;

   defined as::

      new Integer range -4..4

   Can be used to save the setting of the valve. With negative values, the
   pressure is decreased and increased with positive values. Higher values
   increase/decrease the pressure more.

Reading the current temperature and pressure values
+++++++++++++++++++++++++++++++++++++++++++++++++++

.. ada:procedure:: procedure IO.Read(TR : out Temp_Reading);

   Reads the current temperature value and stores it in TR.

.. ada:procedure:: procedure IO.Read(PR : out Pressure_Reading);

   Reads the current pressure value and stores it in PR.

Writing heater and valve settings
+++++++++++++++++++++++++++++++++

.. ada:procedure:: procedure IO.Write(HS : Heater_Setting);

   Sets a new setting for the heater.

.. ada:procedure:: procedure IO.Write(PS : Pressure_Setting);

   Sets a new setting for the valve.

Writing read values to console
++++++++++++++++++++++++++++++

.. ada:procedure:: procedure IO.Write(TR : Temp_Reading);

   Prints a temperature value on the console.

.. ada:procedure:: procedure IO.Write(PR : Pressure_Reading);

   Prints a pressure value on the console.

Calculating settings values
+++++++++++++++++++++++++++

The simulation also contains procedures which allow the user to calculate the
correct settings for a given temperature or pressure value. Using these
procedures should allow easy regulation of the temperature and pressure.

.. ada:procedure:: procedure Control_Procedures.Temp_Convert(TR : Temp_Reading; HS : out Heater_Setting);

   Calculates the correct heater setting if the temperature is to be regulated
   to 20 (degrees).

.. ada:procedure:: procedure Control_Procedures.Pressure_Convert(PR : Pressure_Reading; PS : out Pressure_Setting);

   Calculates the correct valve setting if the pressure is to be regulated to
   1000 (mbar).


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

Ada
+++

In Ada, the monitor objects are implemented using the built-in feature of
``protected`` objects, which ensure that only one thread at a time can use
variables which are inside a protected object. For example, the definition of
the ``TempState`` looks as follows::

    protected TempState is
        function Temp return Temp_Reading;
        procedure SetTemp(TR: Temp_Reading);
        function Heater return Heater_Setting;
        procedure SetHeater(HS: Heater_Setting);
    private
        mTemp : Temp_Reading := 0;
        mHeater : Heater_Setting := Off;
    end TempState;

    protected body TempState is
        function Temp return Temp_Reading is (mTemp);
        procedure SetTemp(TR: Temp_Reading) is begin mTemp := TR; end SetTemp;
        function Heater return Heater_Setting is (mHeater);
        procedure SetHeater(HS: Heater_Setting) is begin mHeater := HS; end SetHeater;
    end TempState;


