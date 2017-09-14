NSYM	= 1000
NVERTEX	= 100
MAXSUCC	= 4
NACTIVE	= 10
PRINT   = 0

all:
	@mkdir -p classes
	#scalac -g:vars -d classes dataflow.scala
	#scalac -deprecation -d classes dataflow.scala
	scalac -deprecation -optimise -d classes dataflow.scala
	scala -classpath classes Driver $(NSYM) $(NVERTEX) $(MAXSUCC) $(NACTIVE) $(PRINT)

run:
	scala -classpath classes Driver $(NSYM) $(NVERTEX) $(MAXSUCC) $(NACTIVE) $(PRINT)

clean:
	rm -rf classes
