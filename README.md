## Dependencies

Runtime requirements

    sudo aptitude install libsdl-mixer1.2 libsdl-net1.2 libcurl3

Development additional requirements

    sudo aptitude install libsdl-dev libsdl-mixer1.2-dev libsdl-net1.2-dev libcurl3-dev

## Branches

* master
* network_multiplayer (development of network multiplayer game)

## Building

    cmake .
    make
    sudo make install

(Use `cmake -DCMAKE_BUILD_TYPE=Debug .' for a debug build.. )

## Running server (temporarily)

    smashbattle -s

(Server cannot be started yet without an SDL window)

## Running client (temporarily)

    smashbattle smashbattle://domain:port

