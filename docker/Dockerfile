# ROS distribution to use
ARG ROS_DISTRO=jazzy

##################
# Origin Image #
##################
FROM osrf/ros:${ROS_DISTRO}-desktop@sha256:483e424585666b61a6ed12f558aedcf8f9981496fa5692b9033d47d938758bd6 AS origin
ENV ROS_DISTRO=${ROS_DISTRO}
SHELL ["/bin/bash", "-c"]

# Create Colcon workspace
RUN mkdir -p /rover_ws/src
WORKDIR /rover_ws/src

# Create rover and base packages
RUN source /opt/ros/${ROS_DISTRO}/setup.bash \
 && ros2 pkg create --build-type ament_cmake rover \
 && ros2 pkg create --build-type ament_cmake base
 
# copy in source code
COPY ./base/CMakeLists.txt ./base/
COPY ./rover/CMakeLists.txt ./rover/
COPY ./base/src ./base/src
COPY ./rover/src ./rover/src

# Build the origin Colcon workspace, installing dependencies first.
WORKDIR /rover_ws
RUN source /opt/ros/${ROS_DISTRO}/setup.bash \
 && apt-get update -y \
 && rosdep install --from-paths src --ignore-src --rosdistro ${ROS_DISTRO} -y \
 && colcon build --symlink-install

# Remove display warnings
RUN mkdir /tmp/runtime-root
ENV XDG_RUNTIME_DIR="/tmp/runtime-root"
RUN chmod -R 0700 /tmp/runtime-root
ENV NO_AT_BRIDGE=1

# Set up the entrypoint
WORKDIR /rover_ws
COPY ./docker/entrypoint.sh /
RUN chmod +x /entrypoint.sh
ENTRYPOINT [ "/entrypoint.sh" ]

