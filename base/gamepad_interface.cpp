//functions to manually load a dynamic library
#include <dlfcn.h>

//for HID input and creating application windows
#include <SDL2/SDL.h>

#include <iostream>
#include <math.h>

//for linux commands
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

//joystick smoothing variables
float const JOYSTICK_DEADZONE = 0.1f;
float const JOYSTICK_CURVE_COEFFICIENT = 4.0f;

//home orientation for tower camera
float const CAM_TILT_HOME = 8500;
float const CAM_PAN_HOME = 10400;

//maximum changes in tilt and pan angles of tower camera from home position
float const CAM_TILT_MAX_DELTA = 15000;
float const CAM_PAN_MAX_DELTA = 10400;

//minimum and maximum angles for camera tilt and pan
float const CAM_MIN = 2400;
float const CAM_MAX = 17200;

using std::cout;
using std::cerr;
using std::endl;
using std::string;

struct Point {
	float x;
	float y;
	
	Point(float x, float y) {
		this->x = x;
		this->y = y;
	}
};

//a positive only version of the modulus function
//returns positive remainder of x/y
float mod(float const x, float const y)
{
	if(x < 0.0f)
	{
		return fmodf(x, y) + y;
	}
	else
	{
		return fmodf(x, y);
	}
}

//applies an exponential curve to x or outputs zero if within deadzone
float joystickSmoothing(float x)
{
	float const c = 4.0f;
	return (powf(expf(JOYSTICK_CURVE_COEFFICIENT) + 1.0f, fabsf(x)) - 1.0f) /
		expf(JOYSTICK_CURVE_COEFFICIENT);
}

//cyclic pattern alternating between 1 and -1 with period of 4*PI
float h(float const x)
{
	return (mod(x + M_PI + M_PI/4.0f, 2.0f*M_PI) - 
		mod(x + M_PI/4.0f, 2.0f*M_PI)) / M_PI;
}

//A curve following a triangle wave pattern with a period of 4*PI
float g(float const x)
{
	return (mod(x + M_PI/4.0f, M_PI) * 4.0f/M_PI - 2.0f) * h(x);
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//if side == false then velocity will be returned for left drive train
//v is the vector whose y axis determines velocity and 
//x axis determines angular velocity
float getVelocity(Point v, bool side)
{
	//v magnitude
	float vMagnitude = 
		sqrt(v.x*v.x + v.y*v.y);
		
	//angle of vector v
	float const vAngle = 
		copysignf(acosf(v.x/vMagnitude), v.y);
	
	//limit vector magnitude between deadzone and 1.0
	vMagnitude = map(vMagnitude, JOYSTICK_DEADZONE, 1.0f, 0.0f, 1.0f);
	if(vMagnitude > 1.0f)
		vMagnitude = 1.0f;
	else if(vMagnitude < 0.0f)
		return 0.0f;
		
		
	float b = 7.0f*M_PI/4.0f;
	if(side)
	{
		b *= -1.0f;
	}
	
	//calculate maximum magnitude for side
	float const g1 = g(vAngle - b);
	float const g2 = g(vAngle - b - M_PI/2.0f);
	float const maxMagnitude = (g1 - g2) / 2.0f;
	
	//velocity for selected side
	float const sideMagnitude = maxMagnitude * vMagnitude;
	
	//cerr << "side" << (int)side << "Angle: " << vAngle << endl <<
	//	"side" << (int)side << "Magnitude: " << sideMagnitude << endl;
	
	return sideMagnitude;
}

int main(int argc, char *argv[])
{	
	//dirty hack to prevent undefined udev symbols in SDL
	dlopen("/lib/x86_64-linux-gnu/libudev.so", RTLD_NOW|RTLD_GLOBAL);
	
	// set stdout to arduino serial console
	//int descriptor = open("/dev/ttyACM0", O_RDWR);
	//fflush(NULL);
	//dup2(descriptor, STDOUT_FILENO);
	
	//connect to rover
	int sockfd = 0;
    	struct sockaddr_in serv_addr;
    	
    	if(argc != 2)
	{
		cerr << "\n Usage: " << argv[0] << " <ip of server> \n";
		return 1;
	}
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error : Could not create socket");
		return 1;
	} 
	
	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(5000);
    	
    	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		perror("inet_pton error occured");
		return 1;
	}
    	
    	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Error : Connect Failed");
		return 1;
	}
	
	dup2(sockfd, STDOUT_FILENO);

	//initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO | 
		SDL_INIT_JOYSTICK | 
		SDL_INIT_GAMECONTROLLER) < 0)
	{
		cerr << "SDL Ran into an error" << endl;
		cerr << SDL_GetError() << endl;
		
		return 0;
	}
	else
	{
		cerr << SDL_GetError() << endl;

		SDL_GameController *controller = nullptr;

		//Look for game controller
		for(int i = 0; i < SDL_NumJoysticks(); i++)
		{
			//Make first controller found the one that's used
			if(SDL_IsGameController(i))
			{
				cerr << "Controller found\n";
				
				controller = SDL_GameControllerOpen(i);
				if(controller == NULL)
					cerr << "Error:" << SDL_GetError() << endl;
				else
				{
					char * controllerMap = 
						SDL_GameControllerMapping(controller);
						
					//check if controller map is returned
					if(controllerMap == NULL)
						cerr << "Error:" << SDL_GetError() << endl;
					else
					{
						cerr << "Controller map:" << endl;
						cerr << controllerMap << endl;
					}
				}
				
				cerr << "exit" << endl;
				
				break;
			}
		}
		
		if(controller == nullptr)
		{
			cerr << "No controller found\n";
			return 0;
		}
		else
		{
			bool isRunning = true; 
			SDL_Event ev;
			
			Point leftStick = Point(0.0f, 0.0f);
			Point rightStick = Point(0.0f, 0.0f);
			
			string lastMessage = "";
			
			//Event polling loop
			while(isRunning)
			{
				//iterate through all present events
				while(SDL_PollEvent(&ev) != 0)
				{
					//exit loop on quit event
					if(ev.type == SDL_QUIT)
						isRunning = false;
					//act on button press
					else if(ev.type == SDL_CONTROLLERBUTTONDOWN)
					{
						if(ev.cbutton.button == SDL_CONTROLLER_BUTTON_A)
							cerr << "button A\n";
						else if(ev.cbutton.button == SDL_CONTROLLER_BUTTON_B)
							cerr << "button B\n";
					}
					//act on joystick movement
					else if(ev.type == SDL_CONTROLLERAXISMOTION)
					{
						//print joystick activity
						/*
						cerr << "Controller: " << ev.caxis.which << endl;
						cerr << "Axis: " << (int)ev.caxis.axis << endl;
						cerr << "Value: " << ev.caxis.value << endl;
						*/
						
						//update joystick axis
						switch((int)ev.caxis.axis)
						{
							case 0:
								leftStick.x = ((float)ev.caxis.value)/32768.0;
								break;
							case 1:
								leftStick.y = ((float)ev.caxis.value)/-32768.0;
								break;
							case 2:
								rightStick.x = ((float)ev.caxis.value)/32768.0;
								break;
							case 3:
								rightStick.y = ((float)ev.caxis.value)/-32768.0;
								break;
						}
					}
				}
				
				//left joystick magnitude
				float lStickMagnitude = 
					sqrt(leftStick.x*leftStick.x + leftStick.y*leftStick.y);
				
				/*
				//set camera tower orientation when joystick is beyond deadzone
				if(lStickMagnitude > JOYSTICK_DEADZONE)
				{
					ccMsg.ctt = -leftStick.x*CAM_TILT_MAX_DELTA + CAM_TILT_HOME;
					if(ccMsg.ctt < CAM_MIN)
					{
						ccMsg.ctt = CAM_MIN;
					} else if (ccMsg.ctt > CAM_MAX)
					{
						ccMsg.ctt = CAM_MAX;
					}
					ccMsg.ctp = -leftStick.y*CAM_PAN_MAX_DELTA + CAM_PAN_HOME;
					if(ccMsg.ctp < CAM_MIN)
					{
						ccMsg.ctp = CAM_MIN;
					} else if (ccMsg.ctp > CAM_MAX)
					{
						ccMsg.ctp = CAM_MAX;
					}
				}
				//Set camera tower to home position
				else
				{
					ccMsg.ctt = CAM_TILT_HOME;
					ccMsg.ctp = CAM_PAN_HOME;
				}
				*/
				
				//Set drivetrain velocity
				int leftTrainVel = getVelocity(rightStick, false)*100;
				int rightTrainVel = getVelocity(rightStick, true)*-100;
				
				
				cerr << "x1: " << leftStick.x << endl <<
					"y1: " << leftStick.y << endl <<
					"x2: " << rightStick.x << endl <<
					"y2: " << rightStick.y << endl <<
					"l1: " << lStickMagnitude << "\n\n";
					
				
				
				
				//display contents of user input message
				cerr << "leftTrainVel: " << leftTrainVel << endl <<
					"rightTrainVel: " << rightTrainVel << "\n\n";
					
				//send message to arduino when different
				string message = "l"+std::to_string(leftTrainVel)+"\nr"+std::to_string(rightTrainVel)+"\n";
				if(message != lastMessage)
				{
					cout << message << std::flush;
					lastMessage = message;
				}
				
				//display sent message
				cerr << message << "END" << endl;
					
				usleep(20000);
			}

			if(controller != NULL)
				SDL_GameControllerClose(controller);

			SDL_Quit();

			return 0;
		}
	}
}
