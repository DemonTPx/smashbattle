# Smash Battle

![Screenshot of Smash Battle leveleditor working on linux](/screenshot.gif?raw=true)

An 8-bit multiplayer platform shooter.

## Prerequisites

Smash Battle depends on the following packages:

- libsdl1.2
- libsdl-mixer1.2
- libsdl-net1.2
- libcurl
- zlib

### Install dependencies on Ubuntu linux

    sudo apt-get install libsdl1.2{debian,-dev} libsdl-{net,mixer}1.2{,-dev} libcurl4-openssl-dev zlib1g-dev

### Install dependencies on Mac OSX (using brew)

    brew install sql sql_net sql_mixer curl zlib
    
## Build instructions

    cmake .
    make

For a debug build, generate a debug Makefile with:

    cmake -DCMAKE_BUILD_TYPE=Debug .
    
### Building Smash Battle variants

Variant determine which characters and title screen are loaded. They can be configured in `Main.h`. There are three variant at the moment:

#### Smash Battle

Do not define `PBWEB` or `TWEAKERS`.

#### PB Web Media Battle

Define `PBWEB`.

#### Tweak Battle

Define `TWEAKERS`. 

## Building documentation

    sudo apt-get install doxygen
    cd /path/to/your/smashbattle
    doxygen Doxyfile

Documentation will be created in `docs/' directory.

## Level editor

The level editor can be [found on github](https://github.com/DemonTPx/smashbattle-leveleditor).

## Networking

#### Running a server

Simply launch a server:

    smashbattle -s "BOULDERDASH" 1100 "Some server name here" [ {true|false} ]

Optional parameter is no\_sdl flag

#### Running a client

    smashbattle --> choose "play online"

Or use the shortcut (can be convenient when debugging):

    smashbattle -c host:port [ character_name ]

#### Simple ksh script to debug server with multiple clients

    #!/bin/ksh
    # CONTROL+C on this script will kill all running smashbattle exes.
    trap "killall -9 smashbattle" 2
    
    ./smashbattle -s "TRAINING DOJO" 1100 "RAY'S TRAINING SERVER" &
    sleep 3
    
    ./smashbattle -c localhost:1100 2>&1 >>/dev/null &
    sleep 0.3
    ./smashbattle -c localhost:1100 2>&1 >>/dev/null &
    sleep 0.3
    ./smashbattle -c localhost:1100 2>&1 >>/dev/null &
    sleep 0.3
    ./smashbattle -c localhost:1100 2>&1 >>/dev/null &
    sleep 0.3
    ./smashbattle -c localhost:1100 2>&1 >>/dev/null &
    # etc....

    # You have one hour to kill everything with ctrl+c. :)
    sleep 3600
