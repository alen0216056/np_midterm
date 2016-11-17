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

void client(int socket_fd)
{
	int n, m, size;
	int max_fd = (fileno(stdin)>socket_fd)? fileno(stdin)+1: socket_fd+1;
	char send[MAX], receive[MAX], buff[MAX];
	char* char_ptr, *filename;
	FILE* file_ptr;
	fd_set read_set;
	
	while(1)
	{
		FD_SET(fileno(stdin), &read_set);
		FD_SET(socket_fd, &read_set);
		select(max_fd, &read_set, NULL, NULL, NULL);
		
		//client receive from server
		if( FD_ISSET(socket_fd, &read_set) )
		{
			if( (n=read(socket_fd, receive, MAX))==0 )
				sys_error("[ERROR] server terminate\n");
			if( write(fileno(stdout), receive, n)!=n )
				sys_error("[ERROR] write to stdout error\n");
			
		}
		//client send to server
		if( FD_ISSET(fileno(stdin), &read_set) )
		{
			if( (n=read(fileno(stdin), send, MAX))==0 )
				return;
			
			if( write(socket_fd, send, n)!=n )
				sys_error("[ERROR] send command to server error\n");
			
			strcpy(buff, send);
			char_ptr = strtok(buff, " \n");
			if( strcmp(char_ptr, "GET")==0 )
			{
				
			}
			else if( strcmp(char_ptr, "PUT")==0 )
			{
				char_ptr = strtok(NULL, " \n");
				filename = char_ptr;
				char_ptr = strtok(NULL, " \n");
				size = file_size(filename);
				file_ptr = fopen(filename, "r");
				if( file_ptr==NULL )
					sys_error("[ERROR] open file error\n");
				
				snprintf(send, MAX, "%d \0", size);
				write(socket_fd, send, strlen(send));
				while( n=read(fileno(file_ptr), send, MAX) )
				{
					if( n<0 )
						sys_error("[ERROR] read error\n");
					if( write(socket_fd, send, n)<n )
						sys_error("[ERROR] translate file to server error\n");
				}
				printf("PUT %s %s succeeded\n", filename, char_ptr);
			}
			else if( strcmp(char_ptr, "LIST")==0 )
			{
				
			}
			else if( strcmp(char_ptr, "EXIT")==0 )
			{
				return;
			}
			else
			{
				printf("[ERROR] error command\n");
			}
		}
	}
	
}

int main(int argc, char** argv)
{
	int socket_fd;
	struct sockaddr_in socket_address;
	
	if( argc!=2 )
		sys_error("[ERROR] usage:./client <IP>\n");

	if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0))<0 )
		sys_error("[ERROR] create socket error\n");
	
	bzero(&socket_address, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	socket_address.sin_port  = htons(PORT);
	if( inet_pton(AF_INET, argv[1], &socket_address.sin_addr)<0 )
		sys_error("[ERROR] convert IP error\n");
	if( connect(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address))<0 )
		sys_error("[ERROR] connect server error\n");
	
	client(socket_fd);
	
	if( close(socket_fd)!=0 )
		sys_error("[ERROR] close socket error\n");
	
	return 0;
}
