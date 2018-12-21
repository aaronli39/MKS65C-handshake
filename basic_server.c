#include <string.h>
#include <signal.h>
#include "pipe_networking.h"

// got code from joan lol

int to_client;
int from_client = -1;

void sighandler(int s)
{
    switch (s)
    {
        case SIGINT:
        remove("publicPipe");
        exit(0);
        case SIGPIPE:
        close(to_client);
        printf("Client disconnected.\n");
        from_client = -1;
    }
}

int main()
{
    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);
    char buf[256];
    int temp;

    for (;;)
    {
        while (from_client == -1)
        {
            from_client = server_handshake(&to_client);
        }
        memset(buf, 0, 256);
        read(from_client, buf, 256);
        sprintf(buf, "String length: %ld\n", strlen(buf));
        write(to_client, buf, 256);
    }
}
