# rover_control.ino
For arduino that controls rover motors.

# socket-arduino_bridge.cpp
Relays messages from network to arduino.
Belongs in:
/home/rover/rover/networking/socket-arduino_bridge.out
Compiles with:
g++ socket-arduino_bridge.cpp -o socket-arduino_bridge.out

# rover-startup.sh
Runs the compiled socket-arduino_bridge.cpp as root.
Belongs in:
/usr/local/sbin/rover-startup.sh

# rover-startup.service
runs rover-startup.sh on device boot.
Belongs in:
/etc/systemd/system/rover-startup.service
