/*
    Variable length packet Client Program
*/
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

int SendData(const struct systeminfo data, size_t structlen)
{
    int sockfd;
    int len;
    char recvdata[10];
    struct sockaddr_in address;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8090);
    len = sizeof(address);
    
    printf("Before Send Client : %s\n", data.cpuinfo);
    ret = connect(sockfd, (struct sockaddr *)&address, len);

    if(ret == -1) {
        perror("Server Connect error");
        exit(1);
    }
//    write(sockfd, &ch, 1);
//    read(sockfd, &ch, 1);
//    printf("Recieve : %c\n", ch);
    
    if(send(sockfd, (struct systeminfo *)&data, structlen, 0) == -1)
    {
        perror("send error");
        return -1;
    }
    printf("Client : send data size %ld\n", structlen);
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


        /* Buffer Setting */ 

        if(ent->d_name[0] == 'c')
        {   
            // cpu         
            mysystem.cpuinfo = (char *)calloc(size + 1, sizeof(char));;
            ret = fread(mysystem.cpuinfo, size, 1, fp);
        } else if(ent->d_name[0] == 'm')
        {
            // memory
            mysystem.meminfo = (char *)calloc(size + 1, sizeof(char));;
            ret = fread(mysystem.meminfo, size, 1, fp);
        } else{
            // harddisk
            mysystem.harddiskinfo = (char *)calloc(size + 1, sizeof(char));;
            ret = fread(mysystem.harddiskinfo, size, 1, fp);
        }
        
        /* file close */ 
        fclose(fp);
    }
    // send data
    size_t structlen = (strlen(mysystem.cpuinfo) + 1) + (strlen(mysystem.meminfo) + 1) + (strlen(mysystem.harddiskinfo) + 1);
    ret = SendData(mysystem,structlen);

    if(ret < 0)
    {
        perror("Socket error!");
        free(mysystem.cpuinfo);
        free(mysystem.meminfo);
        free(mysystem.harddiskinfo);
        exit(1);
    }
    free(mysystem.cpuinfo);
    free(mysystem.meminfo);
    free(mysystem.harddiskinfo);
    closedir(dir);

    return 0;
}
