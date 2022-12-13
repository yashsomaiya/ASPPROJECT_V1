#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// maximum lenth of arguments which should be big enough to make sure the socket never fails of big amount of data
#define MAX_LENGTH 1000000
// maximum lenth of each command and each argument

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

int run(char *buff, int length)
{

    int exitStatus = -1;
    // child Process
    if (fork() == 0)
    {
        // check if command is Valid
        system(buff);
        exit(1);
    }
    else

    { 
        //fprintf(stderr, "Invalid Command detected!\n");
        wait(&exitStatus);
    }
}
//definition of serviceClient
void serviceClient(int s);

int main(int argc, char *argv[])
{
   // write_port_to_file("PortNumber.txt",8080);
    int sd, client, portNumber, status;

    socklen_t len;
    struct sockaddr_in servAdd;

    if (argc != 2)
    {
        printf("Call model: %s <Port:#>\n", argv[0]);
        exit(0);
    }

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Wrong creating new socket");
        exit(1);
    }

    // config the server socket
    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);
    bind(sd, (struct sockaddr *)&servAdd, sizeof(servAdd));
    
    // start listening to the assigned port for at most 5 clients
    listen(sd, 3);
    fprintf(stderr, "waiting for clients to connect \n");
    while (1)
    {
        // wait for clients from any IP address
        client = accept(sd, (struct sockaddr *)NULL, NULL);
        int count= read_from_count_file("count.txt");
        count++;
        write_port_to_file("count.txt",count);
        printf("Client %d connected\n", count);
        printf("Got a new client\n");
        
        //Loadbalancing Logic
        if ((count<5))
        {   
            write_port_to_file("PortNumber.txt",8080);
        }else if ((count<10))
        {
            write_port_to_file("PortNumber.txt",8081);
        }else if((count%2!=0) && count < 15){
            write_port_to_file("PortNumber.txt",8081);
        }else if((count%2==0) && count < 15){
            write_port_to_file("PortNumber.txt",8080);
        }else{
            write_port_to_file("PortNumber.txt",0000);
        }

        // fork a process for each client
        if (fork() == 0)
            serviceClient(client);
    }
}

//Service client method takes socket descriptor 
void serviceClient(int sd)
{
    char message[MAX_LENGTH];
    int n, pid;
    dup2(sd, STDOUT_FILENO); //Writes stdout to socket descriptor
    dup2(sd, STDIN_FILENO);  //Writes stdin to socket descriptor
    dup2(sd, STDERR_FILENO); //Writes stderr to socket descriptor
    while (1)
    {
        n = read(sd, message, MAX_LENGTH);
        message[n] = '\0';
        // printf("MSG");
        run(message, n);
        printf("DONE\n");
    }
}