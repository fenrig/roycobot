#include <sys/types.h>
#include <unistd.h> 
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

pid_t procRijden;
pid_t procImgproc;
pid_t procPathplanner;
pid_t procRosCore;

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
		dup((int)fopen("roscore.txt", "w+"));
		int status = system("roscore");
		exit(0);	
	}

	usleep(15000000);

	procRijden = fork();
	if(procRijden == 0){
		//close(1);
		//dup((int)fopen("rijden.txt", "w+"));
		int status = system("rosrun roycobot rijden");
		exit(0);
	}

	procImgproc = fork();
	if(procImgproc == 0){
		close(1);
		dup((int)fopen("imgProc.txt", "w+"));
		int status = system("rosrun roycobot imgProc");
		exit(0);
	}

	procPathplanner = fork();
	if(procPathplanner == 0){
		close(1);
		dup((int)fopen("pathplanner.txt", "w+"));
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
