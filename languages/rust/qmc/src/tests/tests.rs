#[cfg(test)]
mod tests {
    use super::super::*;
    use super::super::minimizer::minimize_boolean_function;

    // 3-variable truth table: ~AB + BC
    pub fn generate_3var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 3];
        // ~AB (00x)
        for c in 0..2 {
            let m = c;
            table[m] = '1';
        }
        // BC (x11)
        for a in 0..2 {
            let m = (a << 2) | 3;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 4-variable truth table: A~B + C~D
    pub fn generate_4var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 4];
        // A~B (10xx)
        for cd in 0..4 {
            let m = (1 << 3) | cd;
            table[m] = '1';
        }
        // C~D (xx10)
        for ab in 0..4 {
            let m = (ab << 2) | 2;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 5-variable truth table: I~J + K~LM
    pub fn generate_5var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 5];
        // I~J (10xxx)
        for klm in 0..8 {
            let m = (1 << 4) | (0 << 3) | klm;
            table[m] = '1';
        }
        // K~LM (xx101)
        for ij in 0..4 {
            let m = (ij << 3) | (1 << 2) | (0 << 1) | 1;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 6-variable truth table: ~IJK + LM~N
    pub fn generate_6var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 6];
        // ~IJK (000xxx)
        for lmn in 0..8 {
            let m = lmn;
            table[m] = '1';
        }
        // LM~N (xxx110)
        for ijk in 0..8 {
            let m = (ijk << 3) | 6;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 7-variable truth table: ~FG + H~IJ + KLM
    pub fn generate_7var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 7];
        // ~FG (0x0xxxx)
        for ahijk in 0..32 {
            let m = (ahijk & 0x1E) << 1 | (ahijk & 1);
            table[m] = '1';
        }
        // H~IJ (x1x00xx)
        for afgklm in 0..32 {
            let m = (afgklm >> 2) << 5 | (1 << 3) | (afgklm & 3);
            table[m] = '1';
        }
        // KLM (xxx111x)
        for afghij in 0..64 {
            let m = (afghij << 3) | 7;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 8-variable truth table: ~AB + CDE + ~FGH
    pub fn generate_8var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 8];
        // ~AB (00xxxxxx)
        for cdefgh in 0..64 {
            let m = cdefgh;
            table[m] = '1';
        }
        // CDE (xx111xxx)
        for abfgh in 0..32 {
            let m = (abfgh >> 3) << 6 | (7 << 3) | (abfgh & 7);
            table[m] = '1';
        }
        // ~FGH (xxxx000x)
        for abcde in 0..32 {
            let m = (abcde << 3);
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 9-variable truth table: I~J + KLM + ~NOP
    pub fn generate_9var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 9];
        // I~J (10xxxxxxx)
        for klmnop in 0..128 {
            let m = (1 << 8) | (0 << 7) | klmnop;
            table[m] = '1';
        }
        // KLM (xxx111xxx)
        for ijnoq in 0..64 {
            let m = (ijnoq >> 3) << 6 | (7 << 3) | (ijnoq & 7);
            table[m] = '1';
        }
        // ~NOP (xxxxxx000)
        for ijklm in 0..32 {
            let m = ijklm << 3;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 10-variable truth table: ~IJK + LM~N + OP
    pub fn generate_10var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 10];
        // ~IJK (000xxxxxxx)
        for lmnop in 0..128 {
            let m = lmnop;
            table[m] = '1';
        }
        // LM~N (xxxxx110xx)
        for ijkop in 0..128 {
            let m = (ijkop >> 2) << 5 | (6 << 2) | (ijkop & 3);
            table[m] = '1';
        }
        // OP (xxxxxxx11x)
        for ijklmn in 0..64 {
            let m = (ijklmn << 3) | 6;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 11-variable truth table: FG~H + ~IJKL + MNO~P
    pub fn generate_11var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 11];
        // FG~H (xx11x0xxxxx)
        for abcijk in 0..128 {
            let m = (abcijk >> 4) << 8 | (3 << 5) | (abcijk & 0xF);
            table[m] = '1';
        }
        // ~IJKL (xxxx0000xxx)
        for abcdef in 0..64 {
            let m = (abcdef << 4);
            table[m] = '1';
        }
        // MNO~P (xxxxxxxx110)
        for abcdefgh in 0..256 {
            let m = (abcdefgh << 4) | 14;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 12-variable truth table: ~EF + GHI + ~JKL + MNOP
    pub fn generate_12var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 12];
        // ~EF (xxxx00xxxxxx)
        for abcdghij in 0..256 {
            let m = (abcdghij & 0xF0) << 4 | (abcdghij & 0xF);
            table[m] = '1';
        }
        // GHI (xxxxxx111xxx)
        for abcdefj in 0..128 {
            let m = (abcdefj >> 3) << 6 | (7 << 3) | (abcdefj & 7);
            table[m] = '1';
        }
        // ~JKL (xxxxxxx000xx)
        for abcdefg in 0..128 {
            let m = (abcdefg << 4);
            table[m] = '1';
        }
        // MNOP (xxxxxxxx1111)
        for abcdefgh in 0..256 {
            let m = (abcdefgh << 4) | 15;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 13-variable truth table: DE~F + ~GHIJ + K~LMN
    pub fn generate_13var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 13];
        // DE~F (xxx11x0xxxxxx)
        for abchijkl in 0..512 {
            let m = (abchijkl >> 5) << 9 | (3 << 6) | (abchijkl & 0x1F);
            table[m] = '1';
        }
        // ~GHIJ (xxxxxx0000xxx)
        for abcdef in 0..64 {
            let m = (abcdef << 7);
            table[m] = '1';
        }
        // K~LMN (xxxxxxxxx1000)
        for abcdefgh in 0..256 {
            let m = (abcdefgh << 5) | 8;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 14-variable truth table: ~CD + EFGH + ~IJK + LM~NO
    pub fn generate_14var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 14];
        // ~CD (xx00xxxxxxxxxx)
        for abefghij in 0..512 {
            let m = (abefghij & 0x1C0) << 5 | (abefghij & 0x3F);
            table[m] = '1';
        }
        // EFGH (xxxx1111xxxxxx)
        for abcdijk in 0..256 {
            let m = (abcdijk >> 3) << 7 | (15 << 3) | (abcdijk & 7);
            table[m] = '1';
        }
        // ~IJK (xxxxxxxx000xxx)
        for abcdefg in 0..128 {
            let m = (abcdefg << 7);
            table[m] = '1';
        }
        // LM~NO (xxxxxxxxxx1100)
        for abcdefgh in 0..256 {
            let m = (abcdefgh << 6) | 12;
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 15-variable truth table: AB~C + ~DEF + GHIJ + ~KLMNO
    pub fn generate_15var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 15];
        // AB~C (110xxxxxxxxxxxx)
        for defghijk in 0..2048 {
            let m = (3 << 13) | defghijk;
            table[m] = '1';
        }
        // ~DEF (xxx000xxxxxxxxx)
        for abchijklmnop in 0..4096 {
            let m = (abchijklmnop & 0x1FF8) << 2 | (abchijklmnop & 7);
            table[m] = '1';
        }
        // GHIJ (xxxxxxx1111xxxx)
        for abcdefk in 0..128 {
            let m = (abcdefk >> 3) << 7 | (15 << 3) | (abcdefk & 7);
            table[m] = '1';
        }
        // ~KLMNO (xxxxxxxxxxx0000)
        for abcdefghij in 0..1024 {
            let m = (abcdefghij << 5);
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    // 16-variable truth table: ~AB + CDE + ~FGHI + JK~LMNOP
    pub fn generate_16var_truth_table() -> String {
        let mut table = vec!['0'; 1 << 16];
        // ~AB (00xxxxxxxxxxxxxx)
        for cdefghijklmnop in 0..16384 {
            let m = cdefghijklmnop;
            table[m] = '1';
        }
        // CDE (xx111xxxxxxxxxxx)
        for abfghijklmnop in 0..8192 {
            let m = (abfghijklmnop >> 3) << 6 | (7 << 3) | (abfghijklmnop & 7);
            table[m] = '1';
        }
        // ~FGHI (xxxxxx0000xxxxxx)
        for abcdejk in 0..128 {
            let m = (abcdejk << 9);
            table[m] = '1';
        }
        // JK~LMNOP (xxxxxxxx11x00000)
        for abcdefgh in 0..256 {
            let m = (abcdefgh << 8) | (3 << 6);
            table[m] = '1';
        }
        table.into_iter().collect()
    }

    #[test]
    fn test_3_variables() {
        let result = minimize_boolean_function("01X010X1", true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("3-variable result: {}", result);
    }

    #[test]
    fn test_4_variables() {
        let truth_table = generate_4var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("4-variable result: {}", result);
    }

    #[test]
    fn test_5_variables() {
        let truth_table = generate_5var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("5-variable result: {}", result);
    }

    #[test]
    fn test_6_variables() {
        let truth_table = generate_6var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("6-variable result: {}", result);
    }

    #[test]
    fn test_7_variables() {
        let truth_table = generate_7var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("7-variable result: {}", result);
    }

    #[test]
        fn test_8_variables() {
        let truth_table = generate_8var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("8-variable result: {}", result);
    }

    #[test]
    fn test_9_variables() {
        let truth_table = generate_9var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("9-variable result: {}", result);
    }

    #[test]
    fn test_10_variables() {
        let truth_table = generate_10var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("10-variable result: {}", result);
    }

    #[test]
    fn test_11_variables() {
        let truth_table = generate_11var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("11-variable result: {}", result);
    }

    #[test]
    fn test_12_variables() {
        let truth_table = generate_12var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("12-variable result: {}", result);
    }

    #[test]
    fn test_13_variables() {
        let truth_table = generate_13var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("13-variable result: {}", result);
    }

    #[test]
    fn test_14_variables() {
        let truth_table = generate_14var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("14-variable result: {}", result);
    }

    #[test]
    fn test_15_variables() {
        let truth_table = generate_15var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("15-variable result: {}", result);
    }

    #[test]
    fn test_16_variables() {
        let truth_table = generate_16var_truth_table();
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        assert!(!result.is_empty());
        println!("16-variable result: {}", result);
    }
}
