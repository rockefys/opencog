Overview
========

The AtomSpacePublisherModule class publishes AtomSpace change events
across the network using [ZeroMQ sockets](http://zeromq.org) to allow for external
clients to receive updates from the AtomSpace via a publish/subscribe
pattern.

Clients can subscribe to the events by subscribing to a ZeroMQ socket.

##### Supported events:

*   **add**        (Atom added)
*   **remove**     (Atom removed)
*   **tvChanged**  (Atom TruthValue changed)
*   **avChanged**  (Atom AttentionValue changed)
*   **addAF**      (Atom was added to the AttentionalFocus)
*   **removeAF**   (Atom was removed from the AttentionalFocus)

The message is a JSON-formatted string that follows the same conventions
as the [REST API](http://wiki.opencog.org/w/Web_interface).

##### Potential usage examples:

- Debugging
- Monitoring the dynamics of the AttentionAllocation system as it propagates STI and LTI throughout the network and the AttentionalFocus changes
- Monitoring PLN
- Allowing external modules such as the AtomSpace Visualizer to subscribe to real-time updates

Configuration
=============

Prerequisites
-------------

Requires ZeroMQ version 3.2.4 (libzmq3-dev) or higher.

This will be installed automatically if you are using [ocpkg](https://github.com/opencog/ocpkg), or can be
installed manually here: <http://zeromq.org/intro:get-the-software>

Module
------

This is implemented as a Module named *libatomspacepublisher.so* which must be enabled in the *opencog.conf* file.

Parameters
----------

The default configuration parameters in opencog.conf are:

`  ZMQ_EVENT_USE_PUBLIC_IP = TRUE`

`  ZMQ_EVENT_PORT = 5563`

### ZMQ\_EVENT\_USE\_PUBLIC\_IP

If set to TRUE, this parameter will allow ZeroMQ to publish AtomSpace
events to the public internet. If set to FALSE, ZeroMQ will publish
events to localhost.

### ZMQ\_EVENT\_PORT

This is the port that ZeroMQ will use to publish AtomSpace events.

Message format
==============

Atom object
-----------

##### Format

    {
            "atom": {
	        "handle": UUID,
	        "type": TYPENAME,
	        "name": NAME,
                "attentionvalue": ATTENTIONVALUETYPE,
	        "truthvalue": {
	            "type": TRUTHVALUETYPE,
	            "details": TRUTHVALUEDETAILS 
	        },
	        "outgoing": "[ UUID1, UUID2 ... ]",
	        "incoming": "[ UUID1, UUID2 ... ]"
	    }
	}


#### UUID

integer

#### TYPENAME

*Atom type, see [OpenCog Atom types](http://wiki.opencog.org/w/OpenCog_Atom_types)*

string (choose from the list of available atom types)

#### NAME

string

#### ATTENTIONVALUETYPE

##### STI

integer

##### LTI

integer

##### VLTI

boolean

#### TRUTHVALUETYPE

Valid types are:

-   simple
-   count
-   indefinite

*see [TruthValue](http://wiki.opencog.org/w/TruthValue)*

#### TRUTHVALUEDETAILS

The format of the TRUTHVALUEDETAILS object depends on the value of the
TRUTHVALUETYPE parameter.

#### simple

    {
	    'count': TRUTHVALUECOUNT,
	    'confidence': TRUTHVALUECONFIDENCE,
	    'strength': TRUTHVALUESTRENGTH
	}

###### TRUTHVALUECOUNT
float

###### TRUTHVALUECONFIDENCE
float

###### TRUTHVALUESTRENGTH
float

#### count

    {
	    'count': TRUTHVALUECOUNT,
	    'confidence': TRUTHVALUECONFIDENCE,
	    'strength': TRUTHVALUESTRENGTH
	}

###### TRUTHVALUECOUNT
*(see above)*

###### TRUTHVALUECONFIDENCE
*(see above)*

###### TRUTHVALUESTRENGTH
*(see above)*

#### indefinite

	{
	    'strength': TRUTHVALUESTRENGTH,
	    'L': TRUTHVALUEL,
	    'U': TRUTHVALUEU,
	    'confidence': TRUTHVALUECONFIDENCE,
	    'diff': TRUTHVALUEDIFF,
	    'symmetric': TRUTHVALUESYMMETRIC
	}

###### TRUTHVALUECONFIDENCE
*(see above)*

###### TRUTHVALUESTRENGTH
*(see above)*

###### TRUTHVALUEL

###### TRUTHVALUEU

###### TRUTHVALUEDIFF

###### TRUTHVALUESYMMETRIC

Event types
===========

The AtomSpace change event publisher binds to Boost Signals2 signals generated by
the AtomSpace class.

The following event types are available:

add
---

Triggered whenever an atom is added.

ZeroMQ subscription channel name: **add**

##### Format

    {
        "atom": ATOM
    }

remove
------

Triggered whenever an atom is removed.

ZeroMQ subscription channel name: **remove**

##### Format

    {
        "atom": ATOM
    }

avChanged
---------

Triggered whenever the AttentionValue of an atom is changed.

ZeroMQ subscription channel name: **avChanged**

##### Format

    {
        "handle": HANDLE,
        "avOld": ATTENTIONVALUETYPE,
        "avNew": ATTENTIONVALUETYPE,
        "atom": ATOM
    }

tvChanged
---------

Triggered whenever the TruthValue of an atom is changed.

ZeroMQ subscription channel name: **tvChanged**

##### Format

    {
        "handle": HANDLE,
        "tvOld": TRUTHVALUETYPE,
        "tvNew": TRUTHVALUETYPE,
        "atom": ATOM
    }

addAF
---------

Triggered whenever the AttentionValue of an atom changes, if the new STI value
is within the AttentionalFocus boundary and the old STI value is not.

ZeroMQ subscription channel name: **addAF**

##### Format

    {
        "handle": HANDLE,
        "avOld": ATTENTIONVALUETYPE,
        "avNew": ATTENTIONVALUETYPE,
        "atom": ATOM
    }

removeAF
---------

Triggered whenever the AttentionValue of an atom changes, if the old STI value
is within the AttentionalFocus boundary and the new STI value is not.

ZeroMQ subscription channel name: **removeAF**

##### Format

    {
        "handle": HANDLE,
        "avOld": ATTENTIONVALUETYPE,
        "avNew": ATTENTIONVALUETYPE,
        "atom": ATOM
    }

Example clients
===============

Python
------

An example client is available here:

<https://github.com/opencog/external-tools/blob/master/AtomSpaceSubscriber/SampleClient/client.py>

C++
---

Detailed examples of how to subscribe to events and parse the JSON using
Boost Property Trees can be found in the unit tests, located here:

<https://github.com/opencog/opencog/blob/master/tests/persist/zmq/events/AtomSpacePublisherModuleUTest.cxxtest#L140>

JavaScript
----------

A server exists to forward ZeroMQ messages to socket.io sockets to allow
JavaScript web applications to subscribe to AtomSpace events, even
across domains.

To enable this server, follow the instructions in this file:

<https://github.com/opencog/opencog/blob/master/opencog/python/web/socketio/atomspace_publisher.py>

An example client application is available in this file:

<https://github.com/opencog/opencog/blob/master/opencog/python/web/socketio/index.html>

***

##### Todo

- There is a need in the AtomSpace for a new Boost Signals2 signal type,
AttentionalFocusBoundaryChanged. It should contain the old boundary and the new
boundary. Without that, there can be a case where the AttentionalFocusBoundary
changes, and as a result the publisher does not publish all of the addAF and
removeAF events that should have occured, because those events are currently
only triggered by the AVChangedSignal.

- Currently, the **AtomSpace** class triggers redundant events under certain
circumstances. This has been noted in this open issue:
<https://github.com/opencog/opencog/issues/394>
