import java.util.Iterator;
import java.util.ListIterator;
import java.util.LinkedList;
import java.util.BitSet;
import java.util.List;
import java.util.ArrayList;

class Random {
	int	w;
	int	z;
	
	public Random(int seed)
	{
		w = seed + 1;
		z = seed * seed + seed + 2;
	}

	int nextInt()
	{
		z = 36969 * (z & 65535) + (z >> 16);
		w = 18000 * (w & 65535) + (w >> 16);

		return (z << 16) + w;
	}
}

class Vertex {
	int					index;
	boolean				listed;
	LinkedList<Vertex>	pred;
	LinkedList<Vertex>	succ;
	BitSet				in;
	BitSet				out;
	BitSet				use;
	BitSet				def;

	Vertex(int i)
	{
		index	= i;
		pred	= new LinkedList<Vertex>();
		succ	= new LinkedList<Vertex>();
		in	= new BitSet();
		out	= new BitSet();
		use	= new BitSet();
		def	= new BitSet();
	}
    
    public synchronized BitSet getIn() {
        return in;
    }
    
    public synchronized void setListed(boolean status) {
        listed = status;
    }
    
    public synchronized boolean isListed() {
        return listed;
    }

	void computeIn(LinkedList<Vertex> worklist)
	{
		int                   i;
		BitSet                old;
		Vertex                v;
		ListIterator<Vertex>  iter;

		iter = succ.listIterator();

		while (iter.hasNext()) {
			v = iter.next();

			synchronized(this) {
				out.or(v.getIn());
			}
		}
        
        // in = use U (out - def)

        synchronized(this) {
            old = in;
    		
    		in = new BitSet();
    		in.or(out);	
    		in.andNot(def);	
    		in.or(use);
        }
        
        boolean different;
        
        synchronized(this) {
            different = !in.equals(old);
        }

        if(different) {
			iter = pred.listIterator();

			while (iter.hasNext()) {
				v = iter.next();

                if(!v.isListed()) {
                    worklist.addLast(v);
                    v.setListed(true);
                }
			}
		}
	}

	public void print()
	{
		int	i;

		System.out.print("use[" + index + "] = { ");
		for (i = 0; i < use.size(); ++i)
			if (use.get(i))
				System.out.print("" + i + " ");
		System.out.println("}");
		System.out.print("def[" + index + "] = { ");
		for (i = 0; i < def.size(); ++i)
			if (def.get(i))
				System.out.print("" + i + " ");
		System.out.println("}\n");

		System.out.print("in[" + index + "] = { ");
		for (i = 0; i < in.size(); ++i)
			if (in.get(i))
				System.out.print("" + i + " ");
		System.out.println("}");

		System.out.print("out[" + index + "] = { ");
		for (i = 0; i < out.size(); ++i)
			if (out.get(i))
				System.out.print("" + i + " ");
		System.out.println("}\n");
	}
}

class WorkerThread extends Thread {
	private int id, processed;
	private LinkedList<Vertex> worklist;
	
	public WorkerThread(int id, LinkedList<Vertex> worklist) {
		this.id = id;
		this.worklist = worklist;
	}
	
	public void run() {
		while(!worklist.isEmpty()) {
			Vertex u = worklist.remove();
			
			u.setListed(false);
			u.computeIn(worklist);
			
			++processed;
		}
		
		System.out.println("Thread " + id + " processed " + processed + " vertices");
	}
}

class Dataflow {
	public static void connect(Vertex pred, Vertex succ)
	{
		pred.succ.addLast(succ);
		succ.pred.addLast(pred);
	}

	public static void generateCFG(Vertex vertex[], int maxsucc, Random r)
	{
		int	i;
		int	j;
		int	k;
		int	s;	// number of successors of a vertex.

		System.out.println("generating CFG...");

		connect(vertex[0], vertex[1]);
		connect(vertex[0], vertex[2]);
		
		for (i = 2; i < vertex.length; ++i) {
			s = (r.nextInt() % maxsucc) + 1;
			for (j = 0; j < s; ++j) {
				k = Math.abs(r.nextInt()) % vertex.length;
				connect(vertex[i], vertex[k]);
			}
		}
	}

	public static void generateUseDef(	
		Vertex	vertex[],
		int	nsym,
		int	nactive,
		Random	r)
	{
		int	i;
		int	j;
		int	sym;

		System.out.println("generating usedefs...");

		for (i = 0; i < vertex.length; ++i) {
			for (j = 0; j < nactive; ++j) {
				sym = Math.abs(r.nextInt()) % nsym;

				if (j % 4 != 0) {
					if (!vertex[i].def.get(sym))
						vertex[i].use.set(sym);
				} else {
					if (!vertex[i].use.get(sym))
						vertex[i].def.set(sym);
				}
			}
		}
	}

	public static void liveness(Vertex vertex[], int nthreads)
	{
		Vertex	u;
		Vertex	v;
		int		i;
		long	begin;
		long	end;
		
		List<LinkedList<Vertex>> worklist = new ArrayList<LinkedList<Vertex>>();

		System.out.println("computing liveness...");

		begin = System.nanoTime();
		
		if(vertex.length < 2000) {
			nthreads = 1;
		}
		
		for(i = 0; i != nthreads; ++i) {
			worklist.add(new LinkedList<Vertex>());
		}
		
		for(i = 0; i != vertex.length; ++i) {
			worklist.get(i % nthreads).addLast(vertex[i]);
		}
		
		WorkerThread[] workers = new WorkerThread[nthreads];
        
		for (int j = 0; j != nthreads; ++j) {
			workers[j] = new WorkerThread(j, worklist.get(j));
			workers[j].start();
		}
		
		for (i = 0; i != nthreads; ++i) {
			try {
				workers[i].join();
			} catch(InterruptedException exception) {
			}
		}
        
		end = System.nanoTime();

		System.out.println("T = " + (end-begin)/1e9 + " s");
	}

	public static void main(String[] args)
	{
		int	i;
		int	nsym;
		int	nvertex;
		int	maxsucc;
		int	nactive;
		int	nthread;
		boolean	print;
		Vertex	vertex[];
		Random	r;

		r = new Random(1);

		nsym = Integer.parseInt(args[0]);
		nvertex = Integer.parseInt(args[1]);
		maxsucc = Integer.parseInt(args[2]);
		nactive = Integer.parseInt(args[3]);
		nthread = Integer.parseInt(args[4]);
		print = Integer.parseInt(args[5]) != 0;
	
		System.out.println("nsym = " + nsym);
		System.out.println("nvertex = " + nvertex);
		System.out.println("maxsucc = " + maxsucc);
		System.out.println("nactive = " + nactive);

		vertex = new Vertex[nvertex];

		for (i = 0; i < vertex.length; ++i)
			vertex[i] = new Vertex(i);

		generateCFG(vertex, maxsucc, r);
		generateUseDef(vertex, nsym, nactive, r);
		liveness(vertex, nthread);

		if (print)
			for (i = 0; i < vertex.length; ++i)
				vertex[i].print();
	}
}