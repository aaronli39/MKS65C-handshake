#include <string.h>
#include <signal.h>

#include "pipe_networking.h"

int to_server;
int from_server = -1;

void sighandler(int s)
{
    switch (s)
    {
        case SIGINT:
        remove("publicPipe");
        exit(0);
        case SIGPIPE:
        close(to_server);
        printf("Connection to server ended.\n");
        exit(0);
    }
}

int main()
{
    char buf[256];

    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);

    for (;;)
    {
        while (from_server == -1)
        {
            from_server = client_handshake(&to_server);
            if (from_server == -1)
            {
                printf("Error occurred. Retrying handshake in 3 seconds\n");
                sleep(3);
            }
        }

        memset(buf, 0, 256);
        printf("\n--- Input message(type it and press enter) ---\n");
        read(STDIN_FILENO, buf, 256);
        buf[strlen(buf) - 1] = 0;
        write(to_server, buf, 256);
        read(from_server, buf, 256);
        printf("\n--- Server response ---\n%s\n", buf);
    }
}
