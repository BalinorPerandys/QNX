#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MEM_NAME "ourmemory"

int threads(void);
void *thread_1(void *);
void *thread_2(void *);
int process_1(void);
int process_2(void);

typedef struct {
	sem_t sem;
	int value;
}sh_mem_t;

pthread_mutex_t mut;
sh_mem_t *sm;

int main(void){
	int md;
	int childpid;
	shm_unlink(MEM_NAME);
	md=shm_open(MEM_NAME,O_RDWR|O_CREAT,0777);
	if(md==-1){
		perror("shm_open()");
		return -1;
	}
	if(ftruncate(md,sizeof(*sm))==-1){
		perror("ftruncate()");
		return -1;
	}
	sm=mmap(NULL,sizeof(*sm),PROT_READ|PROT_WRITE,MAP_SHARED,md,0);
	if(sm==MAP_FAILED){
		perror("nmap()");
		return -1;
	}
	if(sem_init(&(sm->sem),1,1)==-1){
		perror("sem_init()");
		exit(EXIT_FAILURE);
	}
	sm->value=0;
	childpid=fork();
	if(childpid==-1){
		perror("fork()");
	}
	if(childpid){
		if(process_1()){
			perror("process_1()");
			exit(EXIT_FAILURE);
		}
		wait(NULL);
	}else{
		if(process_2()){
			perror("process_2()");
			exit(EXIT_FAILURE);
		}
		if(threads()){
			perror("threads()");
			exit(EXIT_FAILURE);
		}
	}
	close(md);
	shm_unlink(MEM_NAME);
	pthread_mutex_destroy(&mut);
	return EXIT_SUCCESS;
}

int process_1(void){
	sem_wait(&sm->sem);
	printf("First process.\n(1) Previous value: %i\n",sm->value);
	sm->value=10;
	sm->value+=2;
	sm->value*=4;
	sm->value-=15;
	printf("(1)Result: %i\n",sm->value);
	sem_post(&(sm->sem));
	return 0;
}

int process_2(void){
	sem_wait(&sm->sem);
	printf("Second process.\n(1) Previous value: %i\n",sm->value);
	sm->value=7;
	sm->value+=5;
	sm->value*=2;
	sm->value-=1;
	printf("(1)Result: %i\n",sm->value);
	sem_post(&(sm->sem));
	return 0;
}

int threads(){
	pthread_t thid_1,thid_2;
	if(pthread_mutex_init(&mut,NULL)==-1){
		perror("pthread_mutex_init()");
		return -1;
	}
	if((pthread_create(&thid_1,NULL,&thread_1,NULL)==-1)||
		(pthread_create(&thid_2,NULL,&thread_2,NULL)==-1)){
			perror("thread_create()");
			return -1;
		}
		pthread_join(thid_1,NULL);
		pthread_join(thid_2,NULL);
		return 0;
}
	
void *thread_1(void *arg){
	pthread_mutex_lock(&mut);
	printf("First_lock\n");
	sleep(2);
	printf("First_unlock\n");
	pthread_mutex_unlock(&mut);
}

void *thread_2(void *arg){
	pthread_mutex_lock(&mut);
	printf("Second_lock\n");
	sleep(2);
	printf("Second_unlock\n");
	pthread_mutex_unlock(&mut);
}		
					
	
	