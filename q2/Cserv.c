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
#define MAXCONN 254
#define MAXHISTORY 10

#define EMPTY	-1
#define SAME	-1

#include <sys/select.h>
#include "circularLinkedList.h"

int lis_fd;
int conn_fd[MAXCONN], conn_id[MAXCONN];
struct sockaddr_in serv_addr;

int main(int argc, char *argv[]){
	int n, i, j, cindex = 0, numOfHis = 0;
	char line[MAXLINE];

	fd_set base_rfds; // base read fd set
	fd_set rfds; // read fd set to be passed as a parameter of select()
	int fdmax;

	for (i = 0; i < MAXCONN; i++) conn_fd[i] = conn_id[i] = EMPTY;

	lis_fd = socket(AF_INET, SOCK_STREAM, 0); // create socket
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY; // define IP and Port number of socket

	bind(lis_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); //bind IP and Port number to socket

	listen(lis_fd, 5); //change to passive socket to wait for connection

	FD_ZERO(&base_rfds); //set base_rfds is empty set
	FD_ZERO(&rfds); //set rfds is empty set
	FD_SET(lis_fd, &base_rfds); //add value of lis_fd into base_rfds
	fdmax = lis_fd;

	while(1){
    	    memcpy(&rfds, &base_rfds, sizeof(fd_set)); //copy base_rfds to rfds
	    if (select(fdmax+1, &rfds, NULL, NULL, NULL) < 0) { //select block until something happend with fd
		printf("select error!\n");
		exit(1);
	    }
	    for (i = 0; i <= fdmax; i++) {
		if (FD_ISSET(i, &rfds)) { //check if there is i in set of rfds
		    if (i == lis_fd) { //if i is a listening_fd
			for (j = 0; j < MAXCONN; j++) {
			    if (conn_fd[j] == EMPTY) { //if conn_fd[j] is not connected
				cindex = j; break; //let cindex = j then end of loop
			    }
			}
			if ((conn_fd[cindex] = accept(lis_fd, NULL, NULL)) < 0) { //use accept() to create connection, if no connection
				printf("Accept: Error occured\n"); //show error message
				exit(1);
			}
          		printf("a new connection %d are wating id ...\n", conn_fd[cindex]); //if it connected, show message a new connection
          		FD_SET(conn_fd[cindex] , &base_rfds); //add value of conn_fd[cindex] into base_rfds
          		if(conn_fd[cindex] > fdmax){
		       	    fdmax = conn_fd[cindex];
			}
	      	    } else {
			for (j = 0; j < MAXCONN; j++) {
			    if (conn_fd[j] == i) {
				cindex = j; break;
			    }
			}
          		if (conn_id[cindex] == EMPTY) { //if client don't have ID, It will assign ID to client
			    int tmp_id;
			    read(i, &tmp_id, sizeof(int)); //read message(id) from client and copy to tmp_id
			    for (j = 0; j < MAXCONN; j++) {
				if (conn_id[j] == tmp_id) {
				    tmp_id = SAME; //let tmp_id = SAME ("SAME" mean that ID from client is a duplicate ID and close connection)
				    break; //end of loop
				}
			    }
			    if (tmp_id == SAME) { //if ID from client is a duplicate ID
				n = 0;
			    } else {
			    	conn_id[cindex] = tmp_id;
			    	printf("fd(%d) receive cli-%03d\n", conn_fd[cindex], conn_id[cindex]);
			    	continue; //start new loop
			    }
			} else {
	  		    n = read(i, line, MAXLINE); //read message from client and copy to line
			}
			if (n <= 0) {
			    if (n == 0) { //client close connection
				printf("read: close connection %d\n", i);
				FD_CLR(i, &base_rfds); //clear i from base_rfds
				close(i); //close file description i
    				conn_fd[cindex] = conn_id[cindex] = EMPTY;
		            } else {
				printf("read: Error occured\n");
 				exit(1);
		            }
		    	} else {
		            char str[116];
		 	    sprintf(str, "\ncli-%03d says: %s", conn_id[cindex], line);
			    for(j = 0; j < MAXCONN; j++) {
				if (conn_fd[j] != EMPTY && conn_fd[j] != i) {
				    write(conn_fd[j], str, n); //send message to client
				}
			    }
			    addAtLast(conn_id[cindex], strlen(line), line);
			    if (numOfHis < MAXHISTORY) {
				numOfHis++;
			    } else {
				deleteFirst();
			    }	
			    viewList();
         		}
		    }
		}
	    }
	}
}
