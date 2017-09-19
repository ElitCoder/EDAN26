#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct {
	void *base;
	size_t n;
	int threads_left;
} thread_args_t;

void par_sort(void*, size_t, int);

static double sec(void)
{
	struct timeval now;
    gettimeofday(&now, NULL);
    
    return now.tv_sec;
}

void swap(double *a, double *b)
{
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double *a, int s) {
	int p = a[s - 1];
	int i = -1;
	
	for(int j = 0; j <= s - 2; j++) {
		if(a[j] <= p) {
			i++;
			swap(&a[i], &a[j]);
		}
	}
	
	swap(&a[i + 1], &a[s - 1]);
		
	return i + 1;
}

void run_qsort(void *args)
{
	thread_args_t *t_arg = (thread_args_t*)args;
	
	par_sort(t_arg->base, t_arg->n, t_arg->threads_left);
}

void par_sort(void *base, size_t n, int threads_left)
{
	if(n <= 1)
		return;
		
	double *a = (double*)base;
	int j = partition(a, n);
	
	if(threads_left > 0) {		
		thread_args_t t_arg;
		
		if(j > n - j) {
			t_arg.base = base;
			t_arg.n = j;
			
		} else {
			t_arg.base = (((double*)base) + j + 1);
			t_arg.n = n - j - 1;
		}
		
		t_arg.threads_left = threads_left - 1;
		
		pthread_t thread;
		pthread_create(&thread, NULL, (void*)run_qsort, &t_arg);
		
		if(j > n - j) {
			par_sort(a + j + 1, n - j - 1, 0);
		} else {
			par_sort(a, j, 0);
		}
		
		pthread_join(thread, NULL);
	} else {
		par_sort(a, j, 0);
		par_sort(a + j + 1, n - j - 1, 0);
	}
}


static int cmp(const void* ap, const void* bp)
{	
    double a = *(double*)ap;
    double b = *(double*)bp;
    
	return a < b;
}

int main(int ac, char** av)
{
	int		n = 40000000;
	int		i;
	double*		a;
	double		start, end;

	if (ac > 1)
		sscanf(av[1], "%d", &n);

	srand(getpid());

	a = malloc(n * sizeof a[0]);
	for (i = 0; i < n; i++)
		a[i] = rand();

	start = sec();

#ifdef PARALLEL
	printf("parallel\n");
	par_sort(a, n, 128);
#else
	qsort(a, n, sizeof a[0], cmp);
#endif
	end = sec();
	
	printf("%1.2f s\n", end - start);

	free(a);

	return 0;
}