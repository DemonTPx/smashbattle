
Tweakbattle is a spin-off from Smashbattle, and both games will be merged into
one later.

## Available branches

* master
* network\_multiplayer (development of network multiplayer game)

Currently all work is being done in network\_multiplayer.
The branch is no longer only about the network multiplayer feature, but
currently also about "tweak battle".

This will be merged into master once stable.

## Installing prerequisites Ubuntu

There is a virtual package named `smashbattle-dev' that you can install, and it
will install all required development packages for building smashbattle..

    wget -O - http://cppse.nl/apt/keyFile | sudo apt-key add -
    sudo sh -c "echo deb http://cppse.nl/apt/dists/stable/main/binary / >> /etc/apt/sources.list"
    sudo apt-get update
    sudo apt-get install smashbattle smashbattle-dev

## Building Tweakbattle/Smashbattle

    cmake .
    make
    sudo make install

Note that when running the game, smashbattle will expect certain stuff to be in
`/usr/share/games/smashbattle'.
That's why you should at least now and then perform the make install.

For a debug build, generate a debug Makefile with `cmake -DCMAKE_BUILD_TYPE=Debug .'

## Building documentation

    sudo apt-get install doxygen
    cd /path/to/your/smashbattle
    doxygen Doxyfile

Documentation will be created in `docs/' directory.

Documentation is build automatically by jenkins, and is available here:

http://cppse.nl/smashbattle/docs/

## Jenkins

Jenkins is configured at http://cppse.nl:8080/

Get username and password in #smashbattle on freenode :)

Currently there are two projects:

- smashbattle      - automatically builds merged PR's in
                     git@github.com:/DemonTPx/smashbattle
- smashbattle\_ray - automatically builds pushed commits to
                     git@github.com:/rayburgemeestre/smashbattle

## Running server

Simply launch a server:

    smashbattle -s "BOULDERDASH" 1100 "Some server name here" [ {true|false} ]

Optional parameter is no\_sdl flag

## Running client

    smashbattle --> choose "play online"

Or use the shortcut (can be convenient when debugging):

    smashbattle -c host:port [ character_name ]

## Simply ksh script to debug server with multiple clients


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
