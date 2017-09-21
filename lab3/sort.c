#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
	void *base;
	size_t n;
	int threads_left;
} thread_args_t;

void par_sort(void*, size_t, int, bool);

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
	
	par_sort(t_arg->base, t_arg->n, t_arg->threads_left, true);
}

/*
void add_sorted(int n, int id)
{
	printf("accessing %d\n", id);
	
	pthread_mutex_lock(&g_mutex[id]);
	if(!g_written[id])
		g_sorted[id] = n;
	pthread_mutex_unlock(&g_mutex[id]);
}
*/

void par_sort(void *base, size_t n, int threads_left, bool print)
{
	if(n <= 1)
		return;
		
	double *a = (double*)base;
	int j = partition(a, n);
	
	if(threads_left > 0) {		
		thread_args_t t_arg;
		
		if(j > (int)n - j) {
			t_arg.base = base;
			t_arg.n = j;
			
		} else {
			t_arg.base = (((double*)base) + j + 1);
			t_arg.n = n - j - 1;
		}
		
		t_arg.threads_left = threads_left - 1;
		
		pthread_t thread;
		pthread_create(&thread, NULL, (void*)run_qsort, &t_arg);
		
		if(j > (int)n - j) {
			if(print)
				printf("thread doing %zu work\n", n - j - 1 + 1);
				
			par_sort(a + j + 1, n - j - 1, 0, false);
		} else {
			if(print)
				printf("thread doing %d work\n", j + 1);
				
			par_sort(a, j, 0, false);
		}
				
		pthread_join(thread, NULL);
	} else {
		if(print)
			printf("thread doing %zu work\n", n);
			
		par_sort(a, j, 0, false);
		par_sort(a + j + 1, n - j - 1, 0, false);
	}
}

static int cmp(const void* ap, const void* bp)
{	
    double a = *(double*)ap;
    double b = *(double*)bp;
    
	return a > b;
}

int is_sorted(const double *a, int n)
{
	if(n <= 1)
		return 1;
		
	int i;
	
	for(i = 1; i < n; i++)
		if(a[i - 1] > a[i])
			return 0;
			
	return 1;
}

int main(int ac, char** av)
{
	int		n = 100000000;
	int		i;
	int		threads = 4;
	double*		a;
	double		start, end;
	clock_t cstart, cend;

	if (ac > 1)
		sscanf(av[1], "%d", &n);

	srand(getpid());

	a = malloc(n * sizeof a[0]);
	for (i = 0; i < n; i++)
		a[i] = rand();

	start = sec();
	cstart = clock();

#ifdef PARALLEL
	printf("parallel\n");
	par_sort(a, n, threads - 1, true);
#else
	qsort(a, n, sizeof a[0], cmp);
#endif

 	end = sec();
	cend = clock();
	
	assert(is_sorted(a, n));
	
	/*
	double cpu_time_used;

	start = clock();
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	*/
	
	printf("time: %1.2f s\n", ((double)(cend - cstart)) / CLOCKS_PER_SEC);
	
	/*
	int sum = 0;
	
	for(i = 0; i < threads; i++) {
		printf("thread %d did %d elements\n", i, g_sorted[i]);
		sum += g_sorted[i];
	}
	
	printf("in total sorted %d elements\n", sum);
	*/
	
	printf("%1.2f s\n", end - start);

	free(a);

	return 0;
}