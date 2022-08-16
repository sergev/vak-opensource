//
// Useful routine for printing hex dumps.
//
// From: https://stackoverflow.com/questions/10599068/how-do-i-print-bytes-as-hexadecimal
//
std::ostream& hex_dump(std::ostream& os, const void *buffer,
                       std::size_t bufsize, bool showPrintableChars = true)
{
    if (buffer == nullptr) {
        return os;
    }
    auto oldFormat = os.flags();
    auto oldFillChar = os.fill();
    constexpr std::size_t maxline{8};

    // create a place to store text version of string
    char renderString[maxline+1];
    char *rsptr{renderString};

    // convenience cast
    const unsigned char *buf{reinterpret_cast<const unsigned char *>(buffer)};

    for (std::size_t linecount=maxline; bufsize; --bufsize, ++buf) {
        os << std::setw(2) << std::setfill('0') << std::hex
           << static_cast<unsigned>(*buf) << ' ';
        *rsptr++ = std::isprint(*buf) ? *buf : '.';
        if (--linecount == 0) {
            *rsptr++ = '\0';  // terminate string
            if (showPrintableChars) {
                os << " | " << renderString;
            }
            os << '\n';
            rsptr = renderString;
            linecount = std::min(maxline, bufsize);
        }
    }

    // emit newline if we haven't already
    if (rsptr != renderString) {
        if (showPrintableChars) {
            for (*rsptr++ = '\0'; rsptr != &renderString[maxline+1]; ++rsptr) {
                 os << "   ";
            }
            os << " | " << renderString;
        }
        os << '\n';
    }

    os.fill(oldFillChar);
    os.flags(oldFormat);
    return os;
}
