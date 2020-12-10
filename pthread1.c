#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct thread_data
{
	int sum;
	int m;
	int n;
};

void *thread_fun(void *data)
{
	struct thread_data *pdata = (struct thread_data *)(data);
	int m = pdata->m;
	int n = pdata->n;
	for (int i = 0; i < m/n; i++)
	{
		pdata->sum++;
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	struct thread_data data = {0, 0, 0};
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	data.m = m;
	data.n = n;
	pthread_t *threadsid = (pthread_t *)(malloc(sizeof(pthread_t) * n));
	for (int i = 0; i < n; i++)
	{
	    if (pthread_create(&threadsid[i], NULL, thread_fun, &data) != 0)
		{
			perror("thread create");
			exit(1);
		}
	}
	for (int i = 0; i < n; i++)
	{
		pthread_join(threadsid[i], NULL);
	}
	printf("sum: %d\n", data.sum);
	return 0;
}
