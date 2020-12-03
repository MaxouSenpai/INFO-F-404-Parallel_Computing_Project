# Directory where openmpi library is located
MPI_DIR=/lib/x86_64-linux-gnu/openmpi
# Open MPI include directory
INC_DIR=$(MPI_DIR)/include
# Libs where openmpi shared objects are
LIB_DIR=$(MPI_DIR)/lib
# Bin directory
BIN_DIR=$(MPI_DIR)/bin

FLAGS = -I$(INC_DIR) -L$(LIB_DIR)

all: blur.c blur.h image.o mask.o options.o
	gcc -g -o blur blur.c image.o mask.o options.o $(FLAGS) -lmpi -Wall

image.o: image.c image.h
	gcc -g -c image.c -Wall

mask.o: mask.c mask.h
	gcc -g -c mask.c -Wall

options.o: options.c options.h
	gcc -g -c options.c -Wall

clean:
	rm *.o

run:
	export PATH=$(PATH):$(BIN_DIR) && \
	mpirun -n 4 blur -i data/police1.raw -m data/mask1 -n 10 -o test.raw -d 1280 720
