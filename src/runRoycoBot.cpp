#include <sys/types.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#define DEBUG_LOGGING 1
#define DEBUG_LOG_NULL 0

#define DEBUG DEBUG_LOG_NULL

pid_t procRijden;
pid_t procImgproc;
pid_t procPathplanner;
pid_t procRosCore;
pid_t procShareLoc;

void my_handler(int s){
           printf("Caught signal %d\n",s);
           kill(procRijden, SIGKILL);
	   kill(procImgproc, SIGKILL);
	   kill(procPathplanner, SIGKILL);
	   kill(procRosCore, SIGKILL);
	   exit(0);
}

int main(void){
	int status;

	procRosCore = fork();
	if(procRosCore == 0){
		close(1);
#if (DEBUG & DEBUG_LOGGING)
		dup((int)fopen("roscore.txt", "w+"));
#else
                dup((int)fopen("/dev/null", "w"));
#endif
		int status = system("roscore");
		exit(0);	
	}

	usleep(30000000);

	procRijden = fork();
	if(procRijden == 0){
		close(1);
#if (DEBUG & DEBUG_LOGGING)
		dup((int)fopen("rijden.txt", "w+"));
#else
                dup((int)fopen("/dev/null", "w"));
#endif
		int status = system("rosrun roycobot rijden");
		exit(0);
	}

	procImgproc = fork();
	if(procImgproc == 0){
		close(1);
#if (DEBUG & DEBUG_LOGGING)
		dup((int)fopen("imgProc.txt", "w+"));
#else
                dup((int)fopen("/dev/null", "w"));
#endif
		int status = system("rosrun roycobot imgProc");
		exit(0);
	}
	
	procShareLoc = fork();
	if(procShareLoc == 0){
	        close(1);
#if (DEBUG & DEBUG_LOGGING)
		dup((int)fopen("shareLoc.txt", "w+"));
#else
                dup((int)fopen("/dev/null", "w"));
#endif
		int status = system("rosrun roycobot shareLoc");
		exit(0);
	}

	procPathplanner = fork();
	if(procPathplanner == 0){
	        //close(1);
#if (DEBUG & DEBUG_LOGGING)
		dup((int)fopen("pathplanner.txt", "w+"));
#else
                dup((int)fopen("/dev/null", "w"));
#endif
		int status = system("rosrun roycobot pathplanner");
		exit(0);
	}
	

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);

	usleep(1000000);
	pause();

	return 0;
}
