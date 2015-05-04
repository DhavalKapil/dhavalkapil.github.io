#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main()
{	char file_name[50];
	char buf[1025];
	int fd;
	
	printf("Enter filename:\n");
	fflush(stdout);
	scanf("%s", file_name);
	
	fd = open(file_name, O_RDONLY);
	if(fd==-1)
	{	printf("Error: %d\n", errno);
		return -1;
	}
	if(read(fd, buf, sizeof(buf))<0)
	{	printf("Error: %d\n", errno);
		close(fd);
		return -1;
	}

	printf("%s\n", buf);
	close(fd);
	return 0;
}
