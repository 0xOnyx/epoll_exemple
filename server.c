

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <bits/socket.h>
#include <fcntl.h>

#include <sys/epoll.h>


#define MAX_CONN 400
#define MAX_EVENT 5
#define PORT 5454
#define TIMEOUT 400
#define BUFF_SIZE 255

void	ft_fail(int sockfd, int epoll_fd)
{
	close(epoll_fd);
	close(sockfd);

	perror("[ERROR]\tFATAL ERROR ");
	exit(1);
}

int	set_nonblocking(int fd)
{
	if(fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) < 0)
		return -1;
	return 0;
}

void	set_sockaddr(struct sockaddr_in *sockaddr)
{
	memset(sockaddr, 0, sizeof(struct sockaddr_in));
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr->sin_port = htons(PORT);
}

void	epoll_ctl_add(int epoll_fd, int fd, uint32_t events)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = events;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		close(fd);
}

int	main(int argc, char **argv)
{
	int i;
	int yes;
	int sockfd;
	int new_sockfd;
	struct sockaddr_in host_addr;
	struct sockaddr_in client_addr;
	socklen_t sin_size;

	char buf[BUFF_SIZE + 1];

	int epoll_fd;
	int event_count;
	struct epoll_event epoll_events[MAX_EVENT];

	epoll_fd = epoll_create1(0);
	if(epoll_fd < 0)
		ft_fail(0, epoll_fd);

	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		ft_fail(sockfd, epoll_fd);

	set_sockaddr(&host_addr);
	
	yes = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
		ft_fail(sockfd, epoll_fd);

	if(bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) < 0)
		ft_fail(sockfd, epoll_fd);

	set_nonblocking(sockfd);
	if(listen(sockfd, MAX_CONN) < 0)
		ft_fail(sockfd, epoll_fd);

	epoll_ctl_add(epoll_fd, sockfd, EPOLLIN | EPOLLOUT | EPOLLET );

	sin_size = sizeof(struct sockaddr_in);

	while(1)
	{
		event_count = epoll_wait(epoll_fd, epoll_events, MAX_EVENT, TIMEOUT);
		
		i = 0;
		while(i < event_count)
		{
			if(epoll_events[i].data.fd == sockfd)
			{
				new_sockfd = accept(sockfd, \
						(struct sockaddr *)&client_addr, \
						&sin_size);
				set_nonblocking(new_sockfd);

				epoll_ctl_add(epoll_fd, new_sockfd,  \
						EPOLLIN | \
						EPOLLOUT | \
						EPOLLET | \
						EPOLLRDHUP | \
						EPOLLHUP);
				inet_ntop(AF_INET, &(client_addr.sin_addr), buf, sin_size);
				printf("[INFO]\tnew connection %s:%d\n", buf, ntohs(client_addr.sin_port));
				
			}
			else if( epoll_events[i].events & EPOLLIN)
			{
				while(recv(epoll_events[i].data.fd, buf, BUFF_SIZE, 0) > 0)
				{
					printf("[INFO]\tnew data from => %d\n", epoll_events[i].data.fd);
					printf("\t[DATA]\t=>%s\n", buf);
					memset(buf, 0, BUFF_SIZE);
				}
			}
			else
				printf("[ERROR]\tunexpected\n");

			if(epoll_events[i].events & (EPOLLRDHUP | EPOLLHUP))
			{
				printf("[INFO]\tconnection close for => %d\n", epoll_events[i].data.fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL);
				close(epoll_events[i].data.fd);
			}
			i++;	
		}
	}	
	
	close(sockfd);
	close(epoll_fd);

	return (0);
}

