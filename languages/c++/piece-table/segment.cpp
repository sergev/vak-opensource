class File_Header {
    // Descriptors for each segment.
    std::list<File_Segment> segments;

    // Count of lines in the file,
    // or at least in the scanned portion of it (when last_line==0).
    unsigned num_lines{};

    // Index of last line, starting from 1, or 0 when yet unknown.
    unsigned last_line{};
};

class File_Segment {
    // Pointer to the file stream, or nullptr, for empty lines.
    std::fstream *stream{};

    // Position in the file.
    std::streampos seek;

    const unsigned MAX_OFFSETS = 256;

    // Offsets of each line from a previous line, up to MAX_OFFSETS lines.
    // Empty for the last segment in the file.
    std::vector<uint32_t> offsets;
};

Methods:

    segment_t *fdesc2segm(int chan)
        -- Scan a file and create a segment chain.

    int segmwrite(segment_t *ff, int maxlines, int newf)
        -- Write a list of segments into given file descriptor.

    segment_t *blanklines(int n)
        -- Create a segment with n empty lines.

    int breaksegm(workspace_t *w, int n, int realloc_flag)
        -- Split a segment by line n in workspace w.

    int catsegm(workspace_t *w)
        -- Try to merge several segments into one to save some space.

    void insert(workspace_t *wksp, segment_t *f, int at)
        -- Insert a segment f into file, described by wksp, before the line at.

    segment_t *writemp(char *buf, int nbytes)
        -- Append a line buf of length n to the temporary file.
           Return a segment for this line.

    segment_t *delete(workspace_t *wksp, int from, int to)
        -- Delete specified lines from the workspace.
           Returns a segment chain of the deleted lines, with tail segment appended.

    segment_t *copysegm(segment_t *f, segment_t *end)
        -- Returns a copy of segment subchain, from f to end, not including end.

    segment_t *pick(workspace_t *wksp, int from, int to)
        -- Returns a segment chain for the given lines, tail segment appended.

    void freesegm(segment_t *f)
        -- Free the segment chain.

    void printsegm(segment_t *f)
        -- Debug output of segm chains.
