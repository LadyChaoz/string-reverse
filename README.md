# String Reverse
Reverses and capitalizes strings using a variety of signals and POSIX methods.  
Programmer: Anne Warden

##Welcome to String Reverse!
String reverse uses a UNIX command line to create a terminal environment in which user input will be capitalized, reversed and displayed. The program will continue to prompt for user input and display a result until the quit condition is fulfilled.
*The current quit condition is fulfilled by typing "^D" into the terminal. __Do not use ctrl+D.__*

##Reverse
*Do not call this program.* The master will fork to call this program and use a message queue to pass user input. When reverse is started, it will grab a semaphore, which it will release after passing it's coverted data to a shared memory segment.

##Upper
*Do not call this program.* The master will fork to call this program, which will wait for reverse to release the semaphore and grab it's data from a shared memory segment. It will then convert the string to uppercase and display it to the terminal. Once upper has displayed the text, it will release the semaphore and use a signal queue to iterate the master program.
