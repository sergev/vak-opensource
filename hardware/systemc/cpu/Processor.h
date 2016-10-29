SC_MODULE(Processor) {

public:
    sc_in<bool>     port_ClkIn;     // Clock input
    sc_out<int>     port_Addr;      // Address of access [31:2]
    sc_out<bool>    port_Read;      // Read request
    sc_out<bool>    port_Write;     // Write request
    sc_out<int>     port_WData;     // Write data [31:0]
    sc_out<int>     port_BE;        // Byte enable for access [3:0]
    sc_in<int>      port_RData;     // Read data [31:0]
    sc_in<bool>     port_Stall;     // Access not ready

    SC_CTOR(Processor)
    {
        SC_THREAD(execute);
        sensitive << port_ClkIn.pos();
        dont_initialize();
    }

private:
    void execute()
    {
        initialize();

        for (;;) {
            wait();
            step();
        }
    }

    void initialize();
    void step();
};
