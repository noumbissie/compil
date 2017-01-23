all: my_plugin_pass.pgr

CPP=mpic++
CC=mpicc

FILE=test4.c

MPI_PATH=`dirname \` which mpicc \` `
MPI_FLAGS=-I$(MPI_PATH)/../include/mpi -L$(MPI_PATH)/../lib -lmpi
PLUGIN_FLAGS=-I`gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC
LIB_FLAGS=-g -Wall -shared -fPIC
CFLAGS=-g -O3


libplugin.so: my_plugin_pass.cpp
	 $(CPP) $(PLUGIN_FLAGS) -o libplugin.so $<
	 
libcc.so: checking_c.c
	$(CC) $(LIB_FLAGS) $(MPI_FLAGS) -o $@ $<

%.so: my_plugin_pass.cpp
	$(CPP) $(PLUGIN_FLAGS) -o $@ $<

%.pgr: %.so $(FILE) libcc.so
	$(CC) $(FILE) $(CFLAGS) -o $@ $(MPI_FLAGS) -fplugin=./$< -L. -lcc 

%.pdf: %.dot
	dot -Tpdf $< -o $@

clean:
	rm -rf *.pgr *.pdf

clean_all: clean
	rm -rf libplugin.so *.dot
