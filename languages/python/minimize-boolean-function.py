#!/usr/bin/python3
import math

def minimize_boolean_function(truth_table):
    """
    Minimize a Boolean function of N variables given a truth table with 0s, 1s, and Xs.
    Input: truth_table - list of 2^N values (0, 1, or 'X') for minterms 0 to 2^N-1.
    Output: Simplified Boolean expression as a string.
    """
    # Step 1: Determine number of variables
    n_vars = int(math.log2(len(truth_table)))
    if len(truth_table) != 2 ** n_vars:
        raise ValueError("Truth table length must be a power of 2")

    # Step 2: Extract minterms (1s) and don't cares (Xs)
    minterms = [i for i, val in enumerate(truth_table) if val == 1]
    dont_cares = [i for i, val in enumerate(truth_table) if val == 'X']
    if not minterms:
        return "0"  # No 1s in truth table

    # Step 3: Generate implicants (minterms + don't cares)
    implicants = minterms + dont_cares
    if not implicants:
        return "0"

    # Step 4: Group implicants by number of 1s in binary representation
    def count_ones(n):
        if isinstance(n, str):
            return n.count('1')  # For combined terms like '10-1'
        return bin(n).count('1')  # For integer minterms

    groups = {i: [] for i in range(n_vars + 1)}  # 0 to N ones
    for m in implicants:
        groups[count_ones(m)].append((m, bin(m)[2:].zfill(n_vars)))

    # Step 5: Quine-McCluskey: Combine implicants differing by one bit
    def differ_by_one(s1, s2):
        return sum(a != b for a, b in zip(s1, s2)) == 1

    def combine_terms(m1, m2, s1, s2):
        result = ''.join('-' if a != b else a for a, b in zip(s1, s2))
        return result

    prime_implicants = set()
    used = set()
    while True:
        new_groups = {i: [] for i in range(n_vars + 1)}
        combined = set()
        for i in range(n_vars):
            for m1, s1 in groups[i]:
                for m2, s2 in groups[i + 1]:
                    if differ_by_one(s1, s2):
                        new_term = combine_terms(m1, m2, s1, s2)
                        new_groups[count_ones(new_term)].append((None, new_term))
                        combined.add((m1, s1))
                        combined.add((m2, s2))
                        if m1 is not None:
                            used.add(m1)
                        if m2 is not None:
                            used.add(m2)
        # Add uncombined terms as prime implicants
        for i in range(n_vars + 1):
            for m, s in groups[i]:
                if (m, s) not in combined:
                    prime_implicants.add(s)
        # Update groups
        groups = new_groups
        if not any(groups[i] for i in range(n_vars + 1)):
            break

    # Step 6: Select essential prime implicants
    def covers_implicant(term, minterm):
        term_bits = term
        minterm_bits = bin(minterm)[2:].zfill(n_vars)
        return all(t == '-' or t == m for t, m in zip(term_bits, minterm_bits))

    essential = []
    covered = set()
    for m in minterms:
        if m in dont_cares:
            continue
        covering = [pi for pi in prime_implicants if covers_implicant(pi, m)]
        if len(covering) == 1:
            essential.append(covering[0])
            covered.update([m for m in minterms if covers_implicant(covering[0], m)])
    # Add additional implicants if needed
    for m in minterms:
        if m not in covered and m not in dont_cares:
            for pi in prime_implicants:
                if pi not in essential and covers_implicant(pi, m):
                    essential.append(pi)
                    covered.update([m for m in minterms if covers_implicant(pi, m)])
                    break

    # Step 7: Convert prime implicants to Boolean expression
    def term_to_expression(term):
        vars = [chr(65 + i) for i in range(n_vars)]  # A, B, C, ...
        result = []
        for v, t in zip(vars, term):
            if t == '1':
                result.append(v)
            elif t == '0':
                result.append(f"~{v}")
        return ''.join(result) if result else "1"

    if not essential:
        return "0"
    expression = ' + '.join(term_to_expression(pi) for pi in essential)
    return expression if expression else "1"

# Example usage
if __name__ == "__main__":
    # Example: 3-variable truth table (8 entries)
    truth_table_3 = [0, 1, 'X', 0, 1, 0, 'X', 1]
    print("3-variable result:", minimize_boolean_function(truth_table_3))

    # Example: 4-variable truth table (16 entries)
    truth_table_4 = [0, 1, 'X', 0, 1, 0, 0, 1, 'X', 0, 0, 1, 0, 1, 'X', 1]
    print("4-variable result:", minimize_boolean_function(truth_table_4))
