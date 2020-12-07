# Directory where openmpi library is located
MPI_DIR=/lib/x86_64-linux-gnu/openmpi
# Open MPI include directory
INC_DIR=$(MPI_DIR)/include
# Libs where openmpi shared objects are
LIB_DIR=$(MPI_DIR)/lib
# Bin directory
BIN_DIR=$(MPI_DIR)/bin

FLAGS = -I$(INC_DIR) -L$(LIB_DIR)

build: object/image.o object/mask.o object/options.o
	gcc -g -o blur source/blur.c object/image.o object/mask.o object/options.o $(FLAGS) -lmpi -Wall

object/image.o:
	mkdir object -p
	gcc -c source/image.c -o object/image.o -Wall

object/mask.o:
	mkdir object -p
	gcc -c source/mask.c -o object/mask.o -Wall

object/options.o:
	mkdir object -p
	gcc -c source/options.c -o object/options.o -Wall

clean:
	rm object -d -r

test1:
	export PATH=$(PATH):$(BIN_DIR) && \
	mpirun -n 4 blur -i example/police1.raw -m example/mask1 -n 10 -o test1.raw -d 1280 720

test2:
	export PATH=$(PATH):$(BIN_DIR) && \
	mpirun -n 4 blur -i example/police2.raw -m example/mask2 -n 10 -o test2.raw -d 1280 720

test3:
	export PATH=$(PATH):$(BIN_DIR) && \
	mpirun -n 4 blur -i example/police3.raw -m example/mask3 -n 10 -o test3.raw -d 1280 720
