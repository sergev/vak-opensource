#include <fstream>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>
#include "onnx.pb.h"

using namespace std;

using google::protobuf::util::TimeUtil;

//
// Iterates though the graph in the ModelProto and prints the contents.
//
void print_model(const onnx::ModelProto& model)
{
    if (model.has_ir_version())
        cout << "IR Version: " << model.ir_version() << endl;

    if (model.has_producer_name())
        cout << "Producer Name: " << model.producer_name() << endl;

    if (model.has_graph()) {
        const onnx::GraphProto& graph = model.graph();

        cout << "Graph:" << endl;
        if (graph.has_name())
            cout << "    Name: " << graph.name() << endl;

        for (int i = 0; i < graph.node_size(); i++) {
            const onnx::NodeProto& node = graph.node(i);

            if (i == 0)
                cout << "    Nodes:" << endl;
            if (node.has_op_type())
                cout << "        Op Type: " << node.op_type() << endl;
        }
        for (int i = 0; i < graph.initializer_size(); i++) {
            const onnx::TensorProto& initializer = graph.initializer(i);

            if (i == 0)
                cout << "    Initializers:" << endl;
            if (initializer.has_name())
                cout << "        Name: " << initializer.name() << endl;
        }
        for (int i = 0; i < graph.input_size(); i++) {
            const onnx::ValueInfoProto& input = graph.input(i);

            if (i == 0)
                cout << "    Inputs:" << endl;
            if (input.has_name())
                cout << "        Name: " << input.name() << endl;
        }
        for (int i = 0; i < graph.output_size(); i++) {
            const onnx::ValueInfoProto& output = graph.output(i);

            if (i == 0)
                cout << "    Outputs:" << endl;
            if (output.has_name())
                cout << "        Name: " << output.name() << endl;
        }
    }
}

//
// Main function:  Reads the entire address book from a file and prints all
// the information inside.
//
int main(int argc, char* argv[])
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 2) {
        cerr << "Usage:" << endl;
        cerr << "    " << argv[0] << " model.onnx" << endl;
        return -1;
    }

    // Read the existing ONNX model.
    onnx::ModelProto model;
    fstream input(argv[1], ios::in | ios::binary);
    if (!model.ParseFromIstream(&input)) {
        cerr << "Failed to parse " << argv[1] << endl;
        return -1;
    }

    print_model(model);

    // Optional: Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
