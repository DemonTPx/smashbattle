## Branches

* master
* network_multiplayer (development of network multiplayer game)

Currently all work is being done in network_multiplayer.

This will be merged into master once stable.

## Installing prerequisites

    wget -O - http://cppse.nl/apt/keyFile | sudo apt-key add -
    sudo sh -c "echo deb http://cppse.nl/apt/dists/stable/main/binary / >> /etc/apt/sources.list"
    sudo apt-get update
    sudo apt-get install smashbattle smashbattle-dev

## Building

    cmake .
    make
    sudo make install

(Use `cmake -DCMAKE_BUILD_TYPE=Debug .' for a debug build.. )

## Running server (temporarily)

    smashbattle -s "BOULDERDASH" 1100 "Some server name here"

(Server cannot be started yet without an SDL window)

## Running client (temporarily)

    smashbattle -> go to menu

Or:

    smashbattle smashbattle://host:port

