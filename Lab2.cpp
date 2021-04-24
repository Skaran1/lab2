#include <pthread.h>
#include <math.h>
#include <cstdio>
#include <iostream>

#define NUM_THREADS 318
typedef struct _thread_data_t {
	int tid;
	int stuff;
} thread_data_t;

double shared_summ;
pthread_mutex_t lock_x;


//globals
double initial_limit_a = 0.001;
double initial_limit_b = 1.0;
double critical_segment = 0.010025;
int divide_num = 1000000;


void *thr_func(void *arg);
double function(double x);


int main(int argc, char **argv)
{
	pthread_t thr[NUM_THREADS];
	int i, rc;
	thread_data_t thr_data[NUM_THREADS];
	shared_summ = 0;
	pthread_mutex_init(&lock_x, NULL);

	for(i = 0; i < NUM_THREADS; i++)
	{
		thr_data[i].tid = i;
		thr_data[i].stuff = i;
		if((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i])))
		{
			printf("error: pthread_create, rc: %d\n", rc);
			return EXIT_FAILURE;
		}
 	}

	for(i = 0; i< NUM_THREADS; ++i)
	{
		pthread_join(thr[i], NULL);
	}

	std::cout << "Integral of sin(1/x) from 0.001 to 1 with divide_num " << divide_num << " is equal ";
	std::cout.precision(10);
	std::cout << shared_summ << std::endl;

	return EXIT_SUCCESS;
}

void *thr_func(void *arg)
{
	thread_data_t *data = (thread_data_t *)arg;
	double limit_a = 1 / M_PI / (data -> stuff + 1);
	double limit_b;
	double summ = 0;

	if (limit_a < initial_limit_a)
			limit_a = initial_limit_a;
	if (data -> stuff == 0)
		limit_b = initial_limit_b;
	else
		limit_b =  1 / M_PI / (data -> stuff);

	for(int i = 0; i < divide_num; i++)
		summ += (limit_b - limit_a) / divide_num * function((limit_b - limit_a) / divide_num * i + limit_a);

	pthread_mutex_lock(&lock_x);
	shared_summ += summ;
	pthread_mutex_unlock(&lock_x);
	pthread_exit(NULL);
}


double function(double x)
{
	return sin(1 / x);
}
