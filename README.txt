 ============================================ 
 ex5a1.c: Sheard Memory: Registration server=
       Author: eliyho tsuri
        Login: eliyhots
		Id: 201610672

description:
  In shared memory that processes numbers plugged into the system.
  The server produces queue for customers. Allocates memory to be 
  used by him and his partner application server, The server allows
  two things add new process ,removes the existing process,
  Now he is informed through Message Queuing to add process 
  checks whether the number of the process which exists in shared,
  memory, If the process does not exist and there is space in the 
  data structure he adds the process, Answer is 0 or 1 when you 
  receive a prompt to uninstall process removes it.
 
Compile: gcc -Wall ex5a1.c -o ex5a1
Run: ex5a1.out

Input: not input 
Output : not Output 
 ============================================ 
 ex5a2.c: Sheard Memory: Application server
       Author: eliyho tsuri
        Login: eliyhots
		Id: 201610672

description:
  Application server:
  Application server enables testing of a number of preliminary  
  or test if a string is a Palindrome is the server accepts the 
  order from the client
 
Compile: gcc -Wall ex5a2.c -o ex5a2
Run: ex5a2.out

Input: not input 
Output : not Output
 ============================================  
ex5a3.c: Sheard Memory: Client processes

      Author: eliyho tsuri
       Login: eliyhots
          Id: 201610672
description:

 Client processes:
  The client connects to the server through system when want to
  do something that is a prime number checking Or check string if  
  Palindrome, he turns to the application server and waiting for
  him to finally answer the customer print the answer.
Compile: gcc -Wall ex5a3.c -o ex5a3
Run: ex5a3.out

Input: 'n' 's' or 'e'
Output : answer if the number is prime or string palindrom
