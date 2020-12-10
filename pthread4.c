#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct plus_data
{
	int add;
	pthread_t threadid;
};

struct thread_data
{
	struct plus_data *sum;
	int m;
	int n;
};

void *thread_fun(void *data)
{
	struct thread_data *pdata = (struct thread_data *)(data);
	int m = pdata->m;
	int n = pdata->n;
	int i = 0;
	pthread_t thisid = pthread_self();
	while (pthread_equal(pdata->sum[i].threadid, thisid) == 0)
	{
		i++;
	}
	if (i > n - 1)
	{
		printf("error\n");
		exit(1);
	}
	for (int j = 0; j < m/n; j++)
	{
		pdata->sum[i].add++;
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int result = 0;
	struct thread_data data;
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	data.sum = (struct plus_data *)(malloc(sizeof(struct plus_data) * n));
	for (int i = 0; i < n; i++)
	{
		data.sum[i].threadid = 0;
		data.sum[i].add = 0;
	}
	data.m = m;
	data.n = n;
	for (int i = 0; i < n; i++)
	{
	    if (pthread_create(&data.sum[i].threadid, NULL, thread_fun, &data) != 0)
		{
			perror("thread create");
			exit(1);
		}
	}
	for (int i = 0; i < n; i++)
	{
		pthread_join(data.sum[i].threadid, NULL);
	}
	for (int i = 0; i < n; i++)
	{
		result += data.sum[i].add;
	}
	printf("sum: %d\n", result);
	free(data.sum);
	return 0;
}
