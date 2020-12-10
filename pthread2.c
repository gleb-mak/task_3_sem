#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct thread_data
{
	int sum;
	int m;
	int n;
	pthread_mutex_t mutex;
};

void *thread_fun(void *data)
{
	struct thread_data *pdata = (struct thread_data *)(data);
	int m = pdata->m;
	int n = pdata->n;
	if (pthread_mutex_lock(&(pdata->mutex)) != 0)
	{
		perror("mutex loc");
		exit(1);
	}
	for (int i = 0; i < m/n; i++)
	{
		pdata->sum++;
	}
	if (pthread_mutex_unlock(&(pdata->mutex)) != 0)
	{
		perror("mutex unloc");
		exit(1);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	struct thread_data data;
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	data.sum = 0;
	data.m = m;
	data.n = n;
	pthread_t *threadsid = (pthread_t *)(malloc(sizeof(pthread_t) * n));
	if (pthread_mutex_init(&data.mutex, NULL) != 0)
	{
		perror("mutex init");
		exit(1);
	}
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
	if (pthread_mutex_destroy(&data.mutex) != 0)
	{
		perror("mutex destroy");
		exit(1);
	}
	free(threadsid);
	return 0;
}

