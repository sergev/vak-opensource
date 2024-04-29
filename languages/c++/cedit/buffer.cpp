#include "buffer.h"

Buffer::Buffer()
{
}

std::string Buffer::rem_tabs(std::string line)
{
    int tab = line.find("\t");
    if (tab == line.npos)
        return line;
    else
        return rem_tabs(line.replace(tab, 1, "    "));
}

void Buffer::insert_line(std::string line, int x)
{
    line = rem_tabs(line);
    lines.insert(lines.begin() + x, line);
}

void Buffer::append_line(std::string line)
{
    line = rem_tabs(line);
    lines.push_back(line);
}

void Buffer::remove_line(int i)
{
    lines.erase(lines.begin() + i);
}

void Buffer::clear()
{
    lines.clear();
}
