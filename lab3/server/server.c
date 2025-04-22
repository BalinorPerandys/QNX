#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <string.h>

void compressString(const char *input, char *output) {
    int count = 1;
    char current = input[0];
    int j = 0;

    for (int i = 1; input[i] != '\0'; i++) {
        if (input[i] == current) {
            count++;
        } else {
            j += sprintf(&output[j], "%d%c", count, current);
            current = input[i];
            count = 1;
        }
    }
    sprintf(&output[j], "%d%c", count, current);  
}

int main(void){
	int rcvid;
	int child;
	char message[512];
	char response[512];

	printf("Server start working\n");
	child = ChannelCreate(0);
	printf("Channel id %d \n", child);
	printf("Pid %d \n", getpid());

	while(1){
		rcvid = MsgReceive(child, message, sizeof(message), NULL);
		printf("Receive message rcvid=%X \n", rcvid);
		printf("Request text: \"%s\".\n", message);

		compressString(message, response);

		MsgReply(rcvid, EOK, response, sizeof(response));
		printf("Response text: \"%s\".\n", response);
	}
	return 0;	
}