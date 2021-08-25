/*  ex5b.c: Sheared memory
*
*
*  Created on: Dec 10, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*  Parent process allocates memory and creates two child processes,
*  Each of the books until Grill loop lottery number. When the 
*  child process Prime Grill is stored, In the shared memory
*  Killed his brother sends a signal to horho ends. The parent 
*  displays the number and finish is also
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>  // for wait()
#include <stdbool.h>
#include <sys/ipc.h>
#include <unistd.h>    // for fork()
#include <time.h>
#include <signal.h>    // for signal
#include <sys/shm.h>
// ----------------------- Define ----------------------------
#define MAX_NUMBERS 20
#define ARR_SIZE 5
#define MAX_TEXT 10
// ----------------------- Struct ----------------------------
typedef struct S
{
	int m_prime;
}S;
// ---------------------- Prototype --------------------------
int set_number();
bool check_prime(int num);
void wait_childred(pid_t status[]);
void do_father();
// ------------------------ Main -----------------------------
int main(void)
{
	signal(SIGUSR1, do_father);

	S *shm_ptr;
	pid_t status[2];
	key_t key;
	int shm_id, number;
	int child;

	if ((key = ftok(".", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	shm_id = shmget(key, 1000, IPC_CREAT | IPC_EXCL | 0600);
	if (shm_id == -1)
	{
		perror("shm get failed");
		exit(EXIT_FAILURE);
	}

	if ((shm_ptr = (S *)shmat(shm_id, NULL, 0)) < 0)
	{
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}


	for (child = 0; child < 2; child++)
	{
		status[child] = fork();
		if (status[child] < 0)
		{
			perror("fork() failed");
			exit(EXIT_FAILURE);
		}
		if (status[child] == 0)
		{
			while (1)
			{
				number = set_number();
				if (check_prime(number))
				{
					shm_ptr[0].m_prime = number;
					if (child == 0)
						kill(SIGKILL, status[1]);
					else
						kill(SIGKILL, status[0]);
					kill(getppid(), SIGUSR1);
					exit(EXIT_SUCCESS);
				}
			}
		}
		else
			wait_childred(status);
	}
	return (EXIT_SUCCESS);
}
// ---------------------- set_number -------------------------
/*
* This function return lottery number
*/
int set_number()
{
	srand(time(NULL));
	return (rand() % MAX_NUMBERS);
}
//---------------------- check_prime -------------------------
/*
* This function getting number and check if the number is prime
*/
bool check_prime(int num)
{
	int div;

	if (num == 0 || num == 1)
		return false;
	for (div = 2; div <= num / 2; div++)
		if (num % div == 0)
			return false;
	return true;
}
//----------------------- do_fatehr --------------------------
void do_father()
{
	key_t key;
	int shm_id;
	S *shm_ptr;

	if ((key = ftok(".", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	shm_id = shmget(key, 0, 0600);
	if (shm_id == -1)
	{
		perror("shm get failed");
		exit(EXIT_FAILURE);
	}

	shm_ptr = (S *)shmat(shm_id, NULL, 0);
	if (shm_ptr == (S *)-1)
	{
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}
	printf("%d\n", shm_ptr->m_prime);
	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
	{
		perror("shmctl failed");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
//--------------------- wait_children ------------------------
/*
* This function waiting for the childred
*/
void wait_childred(pid_t status[])
{
	int child;
	for (child = 0; child < 2; child++)
		waitpid(status[child], NULL, 0);
}

