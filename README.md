# rover
Files that belong on the rover's computer.

# base
Files that belong on the base station's computer.

# Running code
Execute these commands in the root folder of this repository on linux.

Build the docker image:
```
sudo docker build -f docker/Dockerfile --target origin -t rover:origin .
```

Run the image for the rover:
```
sudo docker run -it --net=host --ipc=host --privileged rover:origin bash -c "ros2 launch rover launch.xml"
```

Run the image for the base station:
```
sudo docker run -it --net=host --ipc=host --privileged rover:origin bash -c "ros2 launch base launch.xml"
```
