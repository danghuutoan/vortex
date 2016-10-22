#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXBUFLEN 100
int main(void)
{
	// connect to vortex.labs.overthewire.org port 5842
	unsigned char buf[MAXBUFLEN];
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	char  ipstr[INET_ADDRSTRLEN];
	void *addr;
	struct sockaddr_in *ipv4;
	int s;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	int numbytes;
	unsigned int sum = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((status = getaddrinfo("vortex.labs.overthewire.org", "5842", &hints, &servinfo)) != 0)
	{
		printf("error\n");
		exit(1);
	}

	if(servinfo->ai_family == AF_INET)
	{
		printf("IPv4\n");
		ipv4 = (struct sockaddr_in *)servinfo->ai_addr;
		addr = &(ipv4->sin_addr);
		inet_ntop(servinfo->ai_family, addr, ipstr, sizeof ipstr);
		printf("IP:%s", ipstr);
	}
	 
    if ((s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) 
    {
        perror("listener: socket");
    }

    connect(s, servinfo->ai_addr, servinfo->ai_addrlen);
    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");
 	addr_size = sizeof their_addr;

	for (int i = 0; i < 4; ++i)
	{
		if ((numbytes = recvfrom(s, buf, 4 , 0,
        (struct sockaddr *)&their_addr, &addr_size)) == -1)
	    {
	        perror("recvfrom");
	        exit(1);
	    }

		for(int i = 0;i<numbytes; i++)
		{
			sum += (buf[i] << i*8); 
		}
	}
	printf("sum = %u\n", sum);

	numbytes = send(s, &sum, sizeof(unsigned int), 0);

	printf("sent %u bytes\n", numbytes);

	recvfrom(s, buf, MAXBUFLEN -1 , 0, (struct sockaddr *)&their_addr, &addr_size);

	printf("%s\n", buf);
	close(s);
}
