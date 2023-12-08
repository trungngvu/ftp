#include "lib/server.h"

int main(int argc, char const *argv[])
{
	getcwd(root_dir, sizeof(root_dir));
	int ListenSock, CtrlSock, pid;

	if ((ListenSock = socket_create()) < 0)
	{
		perror("Error creating socket");
		exit(1);
	}

	while (1)
	{ // wait for client request

		// create new socket for control connection
		if ((CtrlSock = socket_accept(ListenSock)) < 0)
			break;

		// create child process to do actual file transfer
		if ((pid = fork()) < 0)
		{
			perror("Error forking child process");
		}
		else if (pid == 0)
		{
			close(ListenSock);
			ftserve_process(CtrlSock);
			close(CtrlSock);
			exit(0);
		}
		close(CtrlSock);
	}

	close(ListenSock);
	return 0;
}