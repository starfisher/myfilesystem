#include "filesym.h"

int main()
{
	int semid = semget(999, 1, 0666|IPC_CREAT);
	printf("%d", semctl(semid, 0, GETVAL));	
	return ;
}
