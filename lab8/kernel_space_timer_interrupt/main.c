#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <hw/inout.h>

#include <pthread.h>

volatile uintptr_t timer_base;
volatile int counter = 0;
int counter_2=0;

pthread_t thread_printer_id;

void *thread_printer(void *notused){
	while(1){
		if(counter>1000){
			counter_2++;
			printf("Timer ISR called: %d\n", counter_2);
		}
		sleep(1);
	}
}
const struct sigevent *timer_isr(void *arg, int id) {
    counter++;
    out32(timer_base + 0x0, 0x1);
    return NULL;
}

int main() {
    int intr;
    struct sigevent event;
    struct _clockperiod period;
    if(ThreadCtl(_NTO_TCTL_IO,0)==-1){
    	perror("ThreadCtl");
    	return EXIT_FAILURE;
    }

    timer_base = (uintptr_t)mmap_device_io(4, 0x80810000); 
    intr = InterruptAttach(0, timer_isr, NULL, 0,0);
    if (intr == -1) {
        perror("InterruptAttach failed");
        return EXIT_FAILURE;
    }

    period.nsec = 1000000; 
    period.fract = 0;
    ClockPeriod(CLOCK_REALTIME, &period, NULL, 0);
    pthread_create(&thread_printer_id,NULL,thread_printer,NULL);
    printf("Timer ISR is running. Press Enter to exit...\n");
    getchar();
    InterruptDetach(intr);
    munmap_device_io((void*)timer_base, 4);
    return EXIT_SUCCESS;
}
