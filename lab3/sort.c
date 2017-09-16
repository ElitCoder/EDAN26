#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

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

void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
    if(n <= 1)
        return;
        
    double *p = (double*)base;
    
    int k = n / 2;
    swap(p, p + k);
    
    int key = *p;
    int i = 1;
    int j = n - 1;
    
    while(i <= j) {
        while((i <= n - 1) && (p[i] <= key))
            ++i;
        while((j >= 0) && (p[j] > key))
            ++j;
            
        if(i < j)
            swap(p + i, p + j);
    }
}

static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */
    double a = *(double*)ap;
    double b = *(double*)bp;
    
    if(a > b)
        return -1;
    else if(b > a)
        return 1;

	return 0; 
}

int main(int ac, char** av)
{
	int		n = 4;//2000000;
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
	par_sort(a, n, sizeof a[0], cmp);
#else
	qsort(a, n, sizeof a[0], cmp);
#endif

	end = sec();

	printf("%1.2f s\n", end - start);

	free(a);

	return 0;
}