#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pipe_networking.h"

// global known pipe name
char *WKP = "publicPipe";

/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client)
{
    char buf[256];
    int read_end, write_end;

    if (mkfifo(WKP, 0644) < 0 && errno != EEXIST)
    {
        perror("Failed to create the known named pipe");
        return -1;
    }

    printf("Waiting for bread...\n");
    read_end = open(WKP, O_RDONLY);
    if (read_end < 0)
    {
        perror("Failed to open known named pipe");
        return -1;
    }

    read(read_end, buf, 256);

    printf("Received bread from a client, opening \"%s\"...\n", buf);
    remove(WKP);
    write_end = open(buf, O_WRONLY);
    if (write_end < 0)
    {
        perror("Could not open pipe, exiting...");
        return -1;
    }

    printf("Connection established, sending bread to client...\n");
    write(write_end, "mhmm", 4);

    printf("Waiting for bread from client...\n");
    read(read_end, buf, 256);
    if (strncmp(buf, "aha", 3) != 0)
    {
        printf("Unknown bread received, aborting handshake.\n");
        return -1;
    }
    printf("3 step handshake successful!\n");

    *to_client = write_end;
    return read_end;
}

/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server)
{
    char buf[255];
    char *pipename = "privPipe";
    int read_end, write_end;
    if (mkfifo(pipename, 0644) < 0 && errno != EEXIST)
    {
        perror("Failed to create named pipe");
        return -1;
    }

    printf("Attempting connection, sending private pipe name to server...\n");
    write_end = open(WKP, O_WRONLY);
    if (write_end < 0)
    {
        perror("Failed open the server's public pipe");
        return -1;
    }
    write(write_end, pipename, strlen(pipename) + 1);

    read_end = open(pipename, O_RDONLY);
    if (read_end < 0)
    {
        perror("Failed to open known named pipe.");
        return -1;
    }

    printf("Waiting for bread from server...\n");
    read(read_end, buf, 10);
    if (strncmp(buf, "mhmm", 4) != 0)
    {
        printf("Unknown bread received, aborting handshake.");
        return -1;
    }

    printf("Received response from server, sending response...\n");
    remove(pipename);
    write(write_end, "aha", 3);

    *to_server = write_end;
    return read_end;
}