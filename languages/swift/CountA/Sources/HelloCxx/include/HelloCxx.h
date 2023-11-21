#include <string>

class HelloCxx {
private:
    int count{};

public:
    std::string message() const;

    void increment()
    {
        count++;
    }

    void decrement()
    {
        count--;
    }
};
