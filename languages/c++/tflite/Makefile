CXX     = g++
CXXFLAGS= -std=c++11 -O2 -g
LDFLAGS = -g
OBJS    = tflite_model_parse.o

all:  tflite_model_parse

clean:
	rm -f *.o tflite_model_parse schema_tflite.h

tflite_model_parse: $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

run:    tflite_model_parse
	./tflite_model_parse

schema_tflite.h: schema.fbs
	flatc -c schema.fbs --gen-mutable --filename-suffix _tflite

schema.fbs:
	wget https://github.com/tensorflow/tensorflow/raw/master/tensorflow/lite/schema/schema.fbs

tflite_model_parse.o: tflite_model_parse.cpp schema_tflite.h
