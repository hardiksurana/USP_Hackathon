# USP_Hackathon
To create a telnet Application in C using UNIX system calls.

## Goals
1. Create a client server model to facilitate execution of commands on a remote machine.
2. Look for and handle signals, also deliver the appropiate signals to the remote server.
3. Send and receive files.
4. Handle multiple sessions.


# Execution Instructions
```
make all

# run on client system
./client <IP address of server> 8888

# run on server system
./server
```

## References
https://stackoverflow.com/questions/11952898/c-send-and-receive-file
