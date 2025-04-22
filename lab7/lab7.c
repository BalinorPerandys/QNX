#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/siginfo.h>
#define MY_TIMER_CODE 10

int main(){
	int chid;
	int rcvid;
	struct _pulse pls;
	if((chid=ChannelCreate(0))==-1){
		perror("ChannelCreate");
		exit(EXIT_FAILURE);
	}
	if(timer_setup(chid)==-1){
		perror("timer_seetup()");
		exit(EXIT_FAILURE);
	}
	while(1){
		MsgReceivePulse(chid,&pls,sizeof(pls),NULL);
		if(pls.code==MY_TIMER_CODE){
			printf("Tick.\n");
		}
	}
	return EXIT_SUCCESS;
}

int timer_setup(int chid){
	timer_t tid;
	struct sigevent event;
	struct itimerspec timer;
	int coid;
	coid=ConnectAttach(0,0,chid,0,0);
	if(coid==-1){
		return -1;
	}
	SIGEV_PULSE_INIT(&event,coid,SIGEV_PULSE_PRIO_INHERIT,MY_TIMER_CODE,0);
	if(timer_create(CLOCK_REALTIME,&event,&tid)==-1){
		return -1;
	}
	timer.it_value.tv_sec=3;
	timer.it_value.tv_nsec=0;
	timer.it_interval.tv_sec=1;
	timer.it_interval.tv_nsec=0;
	if (timer_settime(tid,0,&timer,NULL)==-1){
		return -1;
	}
	return 0;
}