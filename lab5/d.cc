#include <cstdio>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sys/time.h>
#include <iostream>
#include <atomic>

class worklist_t {
	int*			a;
	size_t			n;
	size_t			total;	// sum a[0]..a[n-1]
	
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
		
public:
	worklist_t(size_t max)
	{
		n = max+1;
		total = 0;

		a = (int*) calloc(n, sizeof(a[0]));
		if (a == NULL) {
			fprintf(stderr, "no memory!\n");
			abort();
		}
	}

	~worklist_t()
	{	
		free(a);
	}
	
	void lock() {
		while(m_flag.test_and_set(std::memory_order_acquire)) {
		}
	}
	
	void unlock() {
		m_flag.clear(std::memory_order_release);
	}

	void reset()
	{
		total = 0;
		memset(a, 0, n*sizeof a[0]);
	}

	void put(int num)
	{			
		lock();
		
		a[num] += 1;
		total += 1;
		
		unlock();
	}

	int get()
	{
		int				i;
		int				num;

		/* hint: if your class has a mutex m
		 * and a condition_variable c, you
		 * can lock it and wait for a number 
		 * (i.e. total > 0) as follows.
		 *
		 */

		//std::unique_lock<std::mutex>	u(m_mutex);

		/* the lambda is a predicate that 
		 * returns false when waiting should 
		 * continue.
		 *
		 * this mechanism will automatically
		 * unlock the mutex m when you return
		 * from this function. this happens when
		 * the destructor of u is called.
		 *
		 */

		//m_cv.wait(u, [this]() { return total > 0; } );
		
		//while(m_flag.test_and_set(std::memory_order_acquire)) {
		//}
		
		lock();
		
		while(total == 0) {
			unlock();
			lock();
		}

		for (i = 1; i <= n; i += 1)
			if (a[i] > 0)
				break;

		if (i <= n) {
			a[i] -= 1;
			total -= 1;
		} else if (a[0] == 0) {
			fprintf(stderr, "corrupt data at line %d!\n", __LINE__);
			abort();
		} else
			i = 0;
			
		//m_flag.clear(std::memory_order_release);
		unlock();

		return i;
	}
};

static worklist_t*		worklist;
static std::atomic<unsigned long long>	sum;
static int			iterations;
static int			max;

static void produce()
{
	int		i;
	int		n;

	for (i = 0; i < iterations; i += 1)
		for (n = 1; n <= max; n += 1)
			worklist->put(n);

	worklist->put(0);
}

static unsigned long long factorial(unsigned long long n)
{
	return n <= 1 ? 1 : n * factorial(n - 1);
}


static void consume()
{
	int			n;
	unsigned long long	f;
	
	while ((n = worklist->get()) > 0) {
		f = factorial(n);
		//sum += f;
		sum.fetch_add(f, std::memory_order_relaxed);
	}
}

static void work()
{
	sum = 0;
	worklist->reset();

	//printf("before starting\n");
	
	std::thread p(produce);
	std::thread a(consume);
	std::thread b(consume);
	std::thread c(consume);
	std::thread d(consume);

	//printf("after threads\n");
	
	p.join();
	//printf("after join p\n");
	a.join();
	//printf("after join a\n");
	b.join();
	//printf("after join b\n");
	c.join();
	//printf("after join c\n");
	d.join();
	
	//printf("after joins\n");
}

static double sec(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

int main(void)
{
	double			begin;
	double			end;
	unsigned long long	correct;
	int			i;
	double average = 0;
	
	printf("mutex/condvar and mutex for sum\n");

	//init_timebase();

	iterations	= 100000;
	max		= 12;
	correct		= 0;

	for (i = 1; i <= max; i += 1)
		correct += factorial(i);
	correct *= iterations;

	worklist = new worklist_t(max);

	for (i = 1; i <= 10; i += 1) {
		//begin = timebase_sec();
		begin = sec();
		work();
		//end = timebase_sec();
		end = sec();

		if (sum != correct) {
			fprintf(stderr, "wrong output!\n");
			abort();
		}

		printf("T = %1.2lf s\n", end - begin);
		average += end - begin;
	}
	
	std::cout << "avg: " << average / 10 << std::endl;

	delete worklist;

	return 0;
}
