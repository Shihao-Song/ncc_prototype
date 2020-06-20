all:
	g++ -std=c++17 -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial ncc.cc model.cc protobuf/proto_graph/graph.pb.cc -o ncc -lhdf5 -lprotobuf -lboost_system -lboost_filesystem

clean:
	rm -rf ncc
