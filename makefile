# Directory where openmpi library is located
MPI_DIR=/lib/x86_64-linux-gnu/openmpi
# Open MPI include directory
INC_DIR=$(MPI_DIR)/include
# Libs where openmpi shared objects are
LIB_DIR=$(MPI_DIR)/lib
# Bin directory
BIN_DIR=$(MPI_DIR)/bin

FLAGS = -I$(INC_DIR) -L$(LIB_DIR)

all: blur.c blur.h image.o mask.o
	gcc -o blur blur.c image.o mask.o $(FLAGS) -lmpi -Wall

image.o: image.c image.h
	gcc -c image.c -Wall

mask.o: mask.c mask.h
	gcc -c mask.c -Wall

clean:
	rm *.o

run:
	export PATH=$(PATH):$(BIN_DIR) && \
	mpirun -n 4 blur

