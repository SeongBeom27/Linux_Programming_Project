#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Systeminfo.h"

void Printblankline(int n)
{
    for(int i = 0; i < n; i++) printf("\n");
}

void Printinfo(struct systeminfo recvdata)
{
	printf("---------------------------\n");
	Printblankline(1);
	printf("Server Get CPU Data\n");
	printf("Server : %s", recvdata.cpuinfo);
	Printblankline(3);
	printf("Server Get Memory Data\n");
	printf("Server : %s", recvdata.meminfo);
	Printblankline(3);
	printf("Server Get Recently modify file Data\n");
	printf("Server : %s", recvdata.recentlyinfo);
	Printblankline(3);
	printf("Server Get Harddisk Data\n");
	printf("Server : %s", recvdata.harddiskinfo);
	Printblankline(3);
	sleep(2);
}


int main()
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int result;
	int Client_nd = 0;
    fd_set readfds, testfds;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8080);
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    listen(server_sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);

    while(1) {
        struct systeminfo recvdata;
        int fd;
        int nread;
        char confirm[10] = "Disconnect";
        int ch;
        testfds = readfds;

        printf("Server : waiting\n");
        result = select(FD_SETSIZE, &testfds, (fd_set *)0, 
            (fd_set *)0, (struct timeval *) 0);

        if(result < 1) {
            perror("server");
            exit(1);
        }

        for(fd = 0; fd < FD_SETSIZE; fd++) {
            if(FD_ISSET(fd,&testfds)) {
                if(fd == server_sockfd) {
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd, 
                        (struct sockaddr *)&client_address, &client_len);
                    FD_SET(client_sockfd, &readfds);
                    printf("Server : adding client on fd %d\n", client_sockfd);
                }
                else {
                    ioctl(fd, FIONREAD, &nread);

                    if(nread == 0) {
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("Server : removing client on fd %d\n", fd);
                    }
                    else {
						// recv data
                        recv(fd, (struct systeminfo*)&recvdata, sizeof(recvdata), 0);
						// Print get structure data
						Printinfo(recvdata);
						// 'n'st client
						Client_nd++;
                        printf("Server : Get data from %dst client  \n", Client_nd);
						// send confirm data
                        write(fd, confirm , strlen(confirm) + 1);
                    }
                }
            }
        }
    }
}

