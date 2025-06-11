#[cfg(test)]
mod tests {

    use super::super::minimizer::minimize_boolean_function;

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
            let m = abcdef << 4;
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
            let m = abcdefg << 4;
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
            let m = abcdef << 7;
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
            let m = abcdefg << 7;
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
            let m = abcdefghij << 5;
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
            let m = abcdejk << 9;
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
    fn test_2_variables() {
        let result = minimize_boolean_function("0101", true, true, false, 5).unwrap();
        println!("2-variable result: {}", result);
        assert!(result == "B");
    }

    #[test]
    fn test_3_variables() {
        let result = minimize_boolean_function("01X010X1", true, true, false, 5).unwrap();
        println!("3-variable result: {}", result);
        assert!(result == "AB + A~C + ~A~BC");
    }

    #[test]
    fn test_4_variables() {
        let result = minimize_boolean_function("10X0010X0010X001", true, true, false, 5).unwrap();
        println!("4-variable result: {}", result);
        assert!(result == "BCD + ~ABD + ~BC~D + ~A~B~D");
    }

    #[test]
    fn test_5_variables() {
        let result = minimize_boolean_function("X001000001X000000100X000010000X1", true, true, false, 5).unwrap();
        println!("5-variable result: {}", result);
        assert!(result == "ABCD + A~C~DE + B~C~DE + ~A~B~CDE");
    }

    #[test]
    fn test_6_variables() {
        let result = minimize_boolean_function("X001000001X000000100X000010000X000000000000000000000000000000001", true, true, false, 5).unwrap();
        println!("6-variable result: {}", result);
        assert!(result == "ABCDEF + ~AB~D~EF + ~AC~D~EF + ~A~B~C~DEF");
    }

    #[test]
    fn test_7_variables() {
        let result = minimize_boolean_function("X001000001X000000100X000010000X0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001", true, true, false, 5).unwrap();
        println!("7-variable result: {}", result);
        assert!(result == "ABCDEFG + ~A~BC~E~FG + ~A~BD~E~FG + ~A~B~C~D~EFG");
    }

    #[test]
    fn test_8_variables() {
        let truth_table = "XXXXX00000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        println!("8-variable result: {}", result);
        assert!(result == "ABCDEFGH + ~ABC~D~EF~G~H + ~A~B~C~D~FG~H");
    }

    #[test]
    fn test_9_variables() {
        let truth_table = "XXXXX000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001";
        println!("9-variable truth_table: {}", truth_table);
        let result = minimize_boolean_function(&truth_table, true, true, false, 5).unwrap();
        println!("9-variable result: {}", result);
        assert!(result == "ABCDEFGHI + ~A~BCD~E~FG~H~I + ~A~B~C~D~E~GH~I");
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
