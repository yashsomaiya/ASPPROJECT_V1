#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAX_LENGTH 1000000
// Function to read the port number from a text file
int read_port_from_file(const char *file_name)
{
    // Open the file in read-only mode
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    // Read the port number from the file
    int port;
    if (fscanf(fp, "%d", &port) != 1)
    {
        fprintf(stderr, "Error reading port from file\n");
        return -1;
    }
    // Close the file
    fclose(fp);

    // Return the port number
    return port;
}

int read_from_count_file(const char *file_name)
{
    // Open the file in read-only mode
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    // Read the count number from the file
    int count;
    if (fscanf(fp, "%d", &count) != 1)
    {
        fprintf(stderr, "Error reading count from file\n");
        return -1;
    }
    // Close the file
    fclose(fp);

    // Return the port number
    return count;
}

void write_port_to_file(const char *file_name, int port_count)
{
    // Open the file in write-only mode
    FILE *fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Write the port number to the file
    fprintf(fp, "%d", port_count);

    // Close the file
    fclose(fp);
}
// Function to connect to a socket on the given port
int connect_to_socket(int port)
{
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return -1;
    }

    // Set the server address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Clear the memory
    serv_addr.sin_family = AF_INET;           // Use the IPv4 address family
    serv_addr.sin_port = htons(port);         // Set the port number

    // Convert the address from a string to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Error converting address to binary");
        return -1;
    }

    // Connect to the socket
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return -1;
    }

    // Return the socket file descriptor
    return sockfd;
}


void main()
{
    int n;
    char buffer[MAX_LENGTH];
    // Read the port number from a file
    int port = read_port_from_file("PortNumber.txt");
    if (port < 0)
    {
        fprintf(stderr, "Connection refused...\n");
        fprintf(stderr, "Check if server is running on the right port and try again...\n");
        exit(3);    }

    // Connect to the socket on the given port
    int sockfd = connect_to_socket(port);
    if (sockfd < 0)
    {
        fprintf(stderr, "Connection refused...\n");
        fprintf(stderr, "Check if server is running on the right port and try again...\n");
        exit(3);
    }

   while (1)
        {
            // prompt
            write(STDOUT_FILENO, "-INPUT COMMAND(s)->", 20);
            n = read(STDIN_FILENO, buffer, MAX_LENGTH);
            buffer[n] = '\0';
            // send commands to the server using space" " to seperate arguments
            write(sockfd, buffer,strlen(buffer) + 1);
            // user types 'quit' to close the connection
             if (!strcasecmp(buffer, "quit\n")) {
                int count= read_from_count_file("count.txt");
                count--;
                write_port_to_file("count.txt",count);
                close(sockfd);
                exit(0);
            } 
            printf("waiting for server...\n");
            do
            {
                if (n = read(sockfd, buffer, MAX_LENGTH))
                {
                    buffer[n] = '\0';
                    write(STDOUT_FILENO, buffer, n + 1);
                    // printf("server returned %s\n", buffer);
                }
            } while (strncmp(buffer, "DONE", 4));
        }
}