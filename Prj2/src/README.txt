
To start, go to the Prj2/ directory. There you should see all of the source files and a storage/ folder.
Open a terminal in that directory.
Now type the command:

$ make

to compile all of the files.

Next, here are the use cases for each program.

= = = = = = = = = = = = = = = = = = = = = =
1. Basic Server

$ ./server <PORT>

	<PORT> = A port number that is available, use a high value such as 12000

= = = = = = = = = = = = = = = = = = = = = =
2. Basic Client

* Note: You must start the server above before running this program

$ ./client <IP> <PORT> <STRING>

	<IP>     = The IP address of localhost, use "0.0.0."
	<PORT>   = The Port # you used to start the server
	<STRING> = The input string that will be sent to the server, then reversed by the server
	
= = = = = = = = = = = = = = = = = = = = = =

Before you can run the next steps. Run the init program with the following command:

$ ./init

* Note: You must do this first to initialize all of the required storage files

Now you may proceed with the other programs.

= = = = = = = = = = = = = = = = = = = = = =
3. Disk Storage Server

To start the disk server, run the following command:

$ ./disk-server

No parameters are required. Note, you must run this program first.


Now connect to the server via the client, by running the following command:

$ ./disk-client

No parameters are required.

= = = = = = = = = = = = = = = = = = = = = =

*** IMPORTANT ***

In order to test number 4 and 5, you must terminate the ./disk-client program FIRST.

Now, the ./disk-server should continue running and should state 

"[ Finding new connection... ]"

If you terminated the ./disk-server program, simply start it again.

Now you must run the following, once your disk-server is running.

$ ./file-sys-server

No parameters required.


Now, you must run this command:

$ ./master-client

No parameters required.


Now you will be in the user interface. Instructions are provided there, along Usage hints.

Here are some commands:

NOTE: ALL directory names must end with a "/"
      ALL files must NOT end with "/"

$ ls

$ pwd

$ cd dirname/       
$ cd dirname/subdir/
$ cd ..
$ cd root/
$ cd ~

$ mkdir dirname/
$ mkdir dirname/subdir/

$ rmdir dirname/
$ rmdir dirname/subdir/

$ touch filename

$ L 0
$ L 1

