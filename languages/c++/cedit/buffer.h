#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <vector>

class Buffer {
private:
    std::vector<std::string> lines;

public:
    Buffer();

    // Get line by index.
    std::string& line(size_t n) { return lines[n]; }
    const std::string& line(size_t n) const { return lines[n]; }

    // Get number of lines.
    size_t size() const { return lines.size(); }

    // Get length of line by index.
    size_t line_length(size_t n) const { return lines[n].length(); }

    void insert_line(std::string, int);
    void append_line(std::string);
    void remove_line(int);
    void clear();

private:
    std::string rem_tabs(std::string);
};

#endif
