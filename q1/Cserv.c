/*
 A simple echo server program 
 Kasidit Chanchio (kasiditchanchio@gmail.com)
*/
#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP		"127.0.0.1"
#define SERV_PORT 	18800

#define MAXLINE	100

#include <sys/select.h>

int lis_fd;
struct sockaddr_in serv_addr;

#define MAXCONN 100

int main(int argc, char *argv[]){

        int m, n, i, j;
        char line[MAXLINE];
	int conn_fd[MAXCONN]; 
	int cindex = 0; 

	fd_set base_rfds; // base read fd set
	fd_set rfds; // read fd set to be passed as a parameter of select() 
	int fdmax;

	lis_fd = socket(AF_INET, SOCK_STREAM, 0); 

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);

	serv_addr.sin_addr.s_addr = INADDR_ANY;

	bind(lis_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); 

	listen(lis_fd, 5);

	FD_ZERO(&base_rfds);
	FD_ZERO(&rfds);

	FD_SET(lis_fd, &base_rfds);
	fdmax = lis_fd;

	while(1){
	  memcpy(&rfds, &base_rfds, sizeof(fd_set));
	  if(select(fdmax+1, &rfds, NULL, NULL, NULL) < 0){
	    printf("select error!\n");
	    fflush(stdout);
	    exit(1);
	  }

	  for(i = 0; i <= fdmax; i++){
	    if(FD_ISSET(i, &rfds)){
		if(i == lis_fd){
		  if((conn_fd[cindex] = accept(lis_fd, NULL, NULL)) < 0){
		    printf("Accept: Error occured\n");
		    exit(1);
		  }

	          printf("a new connection %d is made!\n", conn_fd[cindex]);
		  FD_SET(conn_fd[cindex] , &base_rfds);
		  if(conn_fd[cindex] > fdmax){
		    fdmax = conn_fd[cindex];
		  }
		  cindex++;
		}
		else{
		  n = read(i, line, MAXLINE);
		  if (n <= 0){
		    if(n == 0){
		        printf("read: close connection %d\n", i);
			FD_CLR(i, &base_rfds);
			close(i);

			conn_fd[j] = -1;
		    }
		    else{
		        printf("read: Error occured\n");
		        exit(1);
		    }
		  }
		  else{
		    char str[150];
		    sprintf(str, "\ncli-%03d says: %s", atoi(line), &line[4]);
	 	    for(j =0; j < cindex; j++){
		      if(conn_fd[j] != -1 && conn_fd[j] != i){
                        m = write(conn_fd[j], str, n);
		      }
		    }
                    printf("%s", &str[1]);
                    fflush(stdout);
        	  }
		}
	    }
	  }
	}
}
