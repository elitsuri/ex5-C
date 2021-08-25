/*
* ex5a1.c: Sheard Memory: Registration server
*
*  Created on: Dec 10, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*  
*  Registration server:
*  In shared memory that processes numbers plugged into the system.
*  The server produces queue for customers. Allocates memory to be 
*  used by him and his partner application server, The server allows
*  two things add new process ,removes the existing process,
*  Now he is informed through Message Queuing to add process 
*  checks whether the number of the process which exists in shared,
*  memory, If the process does not exist and there is space in the 
*  data structure he adds the process, Answer is 0 or 1 when you 
*  receive a prompt to uninstall process removes it.
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for string
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>    // for pid
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>  // for signal
#include <sys/shm.h>
// ----------------------- Define ----------------------------
#define MAX_TEXT 50
#define ARR_SIZE 5
#define SERVER 1
#define APPS 2
// ---------------------- Struct -----------------------------
struct Data
{
	pid_t _id;
	int _num;
	int _answer;
	char _command;
	char _text[MAX_TEXT];
};
struct my_msgbuf
{
	long _type;
	struct Data _data;
};
typedef struct S
{
	pid_t pid;
}S;
//----------------------- For signal -------------------------
int end = 0;
// ---------------------- Prototype --------------------------
void create_memory(key_t *key_shm, int *shm_id, S **shm_ptr);
void create_message(key_t *key_msg, int *id_server);
bool check_memory(S *shm_ptr, pid_t pid);
void signal_handler(int signal);
// ------------------------ Main -----------------------------
int main()
{
	signal(SIGTTIN, signal_handler);

	struct my_msgbuf my_msg;
	key_t key_shm, key_msg;
	int shm_id, id_msg;
	S *shm_ptr;
	int index = 0;

	create_message(&key_msg, &id_msg);
	create_memory(&key_shm, &shm_id, &shm_ptr);



	while (1)
	{
		if (msgrcv(id_msg, &my_msg, sizeof(struct Data), SERVER, 0) == -1)
		{
			perror("msg rcv failed");
			exit(EXIT_FAILURE);
		}
		my_msg._type = my_msg._data._id;
		if (my_msg._data._command == 'e')
			kill(getpid(), SIGTTIN);
		if (index >= ARR_SIZE)
			my_msg._data._answer = 2;
		else
		{
			if (!check_memory(shm_ptr, my_msg._data._id))
			{
				shm_ptr[index].pid = my_msg._data._id;
				my_msg._data._answer = 0;
				index++;
			}
			else
				my_msg._data._answer = 1;
		}
		if (!end && msgsnd(id_msg, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			kill(getpid(), SIGTTIN);
		}
		if (end)
			break;
	}
	if ((msgctl(getpid(), IPC_RMID, NULL)) == -1)
	{
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	}

	shmdt(shm_ptr);
	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
	{
		perror("shmctl failed");
		exit(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
// ------------------- create_memory -------------------------
/*
 * The function allocates memory for the application  
 * and implementation of client-server
 */
void create_memory(key_t *key_shm, int *shm_id, S **shm_ptr)
{
	if ((*key_shm = ftok("/tmp", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	*shm_id = shmget(*key_shm, 1000, IPC_CREAT | IPC_EXCL | 0600);
	if (*shm_id == -1)
	{
		perror("shm get failed");
		exit(EXIT_FAILURE);
	}
	if ((*shm_ptr = (S *)shmat(*shm_id, NULL, 0)) < 0)
	{
		perror("shm mat failed");
		exit(EXIT_FAILURE);
	}
}
// ------------------- create_message ------------------------
/*
 * The function generates a message queue so that you can 
 * communicate with clients that connect to the system
 */
void create_message(key_t *key_msg, int *id_server)
{
	if ((*key_msg = ftok("/tmp", 'y')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	if ((*id_server = msgget(*key_msg, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
		perror("msg get failed");
		exit(EXIT_FAILURE);
	}
}
// -------------------- check_memory -------------------------
/*
 * The function checks whether the number of the 
 * customer's process on the shared memory
 */
bool check_memory(S *shm_ptr, pid_t pid)
{
	int index;
	bool on_arr = false;

	for (index = 0; index < ARR_SIZE; index++)
		if (shm_ptr[index].pid == pid)
			on_arr = true;
	return on_arr;
}
// --------------------- signal_handler ----------------------
/*
 * signal_handler called When the server needs to log out
 */
void signal_handler(int signal)
{
	end = 1;
}
