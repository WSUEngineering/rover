# rover_control.ino
For arduino that controls rover motors.

# socket-arduino_bridge.cpp
Relays messages from network to arduino.
Belongs in the ROS2 rover package

# CMakeLists.txt
Used in the ROS2 rover package to compile source code.

# rover-startup.sh
Runs the compiled socket-arduino_bridge.cpp as root.
Belongs in:
/usr/local/sbin/rover-startup.sh

# rover-startup.service
runs rover-startup.sh on device boot.
Belongs in:
/etc/systemd/system/rover-startup.service
