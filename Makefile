all:
	g++ -std=c++17 ncc.cc model.cc -o ncc -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5
