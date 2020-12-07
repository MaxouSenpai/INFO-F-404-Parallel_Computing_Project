# Directory where openmpi library is located
MPI_DIR=/lib/x86_64-linux-gnu/openmpi
# Open MPI include directory
INC_DIR=$(MPI_DIR)/include
# Libs where openmpi shared objects are
LIB_DIR=$(MPI_DIR)/lib
# Bin directory
BIN_DIR=$(MPI_DIR)/bin

FLAGS = -I$(INC_DIR) -L$(LIB_DIR)

all: #image.o mask.o options.o
	# gcc -g -o blur source/blur.c image.o mask.o options.o $(FLAGS) -lmpi -Wall
	gcc -g -o blur source/blur.c source/image.c source/mask.c source/options.c $(FLAGS) -lmpi -Wall

image.o:
	gcc -g -c source/image.c -Wall

mask.o:
	gcc -g -c source/mask.c -Wall

options.o:
	gcc -g -c source/options.c -Wall

clean:
	rm *.o

run:
	export PATH=$(PATH):$(BIN_DIR) && \
	mpirun -n 4 blur -i example/police1.raw -m example/mask1 -n 10 -o test.raw -d 1280 720
