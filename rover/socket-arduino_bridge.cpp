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

static struct sockaddr_in serv_addr;

int main()
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

	int listenfd = 0, connfd = 0;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	listen(listenfd, 1);

	// accept connections from network
	while(true)
	{
		connfd = accept(listenfd, nullptr, nullptr);

		// relay messages to arduino until connection closes.
		char input;
		while(read(connfd, &input, 1) > 0)
		{
			std::cout << input;
			if(input == '\n')
				std::cout << std::flush;
		}

		//set drive train velocity to zero when connection closes
		std::cout << "l0\nr0\n" << std::flush;

		close(connfd);
		sleep(1);
	}
}
