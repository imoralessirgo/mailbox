MAILBOX
This program aims to help with understanding of threads and semaphores. 
The program takes in a number from the user (must be less than 11) and creates that number of threads.
Each thread then gets assigned a mailbox, which can hold a single message at a time. After these are created 
the parent thread then prompts the user to input a message and a recipient child thread mailbox. 
Using semaphores the acces to such mailboxes is protected allowing a a single thread to access the shared memory
at a time.

The user can then termnate the program by sending a \n as input. Each thread then reports the status of it's mailbox and exits.  



To compile this program execute the following command:
	gcc -std=gnu99 -o mail mail.c -lpthread


This will generate the executable mail.


