/*
 * simple_fifo.cpp -- Simple SystemC 2.0 producer/consumer example.
 *
 * From "An Introduction to System Level Modeling in SystemC 2.0".
 * By Stuart Swan, Cadence Design Systems.
 * Available at www.systemc.org
 *
 * Original Author: Stuart Swan, Cadence Design Systems, 2001-06-18
 */
#include <systemc.h>

class write_if : virtual public sc_interface
{
public:
    virtual void write(char) = 0;
    virtual void reset() = 0;
};

class read_if : virtual public sc_interface
{
public:
    virtual void read(char &) = 0;
    virtual int num_available() = 0;
};

class fifo_ch : public sc_channel, public write_if, public read_if
{
public:
    fifo_ch(sc_module_name name) : sc_channel(name), num_elements(0), first(0) {}

    void write(char c)
    {
        if (num_elements == MAX)
            wait(read_event);

        data[(first + num_elements) % MAX] = c;
        ++ num_elements;
        write_event.notify();
    }

    void read(char &c)
    {
        if (num_elements == 0)
            wait(write_event);

        c = data[first];
        -- num_elements;
        first = (first + 1) % MAX;
        read_event.notify();
    }

    void reset()
    {
        num_elements = first = 0;
    }

    int num_available()
    {
        return num_elements;
    }

private:
    enum { MAX = 10 };
    char data[MAX];
    int num_elements, first;
    sc_event write_event, read_event;
};

class producer_m : public sc_module
{
public:
    sc_port<write_if> out;

    SC_HAS_PROCESS(producer_m);

    producer_m(sc_module_name name) : sc_module(name)
    {
        SC_THREAD(main);
    }

    void main()
    {
        const char *str =
            "Visit www.systemc.org and see what SystemC can do for you today!\n";

        while (*str)
            out->write(*str++);
    }
};

class consumer_m : public sc_module
{
public:
    sc_port<read_if> in;

    SC_HAS_PROCESS(consumer_m);

    consumer_m(sc_module_name name) : sc_module(name)
    {
        SC_THREAD(main);
    }

    void main()
    {
        char c;
        cout << endl << endl;

        while (true) {
            in->read(c);
            cout << c << flush;

            if (in->num_available() == 1)
                cout << "<1>" << flush;
            if (in->num_available() == 9)
                cout << "<9>" << flush;
        }
    }
};

class top_m : public sc_module
{
public:
    fifo_ch *fifo;
    producer_m *producer;
    consumer_m *consumer;

    top_m(sc_module_name name) : sc_module(name)
    {
        fifo = new fifo_ch("Fifo1");

        producer = new producer_m("Producer1");
        producer->out(*fifo);

        consumer = new consumer_m("Consumer1");
        consumer->in(*fifo);
    }
};

int sc_main (int, char *[])
{
    top_m top("Top1");

    sc_start();
    return 0;
}
