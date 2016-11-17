#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX 1024
#define PORT 9999
#define LISTENQ 100

void sys_error(char* str)
{
	printf("%s",str);
	exit(EXIT_FAILURE);
}

int file_size(char* filename)
{
	int n, count = 0;
	char buff[MAX];
	FILE* file_ptr = fopen(filename, "r");
	if( file_ptr==NULL )
		sys_error("[ERROR] open file error\n");
	
	while( n=read(fileno(file_ptr), buff, MAX) )
	{
		if( n<0 )
			sys_error("[ERROR] read error\n");
		count += n;
	}
	fclose(file_ptr);
	return count;
}

void server(int socket_fd)
{
	int n, m, size, i;
	int connect_fd;
	char send[MAX], receive[MAX], buff[MAX];
	char *char_ptr, *filename;
	FILE* file_ptr;
	
	while(1)
	{
		connect_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);
		if( connect_fd<0 )
			continue;
		
		while(1)
		{
			if( (n=read(connect_fd, receive, MAX))==0 )
			{
				printf("client close\n");
				break;
			}
			else
			{
				strcpy(buff, receive);
				char_ptr = strtok(buff, " \n");
				if( strcmp(char_ptr, "GET")==0 )
				{
					
				}
				else if( strcmp(char_ptr, "PUT")==0 )
				{
					char_ptr = strtok(NULL, " \n");
					filename = strtok(NULL, " \n");
					file_ptr = fopen(filename, "w");
					if( file_ptr==NULL )
						sys_error("[ERROR] open file error\n");
					
					i = 0;
					while( read(connect_fd, receive+i, 1) )
					{
						if(receive[i]==' ')
						{
							receive[i] = '\0';
							break;
						}	
						i++;
					}
					size = atoi(receive);
					
					while(size)
					{
						if( size>MAX )
							n = read(connect_fd, receive, MAX);
						else
							n = read(connect_fd, receive, size);
						if( n<0 )
							sys_error("[ERROR] read data from client error\n");
						if( write(fileno(file_ptr), receive, n)<n )
							sys_error("[ERROR] write data to file error\n");
						size -= n;
					}
					fclose(file_ptr);
				}
				else if( strcmp(char_ptr, "LIST")==0 )
				{
					
				}
				else if( strcmp(char_ptr, "EXIT")==0 )
				{
				}
			}
		}
		close(connect_fd);
	}
}

int main(int argc, char** argv)
{
	int socket_fd;
	struct sockaddr_in socket_address;
	
	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))<0)
		sys_error("[ERROR] create socket error\n");
	
	bzero(&socket_address, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	socket_address.sin_port  = htons(PORT);
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if( bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address))<0 )
		sys_error("[ERROR] bind error\n");
	if( listen(socket_fd, LISTENQ)<0 )
		sys_error("[ERROR] listen error\n");
	
	server(socket_fd);
	
	if( close(socket_fd)!=0 )
		sys_error("[ERROR] close socket error\n");
	
	return 0;
}
