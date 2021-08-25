/*
* ex5a2.c: Sheard Memory: Application server
*
*  Created on: Dec 10, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*
*  Application server:
*  Application server enables testing of a number of preliminary  
*  or test if a string is a Palindrome is the server accepts the 
*  order from the client
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
// ----------------------- Define ----------------------------
#define MAX_TEXT 20
#define ARR_SIZE 20
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
// ---------------------- Prototype --------------------------
void create_message(key_t *key_msg, int *id_server);
int check_memory(S *shm_ptr, pid_t pid);
void do_command(struct my_msgbuf *my_msg);
int is_prime(int number);
int check_palindrome(char string[]);
void signal_handler(int signal);
//----------------------- For signal -------------------------
int end = 0;
// ------------------------ Main -----------------------------
int main()
{
	signal(SIGTTIN, signal_handler);

	struct my_msgbuf my_msg;
	S *shm_ptr;
	key_t key, key_shm;
	int msqid, shm_id;


	create_message(&key, &msqid);

	if ((key_shm = ftok("/tmp", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	shm_id = shmget(key_shm, 0, 0600);
	if (shm_id == -1)
	{
		perror("shm get failed");
		exit(EXIT_FAILURE);
	}

	shm_ptr = (S *)shmat(shm_id, NULL, 0);
	if (shm_ptr == (S *)-1)
	{
		perror("shm mat failed");
		exit(EXIT_FAILURE);
	}


	while (1)
	{
		if (msgrcv(msqid, &my_msg, sizeof(struct Data), APPS, 0) == -1)
		{
			perror("msgrcv failed");
			kill(getpid(), SIGTTIN);
		}
		my_msg._type = my_msg._data._id;

		if (check_memory(shm_ptr, my_msg._data._id))
			do_command(&my_msg);
		else
			my_msg._data._answer = -1;

		if (!end && msgsnd(msqid, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			kill(getpid(), SIGTTIN);
		}
		if (end)
			break;
	}

	if (msgctl(msqid, IPC_RMID, NULL) == -1)
	{
		perror("msgctl failed");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
// ------------------- create_message ------------------------
/*
* The function allocates memory for the application
* and implementation of client-server
*/
void create_message(key_t *key_msg, int *id_server)
{
	if ((*key_msg = ftok("/tmp", 'z')) == -1)
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
//--------------------- do_command ---------------------------
/*
 * The function reads the user command in the file and 
 * performs the same command that requested
 */
void do_command(struct my_msgbuf *my_msg)
{
	if (my_msg->_data._command == 'n')
		my_msg->_data._answer = is_prime(my_msg->_data._num);
	else if (my_msg->_data._command == 's')
		my_msg->_data._answer = check_palindrome(my_msg->_data._text);
}
// -------------------- check_memory -------------------------
/*
 * The function checks if the user is in the process of shared memory
 */
int check_memory(S *shm_ptr, pid_t pid)
{
	int index;

	for (index = 0; index < ARR_SIZE; index++)
		if (shm_ptr[index].pid == pid)
			return 1;
	return 0;
}
//----------------------- is_prime ---------------------------
/*
 * The function takes a number and checks whether it is prime.
 * Returns 1 or 0 respectively to the number if it is preliminary
 */
int is_prime(int number)
{
	int div;
	if (number == 1)
		return 0;
	for (div = 2; div <= number / 2; div++)
		if (number % div == 0)
			return 0;
	return 1;
}
//------------------- check_palindrome -----------------------
/*
 * The function takes a string and checks whether the string is a 
 * Palindrome, Returns 1 or 0 if the string is Palindrome
 */
int check_palindrome(char string[])
{
	int start = 0;
	int end = strlen(string);

	while (start < end)
	{
		if (string[start] != string[end])
			return 0;
		start++;
		end--;
	}
	return 1;
}
// --------------------- signal_handler ----------------------
/*
* signal_handler called When the server needs to log out
*/void signal_handler(int signal)
{
	end = 1;
}
