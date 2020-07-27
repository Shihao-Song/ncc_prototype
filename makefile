all:
	g++ -std=c++17 -I/usr/local/opt/hdf5 -L/usr/local/opt/hdf5 \
	-I/Library/Frameworks/Python.framework/Versions/3.8/include/python3.8 \
	-L/Library/Frameworks/Python.framework/Versions/3.8/lib/ \
	ncc.cc model.cc protobuf/proto_graph/graph.pb.cc snn_converter/ann_to_snn.cpp -o ncc -lhdf5 -lprotobuf -lboost_system -lboost_filesystem -lpython3.8

clean:
	rm -rf ncc
