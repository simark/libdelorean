_If my calculations are correct, when this baby hits 88 miles per hour...
you're gonna see some serious shit._


libdelorean
===========

[![Build Status](https://travis-ci.org/eepp/libdelorean.svg?branch=master)](https://travis-ci.org/eepp/libdelorean)

***libdelorean*** is a cross-platform C++11 library that allows you to record
and query a history of intervals. Like Doc Brown's
[famous DeLorean](http://en.wikipedia.org/wiki/DeLorean_time_machine),
it's super fast to go to a specific point in time.

libdelorean is based on Alexandre Montplaisir's work and follows it more
or less:

  * [Stockage sur disque pour accès rapide d'attributs avec intervalles
    de temps] [1] (thesis, French)
  * [State History Tree: an Incremental Disk-based Data Structure for
    Very Large Interval Data] [2]
  * [State History Storage in Disk-based Interval Trees] [3]
  * [State History System] [4]
  * [State System and History for Trace Viewers] [5]

[1]: http://publications.polymtl.ca/752/1/2011_AlexandreMontplaisirGoncalve.pdf
[2]: http://www.dorsal.polymtl.ca/fr/system/files/document.pdf
[3]: http://dmct.dorsal.polymtl.ca/sites/dmct.dorsal.polymtl.ca/files/AMG_StateHistory_29062010.pdf
[4]: http://dmct.dorsal.polymtl.ca/sites/dmct.dorsal.polymtl.ca/files/AMG_StateHistory_20110511.pdf
[5]: http://secretaire.dorsal.polymtl.ca/~alexmont/AMG_StateHistory_20111209.pdf

libdelorean stores intervals (with associated data of your choice) in a
possibly humongous tree built in a specific fashion allowing logarithmic
time queries as well as fast construction.

There's one major condition (detailed in aforementioned papers), though:
intervals must be provided to the library in ascending order of *end time*.

Speaking of which, "timestamps" are provided to libdelorean as 64-bit signed
integers, so your "time" axis doesn't need to contain actual timestamps; the
integers could represent something else.

See [concepts](#concepts) below for more details.


dependencies
------------

You need [Boost](http://www.boost.org/). Any version somehow recent should
do it.

You need [CppUnit](http://cppunit.sourceforge.net/) to build and run
unit tests.


building
--------

libdelorean uses SCons for its build infrastructure. After cloning, simply
issue:

    $ scons

to build the shared library and tests.


concepts
--------

From a user's point of view, libdelorean involves the following concepts:

  * An **interval** is a line between two points in time (or two points in
    whatever dimension you would like). This object has the following
    attributes:
    * A **begin** timestamp (inclusive)
    * An **end** timestamp (inclusive)
    * A **key**. In a history (see below), two intervals having the same
      _key_ cannot intersect. See the [use cases](#use cases)
      section for examples of what a key can be. A key is a unique numeric
      identifier for a set of related intervals (see it as the primary key
      of a given timeline).
    * A **value**. The value type depends on the interval type. Standard
      intervals are provided with the library (intervals that carry integers,
      floating point numbers or strings), but you are free to create your
      own.
  * A **history** is a collection of intervals. There are two flavors:
    * A **history sink** is a history in which intervals may be added.
    * A **history source** is a history in which intervals may be found.
  * A **history file** is a file that holds a history. It can be written
    using a **history file sink** and read using a **history file source**.

Intervals _must be_ added to a history sink in _ascending order of end
timestamp_. This is important to avoid rebalancing the internal history tree
(and, consequently, accelerate the construction process) since the data
structure assumes this condition. Thus, libdelorean is not suited for any
interval-storing application. The few use cases listed in the following
section should help you understand its purpose.

Once a history is created, you may find intervals in it when opened as a
history source. "Finding intervals" means providing a single timestamp and
getting back a list of all intervals intersecting this timestamp. You may
also find a single interval intersecting a given timestamp and matching
a given key.


use cases
---------

libdelorean is particularly well suited (and was designed) for storing a
history of **states**. A _state_ is a set of informations defined between
two given instants in time (i.e. in an interval). Storing states usually
satisfies the ascending order of end timestamp condition because an interval
is created everytime a state ends (and a new state for the same key
may begin right after that).

Here are a few possible use cases that make this obvious:

  * **Persons in charge of multiple projects**
    * _Keys_: individual projects.
    * _Interval payload_: name of the person in charge of a project
      associated with the interval's key.
    * _State change_: when the charge of a given project is transfered from
      one person to another, an interval is created.
    * _"Find one" query_: find which person was in charge of a given
      project at a given time.
    * _"Find all" query_: find all persons in charge of all projects at a
      given time.
  * **Musical composition**
    * _Keys_: individual instruments (or tracks).
    * _Interval payload_: volume and notes of an instrument/track
      associated with the interval's key.
    * _State change_: when an instrument stops playing a note, starts
      playing a new note or changes the volume of any note, an interval
      is created.
    * _"Find one" query_: find all notes/volumes of a given instrument/track
      at a given time.
    * _"Find all" query_: find all notes/volumes of all instruments/tracks at
      a given time.
  * **States of operating system processes**
    * _Keys_: individual OS processes.
    * _Interval payload_: process state associated with the interval's key.
    * _State change_: when a process goes from one state to another,
      an interval is created.
    * _"Find one" query_: find the state of a given process at a given time.
    * _"Find all" query_: find all states of all processes at a given time.
  * **Shipped packages tracking**
    * _Keys_: individual shipped packages.
    * _Interval payload_: location of a package associated with the
      interval's key.
    * _State change_: when a package is transfered from one location to
      another (collection to truck, truck to customs, truck to exchange,
      exchange to truck, truck to post office, delivery, etc.), an interval
      is created.
    * _"Find one" query_: find the location of a given package at a given time.
    * _"Find all" query_: find all locations of all packages at a given time.
  * **Meteorology**
    * _Keys_: individual meteorological stations.
    * _Interval payload_: meteorological status (condition, pressure,
      temperature, visibility, humidity, wind, etc.) of a station associated
      with interval's key.
    * _State change_: when a station detects a meteorological change,
      an interval is created.
    * _"Find one" query_: find the meteorological status of a given station
      at a given time.
    * _"Find all" query_: find all meteorological statuses of all stations at
      a given time.

See data in [`tests/data/headsofstates.src.txt`](tests/data/headsofstates.src.txt)
and its sorted by end timestamp version in
[`tests/data/headsofstates.txt`](tests/data/headsofstates.txt) for a
concrete example.


using
-----

Not available yet.
