#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <climits>

#include <errno.h>

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class MinimalSubscriber : public rclcpp::Node
{
	public:
	MinimalSubscriber() : Node("socket_arduino_bridge")
	{
		auto topic_callback =
			[this](std_msgs::msg::String::UniquePtr msg) -> void {
				// write to arduino
				std::cout << msg->data.c_str() << std::flush;
			};
		subscription_ =
			this->create_subscription<std_msgs::msg::String>("drive_input", 20, topic_callback);
	}

	private:
	rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
	// set stdout to arduino serial console
	int descriptor;
	while ((descriptor = open("/dev/ttyACM0", O_RDWR)) < 0)
	{
		perror("Failed to connect with arduino");
		sleep(3);
	}
	dup2(descriptor, STDOUT_FILENO);

	//wait for arduino to restart
	sleep(3);

	// start ros node
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<MinimalSubscriber>());
	rclcpp::shutdown();
	
	//set drive train velocity to zero on shutdown
	std::cout << "l0\nr0\n" << std::flush;
	return 0;
}
