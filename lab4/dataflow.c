#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "dataflow.h"
#include "error.h"
#include "list.h"
#include "set.h"

typedef struct vertex_t			vertex_t;
typedef struct task_t			task_t;
typedef struct liveness_args_t	liveness_args_t;

/* cfg_t: a control flow graph. */
struct cfg_t {
	size_t			nvertex;	/* number of vertices		*/
	size_t			nsymbol;	/* width of bitvectors		*/
	vertex_t*		vertex;		/* array of vertex		*/
};

/* vertex_t: a control flow graph vertex. */
struct vertex_t {
	size_t			index;		/* can be used for debugging	*/
	set_t*			set[NSETS];	/* live in from this vertex	*/
	set_t*			prev;		/* alternating with set[IN]	*/
	size_t			nsucc;		/* number of successor vertices */
	vertex_t**		succ;		/* successor vertices 		*/
	list_t*			pred;		/* predecessor vertices		*/
	bool			listed;		/* on worklist			*/
	pthread_mutex_t	mutex;		/* for parallelism */
	pthread_mutex_t in_mutex;	/* avoid deadlock by only locking for in */
	pthread_mutex_t listed_mutex/* avoid deadlock by only locking for changing listed */
};

/* liveness_args_t: args for parallel liveness */
struct liveness_args_t {
	cfg_t*				cfg;
	size_t				low;
	size_t				high;
	pthread_barrier_t*	barrier;
};

static void clean_vertex(vertex_t* v);
static void init_vertex(vertex_t* v, size_t index, size_t nsymbol, size_t max_succ);

cfg_t* new_cfg(size_t nvertex, size_t nsymbol, size_t max_succ)
{
	size_t		i;
	cfg_t*		cfg;

	cfg = calloc(1, sizeof(cfg_t));
	if (cfg == NULL)
		error("out of memory");

	cfg->nvertex = nvertex;
	cfg->nsymbol = nsymbol;

	cfg->vertex = calloc(nvertex, sizeof(vertex_t));
	if (cfg->vertex == NULL)
		error("out of memory");

	for (i = 0; i < nvertex; i += 1)
		init_vertex(&cfg->vertex[i], i, nsymbol, max_succ);

	return cfg;
}

static void clean_vertex(vertex_t* v)
{
	int		i;

	for (i = 0; i < NSETS; i += 1)
		free_set(v->set[i]);
	free_set(v->prev);
	free(v->succ);
	free_list(&v->pred);
}

static void init_vertex(vertex_t* v, size_t index, size_t nsymbol, size_t max_succ)
{
	int		i;

	v->index	= index;
	v->succ		= calloc(max_succ, sizeof(vertex_t*));

	if (v->succ == NULL)
		error("out of memory");
	
	for (i = 0; i < NSETS; i += 1)
		v->set[i] = new_set(nsymbol);

	v->prev = new_set(nsymbol);
}

void free_cfg(cfg_t* cfg)
{
	size_t		i;

	for (i = 0; i < cfg->nvertex; i += 1)
		clean_vertex(&cfg->vertex[i]);
	free(cfg->vertex);
	free(cfg);
}

void connect(cfg_t* cfg, size_t pred, size_t succ)
{
	vertex_t*	u;
	vertex_t*	v;

	u = &cfg->vertex[pred];
	v = &cfg->vertex[succ];

	u->succ[u->nsucc++ ] = v;
	insert_last(&v->pred, u);
}

bool testbit(cfg_t* cfg, size_t v, set_type_t type, size_t index)
{
	return test(cfg->vertex[v].set[type], index);
}

void setbit(cfg_t* cfg, size_t v, set_type_t type, size_t index)
{
	set(cfg->vertex[v].set[type], index);
}

void* liveness_parallel(void *args)
{
	liveness_args_t*	arguments;
	size_t				i;
	size_t				j;
	set_t*				prev;
	vertex_t*			u;
	vertex_t*			v;
	list_t*				worklist;
	list_t*				h;
	list_t*				p;
	bool				changed;
	
	arguments = (liveness_args_t*)args;
	worklist = NULL;
	
	for(i = arguments->low; i < arguments->high; ++i) {
		u = &arguments->cfg->vertex[i];
		insert_last(&worklist, u);
		
		u->listed = true;
		
		pthread_mutex_init(&u->mutex, NULL);
		pthread_mutex_init(&u->in_mutex, NULL);
		pthread_mutex_init(&u->listed_mutex, NULL);
	}
	
	pthread_barrier_wait(arguments->barrier);
	
	while((u = remove_first(&worklist)) != NULL) {
		pthread_mutex_lock(&u->mutex);
		
		pthread_mutex_lock(&u->listed_mutex);
		u->listed = false;
		pthread_mutex_unlock(&u->listed_mutex);

		reset(u->set[OUT]);

		for(j = 0; j < u->nsucc; ++j) {
			pthread_mutex_lock(&u->succ[j]->in_mutex);
			or(u->set[OUT], u->set[OUT], u->succ[j]->set[IN]);
			pthread_mutex_unlock(&u->succ[j]->in_mutex);
		}

		prev = u->prev;
		
		pthread_mutex_lock(&u->in_mutex);
		
		u->prev = u->set[IN];
		u->set[IN] = prev;

		propagate(u->set[IN], u->set[OUT], u->set[DEF], u->set[USE]);
		
		changed = u->pred != NULL && !equal(u->prev, u->set[IN]);
		
		pthread_mutex_unlock(&u->in_mutex);

		if(changed) {
			p = h = u->pred;
			do {
				v = p->data;
				
				pthread_mutex_lock(&v->listed_mutex);
				
				if(!v->listed) {
					v->listed = true;
					insert_last(&worklist, v);
				}
				
				pthread_mutex_unlock(&v->listed_mutex);
				
				p = p->succ;

			} while (p != h);
		}
		
		pthread_mutex_unlock(&u->mutex);
	}
	
	return NULL;
}

void liveness(cfg_t* cfg)
{
	size_t 				nthreads = 2;
	size_t				i;
	liveness_args_t 	args[nthreads];
	pthread_t			threads[nthreads];
	pthread_barrier_t	barrier;
	
	pthread_barrier_init(&barrier, NULL, nthreads);
	
	for(i = 0; i < nthreads; ++i) {
		args[i].low = i * (cfg->nvertex / nthreads);
		args[i].high = (i + 1) * (cfg->nvertex / nthreads);
		
		args[i].cfg = cfg;
		args[i].barrier = &barrier;
		
		printf("thread %zu gets vertex %zu to %zu\n", i, args[i].low, args[i].high);
				
		pthread_create(&threads[i], NULL, liveness_parallel, &args[i]);
	}
	
	for(i = 0; i < nthreads; ++i)
		pthread_join(threads[i], NULL);
		
	pthread_barrier_destroy(&barrier);
}

void print_sets(cfg_t* cfg, FILE *fp)
{
	size_t		i;
	vertex_t*	u;

	for (i = 0; i < cfg->nvertex; ++i) {
		u = &cfg->vertex[i]; 
		fprintf(fp, "use[%zu] = ", u->index);
		print_set(u->set[USE], fp);
		fprintf(fp, "def[%zu] = ", u->index);
		print_set(u->set[DEF], fp);
		fputc('\n', fp);
		fprintf(fp, "in[%zu] = ", u->index);
		print_set(u->set[IN], fp);
		fprintf(fp, "out[%zu] = ", u->index);
		print_set(u->set[OUT], fp);
		fputc('\n', fp);
	}
}