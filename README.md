_If my calculations are correct, when this baby hits 88 miles per hour...
you're gonna see some serious shit._


libdelorean
===========

***libdelorean*** is a cross-platform C++11 library that allows you to record
and query a history of intervals. Like Doc Brown's
[famous DeLorean](http://en.wikipedia.org/wiki/DeLorean_time_machine),
it's super fast to go to a specific point in time.

libdelorean is based on
[Alexandre Montplaisir's work](http://publications.polymtl.ca/752/1/2011_AlexandreMontplaisirGoncalve.pdf)
(French) and follows it more or less. It stores intervals (with associated
data of your choice) in a possibly humongous tree built in a specific fashion
allowing logarithmic time queries as well as fast construction.

There's one major condition (detailed in the paper), though: intervals must
be provided to the library in ascending order of *end time*.

Speaking of which, "time stamps" are provided to libdelorean as 64-bit
integers, so your "time" axis doesn't need to contain actual time stamps; the
integers could represent something else.

Although the output format is optimized for multi-gigabyte files stored on
a hard disk drive, libdelorean also provides an API for in-memory history.


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


using
-----

Not available yet.
