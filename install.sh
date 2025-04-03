#! /usr/bin/bash

# Set up files for compilation in temp directory
mkdir /tmp/packbunch
curl -o /tmp/packbunch/packbunch.cpp https://raw.githubusercontent.com/tudorconst/packbunch/refs/heads/main/packbunch.cpp
curl -o /tmp/packbunch/Makefile https://raw.githubusercontent.com/tudorconst/packbunch/refs/heads/main/Makefile

# Compile packbunch
make -C /tmp/packbunch packbunch

# Move compiled binary to final location
sudo mv /tmp/packbunch/packbunch /usr/local/bin/

# Cleanup
rm -r /tmp/packbunch