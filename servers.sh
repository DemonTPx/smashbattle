#!/bin/bash

# This script will start a given number of Tweak Battle servers
# You can customize the nr of servers to run and the starting 
# range of the TCP/IP ports. ps: 48 is nice since we have 16 levels.

# Kill all running servers
killall -9 smashbattle

# Wait for it ...
sleep 2

# Number of servers we want to start
servercount=24

# Define a starting port
port=2000

# Define array of maps
levels=(
	"TRAINING DOJO" 
	"PLATFORM ALLEY" 
	"PITTFALL" 
	"DUCK'N'HUNT" 
	"COMMON GROUNDS" 
	"POGOSTICK" 
	"LA MOUSTACHE" 
	"THE FUNNEL" 
	"BLAST BOWL" 
	"PIT OF DEATH" 
	"RABBIT HOLE" 
	"STAY HIGH" 
	"PIE PIT" 
	"SLIP'N'SLIDE" 
	"BOULDERDASH" 
	"SNOW FIGHT"
)

# Counter to display nr of servers
count=0

# Reset counter for levels array
levelcount=0

while [  $count -lt $servercount ]; do

        # Launch a server
        printf "Starting Tweak Battle Server #$count \n"
        (daemon -- smashbattle -s "${levels[$levelcount]}" $port "${levels[$levelcount]}" > /var/log/smashbattle/server-${count}) &
        sleep 1

        # Increment port, loopcount and count
        port=$((port+1))
        count=$((count+1))
	levelcount=$(((levelcount+1) % 16))
done
