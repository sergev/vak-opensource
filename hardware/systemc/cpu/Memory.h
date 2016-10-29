static const int MEM_SIZE = 512;

SC_MODULE(Memory) {

public:
    sc_in<bool>     port_Clk;       // Clock input
    sc_in<bool>     port_Read;      // Read request
    sc_in<bool>     port_Write;     // Write request
    sc_in<int>      port_Addr;      // Address of access [31:2]
    sc_in<int>      port_BE;        // Byte enable for access [3:0]
    sc_in<int>      port_WData;     // Write data [31:0]
    sc_out<int>     port_RData;     // Read data [31:0]
    sc_out<bool>    port_Stall;     // Access not ready

    SC_CTOR(Memory) {
        SC_THREAD(execute);
        sensitive << port_Clk.pos();
        dont_initialize();

        _data = new int[MEM_SIZE];
    }

    ~Memory() {
        delete[] _data;
    }

private:
    int* _data;

    void execute();
};
