#include <stdio.h>
#include <pthread.h>
#include <string.h>


#include "fifo.h"


pthread_t tid;

void *routine(void *arg)
{
	char buf[10];
	int i;

	while(1)
	{
		memset(buf, 0, 10);
		i = FIFO_Read(0, buf, 10);
		
		printf("i = %d, buf = %s\r\n",i, buf);
		
	}
}

int main()
{
	char c[10];

	FIFO_Init();
	
	pthread_create(&tid, NULL, routine, NULL);
	
	while(1)
	{
		scanf("%s", c);
		printf("%lu\r\n", strlen(c));
		
		printf("wlen : %d", FIFO_Write(0, c, strlen(c)));
	}
	
	
	
	
	
}





