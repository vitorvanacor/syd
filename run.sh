#!/bin/bash
gnome-terminal --geometry=150x30+400+30 -e "bash -c \"./bin/syd; read -p 'Press Enter to exit...'\""
cd bin ; ./server
