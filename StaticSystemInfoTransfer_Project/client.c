#include <stdio.h>
#include <stdlib.h>
#include "Systeminfo.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define DIRPATH "./SystemData/"
#define SIZ 1024

// Error check variable
int ret;

int SendData(const struct systeminfo data)
{
    int sockfd;
    int len;
    char recvdata[10];
    struct sockaddr_in address;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8080);
    len = sizeof(address);

    ret = connect(sockfd, (struct sockaddr *)&address, len);

    if(ret == -1) {
        perror("Server Connect error");
        exit(1);
    }

    if(send(sockfd, (struct systeminfo *)&data, sizeof(data), 0) == -1)
    {
        perror("send error");
        return -1;
    }
    printf("Client : send %ldbyte size data\n", sizeof(data));
    read(sockfd, &recvdata, 10);
    printf("Client : %s\n", recvdata);
    close(sockfd);
    return 1;
}

int main()
{
    // Get system data variable
    struct systeminfo mysystem;
    int size; 
    DIR *dir;
    struct dirent *ent;

    /* Excute shell script to make data file */
    system("./WriteSystemdata.sh");

    /* Open dir */
    dir = opendir(DIRPATH);
    if(dir == NULL)
    {
        perror("Cannot open SystemData Directory.");
        return EXIT_FAILURE;   
    }

    while((ent = readdir(dir)) != NULL)
    {
        // Exception Processing
        if(ent->d_name[0] == '.') continue;

        /* Make Datapath */
        char Datapath[SIZ] = DIRPATH;
        strcat(Datapath, ent->d_name);

        /* File open */
        FILE* fp = fopen(Datapath,"r"); 

        if(fp == NULL)
        {
            perror("Cannot read file");
            exit(1);
        }

        /* Get file size */
        fseek(fp, 0, SEEK_END);         // Moving file pointer to end of file
        size = ftell(fp);               // Get present location of File Pointer
        
        if(size < 0)
        {
            perror("Read file error");
            exit(1);
        }

        fseek(fp, 0, SEEK_SET);         // Moving file pointer to start of file

		if(size > 2048)
			size = 2048;

		/* Buffer Setting */ 

        if(ent->d_name[0] == 'c')
        {   
            // cpu         
            ret = fread(mysystem.cpuinfo, size, 1, fp);
        } else if(ent->d_name[0] == 'm')
        {
            // memory
            ret = fread(mysystem.meminfo, size, 1, fp);
        } else if(ent->d_name[0] == 'r')
        {
            ret = fread(mysystem.recentlyinfo, size, 1, fp);
        }
        else{
            // harddisk
            ret = fread(mysystem.harddiskinfo, size, 1, fp);
        }
        
        /* file close */ 
        fclose(fp);
    }
    // send data
   
    ret = SendData(mysystem);

    if(ret < 0)
    {
        perror("Socket error!");
        exit(1);
    }
    closedir(dir);

    return 0;
}
