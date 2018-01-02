/*
 * Match input string with pattern.
 * Return 1 on success.
 * Pattern can contain wild symbols:
 *
 * ^       - at beginning of string - any string not matched
 * *       - any string
 * ?       - any symbol
 * [a-z]   - symbol in range
 * [^a-z]  - symbol out of range
 */
int match(char *input, char *pattern)
{
    int status, negate_range, matched;

    matched = (*pattern != '^');
    if (!matched)
        pattern++;

    for (;;) {
        switch (*pattern++) {
        default:
            if (*input++ != pattern[-1])
                return !matched;
            break;
        case 0:
            if (*input)
                return !matched;
            return matched;
        case '?':
            if (! *input++)
                return !matched;
            break;
        case '*':
            if (!*pattern)
                return matched;
            for (; *input; input++)
                if (match(input, pattern))
                    return matched;
            return !matched;
        case '[':
            status = 0;
            negate_range = (*pattern == '^');
            if (negate_range)
                pattern++;
            while (*pattern++ != ']') {
                if (*pattern == '-') {
                    if (pattern[-1] <= *input && *input <= pattern[1])
                        status = 1;
                    pattern += 2;
                } else if (pattern[-1] == *input) {
                    status = 1;
                }
            }
            if (status == negate_range)
                return !matched;
            input++;
            break;
        }
    }
}
