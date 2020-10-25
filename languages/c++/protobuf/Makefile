OBJS            = print-graph.o onnx.pb.o
CXXFLAGS        = -g -Wall -O -std=c++14
LDFLAGS         = -g
LIBS            = -lprotobuf

.PHONY:         all clean

all:            print-graph

clean:
		rm -f *.o print-graph onnx.pb.cc onnx.pb.h

onnx.pb.h onnx.pb.cc: onnx.proto
		protoc --cpp_out=. onnx.proto

print-graph:    $(OBJS) onnx.pb.h
		$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
###
onnx.pb.o: onnx.pb.cc onnx.pb.h
print-graph.o: print-graph.cc onnx.pb.h
