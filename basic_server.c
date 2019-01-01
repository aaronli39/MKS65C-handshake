#include <string.h>
#include <signal.h>
#include "pipe_networking.h"

// got code from joan lol

int to_client;
int from_client = -1;

// handle the keyboard interupts
void sighandler(int s)
{
    switch (s)
    {
        case SIGINT:
            remove("publicPipe");
            // exit after
            exit(0);
        case SIGPIPE:
            close(to_client);
            printf("A client has been disconnected\n");
            exit(0);
    }
}

void process_input()
{
    char buf[256];
    for (;;)
    {
        memset(buf, 0, 256);
        read(from_client, buf, 256);
        sprintf(buf, "Server response: length of string is %ld\n", strlen(buf));
        write(to_client, buf, 256);
    }
}

int main()
{
    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);
    int pid;

    for (;;)
    {
        while (from_client == -1)
        {
            from_client = server_handshake(&to_client);
        }
        pid = fork();
        if (pid == 0)
        {
            // child
            process_input();
        } else
        {
            // parent
            from_client = -1;
        }
    }
}
