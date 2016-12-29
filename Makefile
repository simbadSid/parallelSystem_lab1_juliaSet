EXEC=sequential_julia

all: $(EXEC)
CC_sequential=gcc
CC_parallel=smpicc
RUN_sequential=./
RUN_parallel=smpirun
BINDIR=bin/
SRCDIR=src/
OUTPUT_FILE_NAME_SEQUENTIAL=data/output/pictureSeqential.bmp
OUTPUT_FILE_NAME_PARALLEL=data/output/pictureParallel.bmp
CFLAGS  = -g -Wall -Werror -MP -MMD -DCONFIG_DEBUG
LDFLAGS  = -lm





#-----------------------------------------------------------------------------------------------------------
# ----------------------------------------------- Executions -----------------------------------------------
#-----------------------------------------------------------------------------------------------------------
PICTURE_DIMENSION=1001
NBR_PROCESS=5
HOST_FILE=data/input/simple_cluster_hostfile.txt
PLATFORM=data/input/simple_cluster.xml



exec_sequential_julia:	sequential_julia
						$(RUN_sequential)sequential_julia $(PICTURE_DIMENSION) ;\
						eog $(OUTPUT_FILE_NAME_SEQUENTIAL)

exec_1D_parallel_julia:	1D_parallel_julia
						$(RUN_parallel) -np $(NBR_PROCESS) -hostfile $(HOST_FILE) -platform $(PLATFORM) $(BINDIR)1D_parallel_julia $(PICTURE_DIMENSION)


#-----------------------------------------------------------------------------------------------------------
# ----------------------------------------------- Executable -----------------------------------------------
#-----------------------------------------------------------------------------------------------------------
sequential_julia:				$(BINDIR)sequential_julia.o $(BINDIR)tools.o
								$(CC_sequential) $(CFLAGS) -o $@ $^ $(LDFLAGS)

1D_parallel_julia:				$(BINDIR)1D_parallel_julia.o $(BINDIR)tools.o
								$(CC_parallel) -o $(BINDIR)$@ $^  $(LDFLAGS)


#-----------------------------------------------------------------------------------------------------------
# ----------------------------------------------- Modules  -------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
$(BINDIR)sequential_julia.o:	$(SRCDIR)sequential_julia.c
								$(CC) $(CFLAGS) -c -o $@ $^

$(BINDIR)1D_parallel_julia.o:	$(SRCDIR)1D_parallel_julia.c
								$(CC_parallel) -O3 -c -o $@ $^

$(BINDIR)tools.o:				$(SRCDIR)tools.c
								$(CC) $(CFLAGS) -c -o $@ $^



#-----------------------------------------------------------------------------------------------------------
# ------------------------------------------ General Methods -----------------------------------------------
#-----------------------------------------------------------------------------------------------------------
.PHONY	: clean mrproper
clean:
	rm -f $(BINDIR)*
mrproper: clean
	rm -rf $(EXEC) $(OUTPUT_FILE_NAME_SEQUENTIAL) $(OUTPUT_FILE_NAME_PARALLEL)
