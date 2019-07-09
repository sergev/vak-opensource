#include <cstdlib>  // setenv, atoi
#include <iostream> // cerr, cout, endl
#include <boost/python.hpp>

struct World
{
    void set(std::string msg) {
        this->msg = msg;
    }

    std::string greet() {
        return msg;
    }

    std::string msg;
};

/// Staticly linking a Python extension for embedded Python.
BOOST_PYTHON_MODULE(hello)
{
    namespace python = boost::python;
    python::class_<World>("World")
        .def("greet", &World::greet)
        .def("set", &World::set)
        ;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: call pythonfile funcname [args]" << std::endl;
        return 1;
    }
    char* module_name   = argv[1];
    char* function_name = argv[2];

    // Explicitly add initializers for staticly linked modules.
    PyImport_AppendInittab("hello", &inithello);

    // Initialize Python.
    setenv("PYTHONPATH", ".", 1);
    Py_Initialize();

    namespace python = boost::python;
    try {
        // Convert remaining args into a Python list of integers.
        python::list args;
        for (int i=3; i < argc; ++i)
        {
            args.append(std::atoi(argv[i]));
        }

        // Import the user requested module.
        // >>> import module
        python::object module = python::import(module_name);

        // Invoke the user requested function with the provided arguments.
        // >>> result = module.fn(*args)
        python::object result = module.attr(function_name)(*python::tuple(args));

        // Print the result.
        std::cout << python::extract<int>(result)() << std::endl;
    }
    catch (const python::error_already_set&) {
        PyErr_Print();
        return 1;
    }

    // Do not call Py_Finalize() with Boost.Python.
}
