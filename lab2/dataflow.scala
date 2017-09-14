import scala.actors._
import java.util.BitSet

case class StartVertex()
case class StopVertex();
case class ComputeVertexAgain(newIn: BitSet)
case class ComputeVertex()

case class VertexStarted()
case class ControllerAddWork()
case class ControllerFinishWork()
case class ControllerComputeAgain(v: Vertex, in: BitSet)

class Random(seed: Int) {
        var w = seed + 1;
        var z = seed * seed + seed + 2;

        def nextInt() =
        {
                z = 36969 * (z & 65535) + (z >> 16);
                w = 18000 * (w & 65535) + (w >> 16);

                (z << 16) + w;
        }
}

class Controller(val cfg: Array[Vertex]) extends Actor {
  var started = 0;
  var begin: Long   = 0
  var computing = 0

  def act() {
    react {
      case VertexStarted() => {
        started += 1;
        if (started == cfg.length) {
            begin = System.currentTimeMillis()
          for (u <- cfg)
            u ! new ComputeVertex;
        }
        act();
      }
      
      case ControllerAddWork() => {
          computing += 1
                    
          act()
      }
      
      case ControllerComputeAgain(v: Vertex, in: BitSet) => {
          computing += 1
          
          v ! new ComputeVertexAgain(in)
          
          act()
      }
      
      case ControllerFinishWork() => {
          computing -= 1
          
          if(computing <= 0) {
              cfg.foreach(v => v ! StopVertex())
              cfg.foreach(v => v.print)
              
              printf("Time elapsed: %f s\n", (System.currentTimeMillis() - begin) / 1000.0f)
          }
          
          else {
              act()
          }
      }
    }
  }
}

class Vertex(val index: Int, s: Int, val controller: Controller) extends Actor {
  var pred: List[Vertex] = List();
  var succ: List[Vertex] = List();
  val uses               = new BitSet(s);
  val defs               = new BitSet(s);
  var in                 = new BitSet(s);
  var out                = new BitSet(s);
  
  def connect(that: Vertex)
  {
    this.succ = that :: this.succ;
    that.pred = this :: that.pred;
  }
  
  def compute(new_in: BitSet) {
      out.or(new_in)
      
      val old = in
      
      in = new BitSet(s)
      in.or(out)
      in.andNot(defs)
      in.or(uses)
      
      if(!in.equals(old)) {
          pred.foreach(v => controller ! new ControllerComputeAgain(v, in))
      }
  }
  
  def act() {
    react {
      case StartVertex() => {
        controller ! new VertexStarted;
        
        act();
      }

      case ComputeVertex() => {        
        controller ! new ControllerAddWork
        compute(new BitSet())  
        controller ! new ControllerFinishWork
        
        act();
      }

      case StopVertex()  => { }
      
      case ComputeVertexAgain(newIn: BitSet) => {
          compute(newIn)
          controller ! new ControllerFinishWork
          
          act()
      }
    }
  }

  def printSet(name: String, index: Int, set: BitSet) {
    System.out.print(name + "[" + index + "] = { ");
    for (i <- 0 until set.size)
      if (set.get(i))
        System.out.print("" + i + " ");
    println("}");
  }

  def print = {
    printSet("use", index, uses);
    printSet("def", index, defs);
    //println("");
    printSet("in", index, in);
    printSet("out", index, out);
    println("");
  }
}

object Driver {
  val rand    = new Random(1);
  var nactive = 0;
  var nsym    = 0;
  var nvertex = 0;
  var maxsucc = 0;

  def makeCFG(cfg: Array[Vertex]) {

    cfg(0).connect(cfg(1));
    cfg(0).connect(cfg(2));

    for (i <- 2 until cfg.length) {
      val p = cfg(i);
      val s = (rand.nextInt() % maxsucc) + 1;

      for (j <- 0 until s) {
        val k = cfg((rand.nextInt() % cfg.length).abs);
        p.connect(k);
      }
    }
  }

  def makeUseDef(cfg: Array[Vertex]) {
    for (i <- 0 until cfg.length) {
      for (j <- 0 until nactive) {
        val s = (rand.nextInt() % nsym).abs;
        if (j % 4 != 0) {
          if (!cfg(i).defs.get(s))
            cfg(i).uses.set(s);
        } else {
          if (!cfg(i).uses.get(s))
            cfg(i).defs.set(s);
        }
      }
    }
  }

  def main(args: Array[String]) {
    nsym           = args(0).toInt;
    nvertex        = args(1).toInt;
    maxsucc        = args(2).toInt;
    nactive        = args(3).toInt;
    val cfg        = new Array[Vertex](nvertex);
    val controller = new Controller(cfg);

    controller.start;

    println("generating CFG...");
    for (i <- 0 until nvertex)
      cfg(i) = new Vertex(i, nsym, controller);

    makeCFG(cfg);
    println("generating usedefs...");
    makeUseDef(cfg);

    println("starting " + nvertex + " actors...");

    for (i <- 0 until nvertex)
      cfg(i).start;

    for (i <- 0 until nvertex)
      cfg(i) ! new StartVertex;
  }
}