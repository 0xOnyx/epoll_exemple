#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>

#define MAX_EVENT 10
#define TIMEOUT 30000
#define BUFF_SIZE 10

void	ft_fail(int fd)
{	
	puts("[ERROR]\terror\n");
	close(fd);
	exit(1);
}


int	main(int argc, char **argv)
{
	int fd = 0;
	int epoll_fd = epoll_create1(0);
	int event_count = 0;

	struct epoll_event event;
	struct epoll_event events[MAX_EVENT];
	
	if(epoll_fd < 0)
		ft_fail(epoll_fd);

	event.events = EPOLLIN;
	event.data.fd = fd;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
		ft_fail(epoll_fd);

	int running = 1;
	int i = 0;
	size_t bytes_read;
	char buffer_read[BUFF_SIZE + 1];
	while(running)
	{
		puts("[INFO]\tPolling\n");
		event_count = epoll_wait(epoll_fd, events, MAX_EVENT, TIMEOUT);
		printf("[INFO]\t %d ready to read\n", event_count);

		while(i < event_count)
		{
			printf("[INFO]\t Read from file => %d\n", events[i].data.fd);
			bytes_read = read(events[i].data.fd, buffer_read, BUFF_SIZE);
			printf("[INFO]\t %zd read bytes\n", bytes_read);
			buffer_read[bytes_read] = '\0';
			
			printf("[INFO]\t Read => %s\n", buffer_read);

			if(!strncmp(buffer_read, "stop\n", 5))
				running = 0;

			i++;
		}
		i = 0;

	}


	close(epoll_fd);
	return (0);
}
