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
    #print("Step 3: minterms =", minterms, ", dont_cares =", dont_cares)
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
    # Convert to list for repeatability
    prime_implicants = sorted(prime_implicants)

    # Step 6: Select essential prime implicants
    #print("Step 6: prime_implicants =", prime_implicants)
    def covers_implicant(term, minterm):
        term_bits = term
        minterm_bits = bin(minterm)[2:].zfill(n_vars)
        return all(t == '-' or t == m for t, m in zip(term_bits, minterm_bits))

    essential = set()
    covered = set()
    for m in minterms:
        if m in dont_cares:
            continue
        covering = [pi for pi in prime_implicants if covers_implicant(pi, m)]
        if len(covering) == 1:
            essential.add(covering[0])
            covered.update([m for m in minterms if covers_implicant(covering[0], m)])
    # Add additional implicants if needed
    for m in minterms:
        if m not in covered and m not in dont_cares:
            for pi in prime_implicants:
                if pi not in essential and covers_implicant(pi, m):
                    essential.add(pi)
                    covered.update([m for m in minterms if covers_implicant(pi, m)])
                    break
    # Convert to list for repeatability
    essential = sorted(essential, reverse=True)

    # Step 7: Convert prime implicants to Boolean expression
    #print("Step 7: essential =", essential)
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
    #
    # Example: 2-variable truth table (4 entries)
    #
    input = [0, 1, 0, 1]
    result = minimize_boolean_function(input)
    print("2-variable input:", input)
    print("          result:", result)
    assert result == "B"

    #
    # Example: 3-variable truth table (8 entries)
    #
    input = [0, 1, 'X', 0, 1, 0, 'X', 1]
    result = minimize_boolean_function(input)
    print("3-variable input:", input)
    print("          result:", result)
    assert result == "AB + A~C + ~A~BC"

    input = [0, 1, 0, 1, 'X', 1, 'X', 0]
    result = minimize_boolean_function(input)
    print("3-variable input:", input)
    print("          result:", result)
    assert result == "~AC + ~BC"

    #
    # Example: 4-variable truth table (16 entries)
    #
    input = [0, 1, 'X', 0, 1, 0, 0, 1, 'X', 0, 0, 1, 0, 1, 'X', 1]
    result = minimize_boolean_function(input)
    print("4-variable input:", input)
    print("          result:", result)
    assert result == "ABD + ACD + ~AB~C~D + ~A~B~CD + BCD"

    # Truth table for f(A,B,C,D) = Σ(0,5,10,15) + d(2,7,12)
    #        0  1   2   3  4  5  6   7   8  9 10 11  12  13 14 15
    input = [1, 0, 'X', 0, 0, 1, 0, 'X', 0, 0, 1, 0, 'X', 0, 0, 1]
    result = minimize_boolean_function(input)
    print("4-variable input:", input)
    print("          result:", result)
    assert result == "~ABD + ~A~B~D + BCD + ~BC~D"

    #
    # Example: 5-variable truth table (32 entries)
    #
    # Truth table for f(A,B,C,D,E) = Σ(3,9,17,25,31) + d(0,10,20,30)
    #         0   1  2  3  4  5  6  7  8  9  10  11 12 13 14 15 16 17 18 19  20  21 22 23 24 25 26 27 28 29  30  31
    input = ['X', 0, 0, 1, 0, 0, 0, 0, 0, 1, 'X', 0, 0, 0, 0, 0, 0, 1, 0, 0, 'X', 0, 0, 0, 0, 1, 0, 0, 0, 0, 'X', 1]
    result = minimize_boolean_function(input)
    print("5-variable input:", input)
    print("          result:", result)
    assert result == "ABCD + A~C~DE + ~A~B~CDE + B~C~DE"

    #
    # Example: 8-variable truth table (256 entries)
    #
    input = [
        'X' if i in [0, 1, 2, 3, 4] else
        1 if i in [10, 100, 255] else
        0
        for i in range(256)
    ]
    result = minimize_boolean_function(input)
    print("8-variable input:", input)
    print("          result:", result)
    assert result == "ABCDEFGH + ~ABC~D~EF~G~H + ~A~B~C~D~FG~H"
