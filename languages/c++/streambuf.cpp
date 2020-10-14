#include <iostream>
#include <streambuf>
#include <queue>

class Feedback : public std::streambuf {
    std::queue<char> q;
    char c;
public:

    Feedback() : std::streambuf()
    {
        setp(0, 0);
        setg(0, 0, 0);
        for (auto c : "1 1 ")
            if (c) q.push(c);
    }

    int underflow()
    {
        c = q.front();
        q.pop();
        setg(& c, & c, & c+1);
        return c;
    }

    int overflow(int c)
    {
        std::cout << char(c);
        q.push(c);
        return 0;
    }
};

void fib(std::istream & in, std::ostream & out)
{
    int a, b;
    in >> a;
    for (int i = 0; i < 20; ++i) {
        in >> b;
        out << a + b << ' ';
        a = b;
    }
    out << '\n';
}

int main()
{
    Feedback fb;
    std::istream in(&fb);
    std::ostream out(&fb);
    fib(in, out);
}
