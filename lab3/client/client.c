#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <string.h>

int main(void){
	char smsg[20] = "aaabbcccrrr";
	char rmsg[200];
	int coid;
	long serv_pid;
	
	printf("Enter server PID: ");
	scanf("%ld", &serv_pid);
	
	coid = ConnectAttach(0, serv_pid, 1, 0, 0);
	printf("Connect res %d\n", coid);
	
	printf("Sending request: %s\n", smsg);
	
	if(MsgSend(coid, smsg, strlen(smsg) + 1, rmsg, sizeof(rmsg)) == -1){
		printf("Error MsgSend\n");
	}
	
	printf("Response: \"%s\"\n", rmsg);
	ConnectDetach(coid);
	
	return 1;
}