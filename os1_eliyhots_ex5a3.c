/*
* ex5a3.c: Sheard Memory: Client processes
*
*  Created on: Dec 10, 2017
*      Author: eliyho tsuri
*       Login: eliyhots
*		  Id: 201610672
*
*  Client processes:
*  The client connects to the server through system when want to
*  do something that is a prime number checking Or check string if  
*  Palindrome, he turns to the application server and waiting for
*  him to finally answer the customer print the answer.
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
// ----------------------- Define ----------------------------
#define MAX_TEXT 20
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
void connect_server(key_t *key_server, int *id_server);
void connect_apps(key_t *key_apps, int *id_apps);
void get_command(struct my_msgbuf *my_msg);
void print_answer(struct my_msgbuf *my_msg);
// ------------------------ Main -----------------------------
int main()
{
	struct my_msgbuf my_msg;
	key_t key_server, key_apps;
	int id_server, id_apps;


	connect_server(&key_server, &id_server);
	connect_apps(&key_apps, &id_apps);



	while (1)
	{
		my_msg._type = SERVER;
		if (msgsnd(id_apps, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
		if (msgrcv(id_apps, &my_msg, sizeof(struct Data), SERVER, 0) == -1)
		{
			perror("msgrcv failed");
			exit(EXIT_FAILURE);
		}

		my_msg._type = APPS;

		get_command(&my_msg);
		if (my_msg._data._command == 'e')
			break;
		print_answer(&my_msg);
		if (msgsnd(id_server, &my_msg, sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
		if (msgrcv(id_server, &my_msg, sizeof(struct Data), APPS, 0) == -1)
		{
			perror("msgrcv failed");
			exit(EXIT_FAILURE);
		}
	}

	my_msg._type = SERVER;
	puts("Program end\n");
	if (msgsnd(id_server, &my_msg, sizeof(struct Data), 0) == -1)
	{
		perror("msgsnd failed");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
// ------------------- connect_server ------------------------
/*
 * The function takes a key and ID number to the client 
 * connect to the registration server
 */
void connect_server(key_t *key_server, int *id_server)
{
	if ((*key_server = ftok("/tmp", 'y')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((*id_server = msgget(*key_server, 0)) == -1)
	{
		perror("msg get  failed");
		exit(EXIT_FAILURE);
	}
}
// -------------------- connect_apps -------------------------
/*
* The function takes a key and ID number to the 
* client connect to the Application server
*/
void connect_apps(key_t *key_apps, int *id_apps)
{
	if ((*key_apps = ftok("/tmp", 'z')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((*id_apps = msgget(*key_apps, 0)) == -1)
	{
		perror("msg get 1 failed");
		exit(EXIT_FAILURE);
	}
}
//----------------------- get_command ------------------------
/*
 * The function takes the structure with which the client 
 * communicates with the servers, The function reads the 
 * command the user wants to ask and the number or string
 */
void get_command(struct my_msgbuf *my_msg)
{
	printf("Enter Command: 'n', 's', 'e' to end\n");
	scanf("%c", &my_msg->_data._command);

	my_msg->_type = APPS;
	if (my_msg->_data._command == 'n')
	{
		puts("Enter number");
		scanf("%d", &my_msg->_data._num);
	}
	else if (my_msg->_data._command == 's')
	{
		puts("Enter string");
		scanf("%s", &my_msg->_data._text[MAX_TEXT]);
	}
}
//---------------------- print_answer ------------------------
/*
 * The function takes the structure with which the   
 * client communicates with the server and prints the   
 * answer returned by the application server
 */
void print_answer(struct my_msgbuf *my_msg)
{
	if (my_msg->_data._answer == -1)
	{
		puts("User is not on system");
		end = 1;
	}
	else
	{
		if (my_msg->_data._command == 'n')
		{
			if (my_msg->_data._answer == 1)
				printf("The number %d is prime number\n", my_msg->_data._num);
			else
				printf("The number %d is not prime number\n", my_msg->_data._num);
		}
		else if (my_msg->_data._command == 's')
		{
			if (my_msg->_data._answer == 1)
				printf("The string %s is palindrome\n", &my_msg->_data._text[MAX_TEXT]);
			else
				printf("The string %s is not palindrome\n", &my_msg->_data._text[MAX_TEXT]);
		}
	}
}
