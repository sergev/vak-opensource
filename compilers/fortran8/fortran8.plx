/* Page 62 */
                    /* Appendix F */

                    /* The FORTRAN/8 compiler */

declare nsy literally '105', nt literally '47';
declare v(nsy) character initial ( '<error: token = 0>', ';', '(', ')', ',',
    '+', '-', '/', '*', '=', 'if', 'do', 'go', 'to', '_|_', 'end', 'abs',
    'sqr', 'tab', 'stop', 'sqrt', '.or.', '.lt.', '.le.', '.eq.', '.ne.',
    '.gt.', '.ge.', 'got0', 'real', 'data', 'call', 'read', 'float', '.not.',
    '.and.', 'write', 'return', 'common', 'integer', 'writeon', '<number>',
    'continue', 'function', '<string>', 'dimension', 'subroutine',
    '<identifier>', '<if>', '<term>', '<goto)', '<data>', '<call>', '<paren>',
    '<comma>', '<labell>', '<label2>', '<end go>', '<program>', '<primary>',
    '<do head>', '<primary*>', '<variable>', '<relation>', '<do label>',
    '<statement>', '<secondary>', '<data head>', '<read head>',
    '<logical if>', '<expression>', '<right part>', '<write head>',
    '<do variable>', '<go transfer>', '<declaration>', '<if statement>',
    '<do statement>', '<go statement>', '<double label>', '<boolean term>',
    '<arithmetic if>', '<variable list>', '<master program>',
    '<statement list>', '<read statement>', '<subscript head>',
    '<procedure head>', '<procedure type>', '<tab expression>',
    '<statement block>', '<procedure block>', '<basic statement>',
    '<subroutine call>', '<write statement>', '<boolean primary>',
    '<declaration list>', '<declaration type>', '<data declaration>',
    '<labeled statement>', '<procedure heading>', '<boolean expression>',
    '<logical expression>', '<paramless procedure>', '<assignment statement>',
    '<procedure & parameters>');
declare v_index(12) bit(8) initial ( 1, 10, 14, 10, 33, 37, 39, 41, 45, 46,
    47, 47, 48);
declare cl(nsy) bit(96) initial (
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 22202 20002 00000 00022 22200 02222 22020 222",
    "(2) 00300 33000 00000 03320 30000 00000 00033 00000 03002 003",
    "(2) 03023 22222 00200 60002 02222 22220 02200 22200 21000 002",
    "(2) 00300 33000 00000 03320 30000 00000 00030 00000 03002 003",
    "(2) 00100 00000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 00100 00000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 02100 00000 00000 01100 10000 00000 00010 00000 03000 001",
    "(2) 00300 00020 00000 03300 30000 00000 00030 00000 03000 003",
    "(2) 00100 11000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 00100 00000 00010 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 01000 000",
    "(2) 00000 00000 00010 00000 00000 00000 00000 00000 03000 000",
    "(2) 00200 00000 00000 00000 00000 00000 00000 00000 02000 000",
    "(2) 00000 00000 11100 00001 00000 00011 11100 01111 11010 111",
    "(2) 01000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 00300 00000 00000 00000 00000 03000 00000 00000 000",

/* Page 63 */

    "(2) 00100 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 11000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 C0000 00090 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 11000 00000 01100 10000 00000 00011 00000 01000 001",
    "(2) 00200 22000 00000 02200 20000 00000 00020 00000 02000 002",
    "(2) 00200 22000 00000 02200 20000 00000 00020 00000 02000 002",
    "(2) 00200 22000 00000 02200 20000 00000 00020 00000 02000 002",
    "(2) 00200 22000 00000 02200 20000 00000 00020 00000 02000 002",
    "(2) 00200 22000 00000 02200 20000 00000 00020 03000 02000 002",
    "(2) 00200 22000 00090 02200 20000 00000 00020 00000 02000 002",
    "(2) 00200 00000 00000 00000 00000 00000 00300 00000 02000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 002",
    "(2) 00000 00000 03000 00000 00000 00000 00000 00000 00000 002",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 002",
    "(2) 00100 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 11000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 00L00 11000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 00100 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00030 002",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 002",
    "(2) 00100 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 02023 22320 22200 00002 02222 22220 02200 22200 22200 002",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 00011 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 002",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 03133 22222 00000 00000 02222 22200 00000 20000 00000 000",
    "(2) 00100 11000 00000 01100 10000 00000 00011 00000 01000 001",
    "(2) 02022 22100 00000 00000 02222 22200 00000 20000 00000 000",
    "(2) 00100 00000 00000 00000 00000 00000 00000 00000 01000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 00000 00000 00090 00000 00000 00000 00000 00000 00000 001",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 01000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 01300 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 03000 000",
    "(2) 00000 00000 11100 00001 00000 00010 01100 01100 11100 001",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 00000 00000 11102 00001 00000 00011 11100 01111 11010 111",
    "(2) 02022 22200 00000 00000 02222 22200 00000 20000 00000 000",
    "(2) 02001 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 00010 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 02033 22221 00000 00000 02222 22200 00000 20000 00000 000",
    "(2) 00100 11000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 001",

/* Page 64 */

    "(2) 03000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 02022 22210 00000 00000 02222 22200 00000 20000 00000 000",
    "(2) 00000 00100 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 00000 00000 00100 00001 00000 00010 01100 01100 10000 001",
    "(2) 02033 11000 00000 00000 02111 11100 00000 20000 00000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 11000 00000 01110 10000 00000 00010 00000 01001 001",
    "(2) 00001 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00011 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 01000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 01000 000",
    "(2) 00020 00000 00000 00000 02000 00000 00000 10000 00000 000",
    "(2) 00010 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00300 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 00000 00000 00002 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 11100 20001 00000 00010 01100 01100 11000 001",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00100 11000 00000 01100 10000 00000 00010 00000 01000 001",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 00000 002",
    "(2) 00011 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 00000 10000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 11300 00001 00000 00010 01100 01100 11000 001",
    "(2) 02000 00000 00030 00000 00000 00000 00000 00000 00000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 coo",
    "(2) 00020 00000 00000 00000 02000 00000 00000 20000 00000 000",
    "(2) 00000 00000 33300 00003 00000 00031 13300 03311 33000 103",
    "(2) 00000 0000q 00000 00000 00000 00000 00000 00000 00000 001",
    "(2) 00000 00000 00000 00000 00000 00000 00000 00000 01000 000",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 22200 00002 00000 00021 12200 02211 22000 102",
    "(2) 00010 00000 00000 00000 01000 00000 00000 00000 00000 000",
    "(2) 00020 00000 00000 00000 02000 00000 00000 20000 00000 000",
    "(2) 00000 00000 22200 00002 00000 00022 22200 02222 22000 202",
    "(2) 02000 00000 00000 00000 00000 00000 00000 00000 00000 000",
    "(2) 00000 00000 22200 00002 00000 00022 22200 02222 22000 202");
declare nc1triples literally '259';
declare c1triples(nc1tpiples) fixed initial ( 131586, 131589, 131590, 131600,
    131601, 131604, 131617, 13161e, 131625, 131631, 148995, 207108, 210729,
    262658, 262661, 262662, 262672, 262673, 262676, 262689, 262697, 262703,
    328194, 328197, 328198, 328208, 328209, 328212, 328225, 328233, 328239,
    393730, 393733, 393734, 393744, 393745, 393748, 393761, 393769, 393775,
    459266, 459269, 459270, 459280, 459281, 459284, 459297, 459305, 459311,

/* Page 65 */

    524802, 524805, 524806, 524816, 524817, 524820, 524833, 524841, 524847,
    590338, 590341, 590342, 590352, 590353, 590356, 590369, 590377, 590383,
    934145, 942090, 942091, 942092, 942099, 942108, 942111, 942112, 942116,
    942117, 942120, 942121, 942127, 1049090, 1049093, 1049094, 1049104,
    1049105, 1049108, 1049121, 1049129, 1049135, 1114626, 1114629, 1114630,
    1114640, 1114641, 1114644, 1114657, 1114665, 1114671, 1180162, 1180165,
    1180166, 1180176, 1180177, 1180'..-80, 1180193, 1180201, 1180207, 1311234,
    1311237, 1311238, 1311248, 1311249, 1311252, 1311265, 1311273, 1311279,
    1376770, 1376773, 1376774, 1376764, 1376785, 1376788, 1376801, 1376802,
    1376809, 1376815, 2163202, 2163205, 2163206, 2163216, 2163217, 2163220,
    2163233, 2163241, 2163247, 2228738, 2228741, 2228742, 2228752, 2228753,
    2228756, 2228769, 2228770, 2228777, 2228783, 2294274, 2294277, 2294278,
    2294288, 2294289, 2294292, 2294305, 2294306, 2294313, 2294319, 3026699,
    3080961, 3146242, 3146245, 3146246, 3146256, 3146257, 3146260, 3146273,
    3146274, 3146281, 3146287, 3213097, 3358212, 3614980, 3817729, 3225674,
    3825675, 3825676, 3825683, 3825692, 3325695, 3825696, 3825700, 3825701,
    3825704, 3825705, 3825711, 3933186, 3933189, 3933190, 3933200, 3°33201,
    3933204, 3923217, 3933225, 3933231, 3998210, 3998213, 3998214, 3998224,
    3998225, 3998228, 3998241, 3908249, 3998255, 3999746, 3999760, 3999761,
    3999764, 3999777, 3999785, 3999791, 4129282, 4129285, 4129286, 4129296,
    4129297, 4129300, 4129313, 4129321, 4129327, 4472323, 4472324, 4719106,
    4719109, 4719110, 4719120, 4719121, 4719124, 4719137, 4719145, 471°151,
    4736515, 4736516, 4765154, 4785157, 4785158, 4785168, 4785169, 4785172,
    4785185, 4785193, 4785199, 4850436, 5389828, 5521665, 5636610, 5636613,
    5636614, 5636624, 5636625, 5636628, 5636641, 5636649, 5636655, 5654019,
    5654020, 5713667, 5713668, 5980417, 6308097, 6372868, 6433028, 6433031);
declare prtb(122) fixed initial (0, 3824143, 5713667, 23055, 11823/ 24651,
21569, 75, 65, 11055, 11823, 32, 10, 36, 40, 47, 0, 1049158, 1114694,
1311302, 2163270, 17470, 18502, 18476, 18521, 613, 12389, 582, 22086,
21054, 22319, 25129, 17470, 18502, 18476, 18521, 22086, 3, 41, 62, 0,
25129, 67, 66, 12, 0, 0, 0, 0, 01 0, 0, 0/ 0, 0, 0, 0, 0, 0, 20815, 128531
18998, 11, 50, 0, 0, 56, 0, 0, 17925, 17926, 5, 6, 0, 823, 0, 12551, 61,
0, 0, 24914, 13138, 19001, 97, 51, 52, 0, 56, 15624, 0, 4210185, 15364,
18692, 17983, 9, 18, 48, 2366, 62, 0, os 0, 25877, 0, 96, 91, 0, 0, 0, 69,
0, 0, 0, 20515, 34, 0, 100, 0, 01 0, 0, 0, 0);
declare prdtb(122) bit(8) initial (0, 3, 104, 2, 103, 84, 8, 83, 7, 107, 108,
112, 23, 117, 118, 67, 80, 40, 41, 42, 43, 111, 114, 115, 116, 51, 44, 39,
66, 93, 106, 95, 113, 1191 120, 121, 68, 82, 25s 92, 81, 87, 94s 36, 76,
19, 53, 54, 55, 56, 57, 58, 77, 90, 98, 110, 18, 91, 89, 20/ 781 79, -73,
(4, 61, 38, 60, 88, 65/ 27, 28, 29, 30, 26, 24, 1, 33, 32, 31, 69, 86, 97,
75, 85, 96, 109, 37, 59, 35, 34, 72, 70, 71, 52, 63, 122, 22, 64, 62, 9,
11, 14, 46, 45, 5, 6, 4, 16, 105, 21, 10, 15, 17, 49, 48, 47, 100, 12, 99,
50, 101, 13, 102);
declare hdtb(122) 8it(8) initial (0, 58, 105, 581 103, 96, 84, 96, 84, 88,
88, 68, 48, 72, 72, 86, 53, 66, 66, 66, 66, 85, 94, 94, 94, 95, 69, 66,
62, 82, 87, 98, 68, 72, 72, 72, 86, 571 55, 62, 54, 75, 98, 61, 50, 92,
63, 63, 63, 63, 63, 63, 50, 97, 51, 52, 92, 97, 97, 76, 74, 74, 64, 78, 50, 66, 99, 97, 62, 70, 70, 70, 70, 70, 79, 83, 49, 49, 49, 77, 75, 67,
78, 75, 67/ 93, 66, 99, 59, 59, 73/ 77, 60, 102/ 71, 89, 81, 71, 104, 65,
65/ 92, 101, 101/ 90, 90, 90, 92, 87, 76, 65, 92, 92, 80/ 80, 80, 91, 65,
91, 95, 100, 92/ 100);
declare prlength(122) bit(8) initial (0, 4/ 4, 3, 3, 3, 3/ 2, 2, 3, 3, 2, 2,
2, 2, 2, 1, 4, 4/ 4, 4/ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 31 31 3, 3, 3, 2:
2, 2, 1/ 3, 2/ 21 2, 1, 1/ 1, 1, 1, 1, iv 11 11 1, 1/ 11 1/ 1, 3, 3, 3: 2, 2/ 1, 1, 2, 1/ 1/ 3, 3, 2, 2, 1, 3, 1/ 31 2, 1, 1, 3, 3, 3, 2, 2, 2, 1, 2,
3, 1, 4, 3, 3, 3/ 2, 2, 2, 3, 2, 1, 1, 1, 3, 1, 2, 2, 1, 1, 1, 2, 1, 1, 1,
3, 2/ 1, 2, 11 1, 1/ 1, 1, 1);
declare c0ntext_case(122),bit(8) initial (0, 0, 0, 0, 0: 0, 0, 0, 0, 0, 0, 0,
0/ 0/ 0/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 01 0, 0, 0, 0, 0, 0, 0,
0, 0, 1/ 0, 0, 0/ 0, 0, 0/ 0, 0, 0, 0: 0, 0, 0: 0, 0: 0/ 0: 0, 0, 0: 0, 0,
0, 0: 0, 1, 0, 0, 0, 0: 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0/ 0, 0, 0, 0, 0, 0,
0/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
declare left_context(0) bit(8) initial ( 0);
declare left_index(58) bit(8) initial ( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0: 0/ 0, 0/ 0, 0/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 07 (j, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0/ 0, 0, 0, 0, 0, 0, 0, 0, 1); declare context triple(0) fixed initial ( 0);
declare triple_indfx(58) bit(8) initial ( 0/ 0, 0, 0, 0, 0, 0, 01 0, 0, 0, 0,
0, 0, 0, 0/ 0, 0, 0/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0/ 0, 0, 0, 0, 0, 0, 0, 0,
/* end of cards punched by syntax el,(/ /* declarations for the scanner
/* token is the index into the vocabulary v() of the last symbol scanned:
cp is the pointer to the last character scanned in the cardimage,
bcd is the last symbol scanned (literal character string). */ declare (token, cp) fixed, bcd character;
/* set up some convenient abbreviations for printer control */ declare eject page litfrally 'output(l) = page',
page charatter initial (11'), double character initial (101), double space literally 'output(1) = double',
x70 chtracter initial (1
1);
/* length of longest symbol in v */
0, 0, 0/ 0, 0, 0, 0, 0/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0: 0, 0, 0, 1);
declare pr_index(105) bit(8) initial ( 1, 9, 17, 29, 41, 41, 41, 43, 44, 44,
44, 44, 44, 45, 45, 45, 45, 45, 45, 46, 46, 46, 47, 48, 49/ 50, 51: 52,
53, 54, 55, 56, 56, 56, 56, 56, 56, 57, 58, 59, 59, 66, 67, 67, 67, 68,
68, 69, 69, 74, 74, 74, 74, 74, 74, 75, 75, 75, 76, 79, 80, 80, 87, 87,
87, 88, 90, 90, 90, 90, 97, 99, 99, 99, 99, 99/ 100, 101, 102, 102, 104,
104, 104, 104, 107, 108, 108, 108, 109, 109, 109, 109, 111, 112, 113, 116,
117, 117, 117, 118, 119, 119, 120/ 121, 122, 123);
declare (reserved_limit, margin_chop) fixed;
/* chartype() is used to distinguish classes of symbols in the scanner. tx() is a table used for translating from one character set to another. control() holds the value of the compiler control toggles set in $ cards. not letter_or_digit() is similiar to chartype() but used in scanning identifiers only.
all are used by the scanner and control() is set there.
declare (chartype, tx) (255) bit(8),
(control, not_letter_or_digit)(255) bit(1);
/* alphabet consists of the symbols considered alphabetic in building identifiers */
declare alphabet character initial (,abcdefghijklmnopqrstuvwxyz_$@#');
/* buffer holds the latest cardimage,
text holds the present state of the input text
(not including the portions deleted by the scanner),
text limit is a convenient place to store the pointer to the end of text, card-count is incremented by one for every source card read,
errof count tabulates the errors as they are detected,
severeerrors tabulates those errors of fatal significance. */ declare (buffer, text) character,
(text_limit, tard_count, error_count, severe_errors, previous_error) fixed
/* number_value contains the numeric value of the last constant scanned, declare number_value fixed;
/** each of the following contains the index into v() of the corresponding symbol. we ask: if token = ident etc. */
declare (ident, number, divide, eofile) fixed;
/* stopit() is a table of symbols which are allowed to terminate the error flush process. in general they are symbols of sufficient syntactic hierarchy that we expect to avoid attempting to start checking again right into another error producing situation. the token stack is also flushed down to something acceptable to a stopit() symbol. failsoft is a bit which allows the compiler one attempt at a gentle recovery. then it takes a strong hand. when there is real trouble compiling is set to false, thereby terminating the compilation.
*1
declare stopit(100) bit(1), (failsoft, compiling) bit(1);
declare s character; /* a temporary used various places */
/4 the entries in prmask() are used to select out portions of coded productions and the stack top for comparison in the analysis algorithm 4/ declare prmask(5) fixed initial (0, 0, "fe", "ffff“, uffffffu, uffffffff");
/*the proper substring of pointer is used to place an i under the point of detection of an error during checking. it marks the last character scanned. 4/
declare pointer character initial ('
i');
declare callcount(20) fixed /4 count the calls of important procedures 4/ initial(0,0,0,010,0,010,010,010,0,0,0,0,010,010,0);
/4 record the times of important points during checking 4/ declare clock(5) fixed;
/4 commonly used strings 4/
declare x1 character initial("), x4 character initial(' ; declare period character initial ('.');
/4 temporaries used throughout the compiler 4/ declare (i, j, k, l) fixed;
declare true literally '1', false literally '0', forever literally 'while 1';
/4 the stacks dec-lared below are used to drive the syntactic
analysis algorithm and store information relevant to the interpretation of the text. the stacks are all pointed to by the stack pointer sp. */
declare stacksize literally '75'; 14 size of stack 4/
declare parse_stack (stacksize) bit(8); /4 tokens of the partially parsed text 4/
declare var (stacksize) character;/4 ebcdic name of item 4/
declare fixv (stacksize) fixed; /* fixed (numeric) value 4/
/4 sp points to the right end of the reducible string in the parse stack, mp points to the left end, and
mpp1 = mp+1.
4/
declare (sp, mp, mpp1) fixed;
declare
advance literally 'call advance_page1, aflagck literally 'call aflag check', emit literally 'call store_coue',
emita literally 'call emit address':
emitcar literally 'call stnre_ascii_code',
emitck literally 'call emit check',
consize literally '1023'; 7* size of constant arrays */
declare
aflag bit(8) initial(0), /* indicates processing real expressions */
at bit(8) initial(0): /* pointer to top of vcell_address table */
begin bit(8) initial(0), /* sets up jump to main if subprograms preceed */
buffer1 character, /* contains next card image to be scanned */
buffer2 character initial(' 1), /* temporarily holds fortran statement */
ca bit(16) initial(2845), /* points to next variable array space */
cb bit(16) initial(128), /* next instruction cell to be allocated */
cflag bit(8) initial(0), /* indicates call to subroutine */
code(4095) fixed, /* contains code for pdp-8 */
constant1(consize) fixed, /* lower 12 bits=exponent,next 12 octal location */
constant2(consize) fixed, /* mantissa or number < 409 */
ct bit(16) initial(255), /* top location of present page */
:dflag bit(8) initial(3), /* determines action for declarafions */
' dim(70) fixed, temporary array for subscripting */
dt fixed, /* pointer within dim array */
m ep fixed initial(128): /* used for code output while compiling */
to
entry fixed, /* contains procedure entry & function return */
fixm(stacksize) fixed, /* holds mantissa of real number */
(hold1,hold2) fixed, /* passes real numbers to compilation loop */
lab(253) fixed: /* contains label entries */
loc(stacksize) fixed: /code locations of variable or constant */
maxtcell bit(16) initial(62), /* max no. temp cells in use during compile */
next bit(8) initial(0), /* next parameter to be assigned this address */
nflag bit(8) initial(0): /* indicates next number in scan is a real */
page_base bit(16) initial(128), /* location zero of present page */
page_block bit(16) initial(128), /* updates labels within this block */
parm bit(8): /* if 1 then variable is a parameter */
parmcell bit(8) initial(127), /* pointer to pass parameter */
pc fixed initial(1): /*pointer into ptable & psymbol */
pcell bit(8) initial(16), /* next cell for procedure indirect addressing */
prt(383) fixed: /* holds addess,type,etc. of identifier */
p5ymbol(13) character: /* holds names of procedures */
pt fixed initial(147), /* addresses top of prt */
ptable(13) fixed: /* holds statistics cn procedures */ rflag bit(8) initial(0), /* checks for return statement in subprograms */ (save label_address,save_first,step,do_until) fixed, /* do state. variable */
sc bi7(8) initial(0): /* next common cell in sy:abol table */
sflag bit(8) initial(0), /* indicates scanning a subprogram */
5ize(255) bit(16): /* contains number of cells assoc. with variable */
st fixed initial(19), /* addresses top of symbol table */
stop bit(8) initial(1), /* checks for stop statement in main program t/
string fixed, /* token index for v set in initialize */
symbol(255) character, /* holds variable names *1
tcell bit(16) initial(62), /* points to next temporary cell */
type fixed, /* variable type, set in lookup & enter */
vcell_address(127) fixed; /* addresses of variables on other pages */
/* float subprogram converts an integer to real */
declare float(30) bit(16) initial(0,3648,3904,2723,3664,242,1574,3776,
1572,3716,4008,2731,754,3616,3857,550,3880,2738,1060,3592,2733,
550,3912,2743,3588,2739,3592,3905,1573,1574,2973);
/* write_string array contains two subprograms
subprogram number of instructions octal locations
tab 15 5474-5513
write_string 12 5514-5527 */
declare write string(27) bit(16) initial(0,956,1739,1212,2333,141,0,715,
4032,304,2333,160,0,1227,2755,0,0,3776,3110,972,1228,3880,3020,
3105,2771,3110,3776,2767);
/* integer to array contains two subprograms
subprogram number of instructions octal locations
integer output 27 5530-5562
-..)
c) integer input 13 5563-5577 */
declare integer 10(39) bit(16) initial(0,1573,549,4032,2786,549,3616,
516,1573,754,3857,1574,1572,549,3592,1060,3872,2790,549,3588,3904,
2795,3592,3905,1573,3032,2048,0,548,767,4032,2813,549,3656,1573,
1060,2804,549,3059,4085);
/* subprogs array contains four subprograms which are preloaped
along with floating point package(digital 8-5a-s). program
execution will overlay rim loader location 7756-7775 octal
subprogram number of instructions octal locations
array subscriptor 42 7600-7651
multiply 18 7652-7673
divide 28 7674-7727
exponentiate 22 7730-7755 */
declare 5ubprogs(109) bit(16) initial(0,896,1775,1152,1007,1776,1263,
563,3904,2703,3616,1774,750,750,750,1774,1264,2707,2721,1264,1007i
1777,1263,563,3616,516,1778,754,1266,2715,750,1774,2704,647,168t,
1007,3616,515,751,750,2688, 0,0,37761938,1774,1006,1774,1194,v38,
1775,1007,3616,1775,1194,1263,2746,2986,750,2743,0,3776,956,1774,
1006,1774,1212,956,1212,1775,1007,3616,516,3872,2764,3842,1775,
1776,750,751,1264,3036,2767,3904,e774,514,752,3004,0,3776,984,
1774,1006,3616,516,1774,1240,984,1775,1240,2311,3055,3311,0,2311,
1775,0,1262,2792,3032);
/* arrays a of 56 through 74 contain floating point package */
declare
a56(127) bit(16) initial(0,3776,1571,1575,896,1707,683,179,4008,
2700,689,128,1710,690,171,686,1710,692,171,4008,2711,942,
1710,1152,942/1568,686,1711,1199,943,1569,1199,943,1570,683,3654,
3590,176,693,1708,940,1708,2988,0,'00,0,0,15,3968,127,128,256,
2998,3042,3022,3039,3057,3269,3006,3013,3067,544,1572,545,1573,
546,1574,2689,548,1966,1198,549,1966,1198,550,1966,2689,2553,
2689,2554,3648,547,551,1575,3588,546,550,1574,3588,545,549,1573,
2552,2689,2529,2766,3328,683,176,3880,2944,752,1708,940,17081
0640,1709,2476,685,1664,2689,3428,3713,544,548,1572,2551,2689,
3217,3456,3088,3444,2552,2689,0,0,0),
a60(127) bit(16) initial(0,3776,551,3617,1575,550,3616,3864,3649,
1574,549,3616,3864,3649,1573,2944,0,549,4000,2711,550,4008,2783,
545,4000,2717,546,4008,2960,544,3617,548,3880,2764,3904,3617,1762,
738,739,4032,2734,2254,3864,744,743,2773,2254,3856,744,743,1764,
738,3617,996,2020,1252,740,1765,1253,741,1766,1254,3648,9961
3912,3600,3592,2020,997,3592,2021,998,35(.42,2022,1250,2750,1168/
2960,0,544,2617,548,3588,3712,3022,1764,996,1572,1252,996,1573,
1252,96,1574,2960,544,1572,2764,0,24,0,0,0,36,4092,2945,754,
1572,754,1573,3616,1574,1073,3049,2047,0,2311,3114,1578,0,3059/
0,3065,0,0,0,0,0),
a62(127) 811(16) initial(0,3776,549,3912,3664,3592,1573,550,3592,
1574,551,3592,1575,3648,1060,3584,2944,0,3776,755,1767,2270,546,
2030,550,2541,3712,1007,1575,549,2030,546,2541,551,1575,3588,
1007,1783,3588,1784,545,2030,550,2541,551,1575,3588,1007,759,
1783,3588,760,1784,549,2030,545,2541,759,1574,3588,1007,760,15731
2544,1575,1269,2961,2555,2961,545,4000,2763,546,4008,3068,544,
3617,548,3585,1572,754,1767,2270,2545,1007,545,3992,3535,1570,
1569,1269,2555,3037,3024,0,758,1781,549,4032,2790,2555,1269,
545,4032,3038,2553,1269,5584,3038,3359,3385,3381,3456,3386,4040,
4032,2945,0,4094,0,(j/3328,3344,3072,3178,0,0,0),
a64(127) bit(16) initil(0,3776,547,3617,1571,546,3616,3864,3640,
1570,545,3616,3864,3649,1569,2944,0,3776,545,3912,3664,3592,1569,
546,3592,1570,547,3592,1571,3648,2960,0,1718,1717,696,1719,2648,
694,3592,1718,693,3856,2733,3648,697,3592,1717,1207,2725,694,3592,
3648,2975,0,0,0,4084,0,0,3776,1577,740,1719,2758,550,3588,15741
549,3588,1573,546,550,1721,3588,549,.545,3856,2769,1573,697,1574,
3712,553,3588,1577,551,3588,1575,1207,2752,553,1574,549,3654,1717,
551,1573,1575,693,3002,4073,3187,3502,3193,3193,3193,3193,3193,
3193,3193,3193,3193,3193,3193,3193,3193,0,2192,2554,1056,3584,
3060,3200,0,0,0,0,0),
a66(127) bit( 16) inilial(0,3776,1709,1708,549,3°12,1196,4000,
2703,550,4000,2703,551,4008,2729,684,4000,2475,549,3652,4040,
2721,551,3652,1575,550,3588,1574,549,35r8,1573,1197,2706,685,
3617,548,1572,684,4000,2475,2944,1572,2944,3072,0,0,0,1778,2311,
3114,0,549,4040,2793,548,3648,3912,3600,3592,1774,3864,1262,3584,
753,1775,1776,555,4000,2760,556,4005,2796,2311,2602,2286,750,0,
3744,54c,1572,548,3617,750,4000,2789,549,3617,751,4000,2789,550,
3617,752,3904,3617,3585,4040,2789,754115p5,2990,2311,3310,0,2760,
2475,1266,2736,1572,2990,0,0,0,1549,0,0,0,0,0,0,558,4008,3063,
767,2558,3063,3812,138),
a70(127) bit(16) initial(0,3712,1573,1574,1717,1718,2274,734,3880,
2703,733,3872,2704,3744,1717,2274,3712,559,735,3904,2944,736,
3912,2944,1715,549,225,3872,2703,1072,1206,2209,2703,0,550,1713,
549,1714,1716,2231,2231,2243,2231,691,1713,1714,2243,692,2977,
0,0,0,0,0,0,0,3776,550,3588,1574,549,3588,15731692,3588,1716,299°,
0,3776,550,689,1574,3538,549,690,1573,3588,692,1716,3011,0,3776,
1205,3024,550,3617,1574,549,3616,3864,35p5,1573,3024,4094,3925,3910,
10,3968,0,3712,3097,2788,3102,1583,559,2550,55(:,3880,2787,760,
3880,3063,761, 4008,2549,559,3042,3575,3812,3841,3b41,114,0,0,0,0,0,0),
a72(127) bit(16) init1al(0,2191,724,1572,739,2276,2527,557,4008,
2944,737,2276,738,2276,2944,0,3776,549,4040,3728,727,3864,728,
2276,2282,729,2276,3776,549,4032,2722,3616,2011,2522,3744,548,1572,
1748,548,3904,2739,726,4032,2744,2311,2016,0,3744,724,1748,2726,
2311,1786,0,1236,2726,2012,2526,2525,3848,2287,1060,2748,3380,
2761,2282,725,1572,2525,2282,1060,2756,2959,3744,724,1748,549,
4000,2770,550,4008,1748,3744,2754,0,4090,4,171,2,174,3664,3637,3635,
3617,3639,3922,3908,141,138,197,0,3105,2789,3110,3712,3044,0,
750,2276,3050,176,0,3656,1764,549,3592.1973,590,3592,1574,740,
3055,4093,1638,1639,0,0,0),
474(127) bit(16) initial (0,3744,1739,1584,2510,3712,559,714,4000,
2704,715,4008,2706,2001,1739,3023,715,4008,977,3617,1740,2512,
713,1572,2311,3584,3114,0,559,712,4000,2730,2510,2512,549,3912,
3585,4000,2751,550,716,1740,2311,2602,0,716,3380,2944,4032,2744,2311,
1991,0,1228,2733,2944,2311,1732,0,3744,716,1740,2733,717,1572,
717,1573,2944,4,1280,0,3834,3899,23,3q22,0,0,2047,3584,35q9,
3664,3638,0,3776,548,3912,3633,1572,758,3864,759,2556,1573,548,1061,
760,3904,2782,761,1572,3616,549,3872,2556,1573,548,1061,762,3904,
2794,763,1574,3744,549,2556,550,2556,3026,4091,2,3996,100,4086,
10,3818,0,0,0);
/* procedures *1
i format:
— procedure (number, width) character;
declare (number, width, l) fixed, string character;
string = number;
l = length(string);
if l >= width then return string;
else return su6str(x70, 0, width—l) 11 string;
end i_format;
read octal:
p7ocedure(n) fixed;
/* octal binary n changed to octal number representation */ declare (m,n) fixed;
m=(shr(n,9) & "7") * 1000; m=m+ushr(n16) & "7") * 100); m=m+((shr(n,3) & "7") * 10); m=m+(n & "7"); return m; end read octal;
list card:
procedure(s);
/* prints the fortran statement */
declare (c,s,rest) character, i fixed;
if buffer2=" then do; buffep2=s;
return; end;
c=buffer2; buffer2=s;
card count=card count+1;
if m—a—rgin chop 5 0 then
do; /*—the margin control frcm dollar i */
i=length(c) — margin chop; rest=substr(c,i);
c=substr(c,o,i); end;
else rest=' ';
-..] if control(ryte('m')) then output=c;
w
else if control(pyte(,l1)) then
output=i format(card count,4) ii '1' 11 c 11 '1' 11 rest; if control(bytr(sc')) then doi=ep to cb—i;
ep=code(i); c=' iiipead nual(i)11' ,.
,
if ep<4096 then output=ciiread octal—cep); else if ep=4096 then output=ci p----fext,; else if ep=4097 then output=c 11---.twext';
else 0utput=c111----label= '1 lab(shr(ep,16)); end;
. cp=0; ep=cb;
end list_card;
error:
procedure(msg, severity);
/* prints and accounts for all error messages *(
/* if severity is not supplied, 0 is assuled */
declare msg character, severity fixed;
call list card("); /* force the listing of the card in buffer2 */ error_count = error count + i;
/* if listing is suppressed, fopce printing of this line */ if , control(byte('l')) then
output = i format (card_count, 4) ii ' ii 11 buffer 11 '1'; output = substr(pointer, text_limit—cp+margin_chop);
output = 1*** error, 11 msg ii
last previous error was detected on line ' 11 previous_error 11 '• ***1;
previous error = card_count;
if severity > 0 then
if severe_errors > 25 then
do;
output = 1*a* too many severe errors, checking aborted ***1; compiling = false; end;
else severe_errors = severe_errors + i;
end error;
1* card image handling procedure */
get_card:
procedure;
declare (flag,!) fixed, c character;
/* flag holds count of continuation cards - i determines where to insert ; for end of statement */
1=72; flag=0; buffer=buffer1;
dc forever;
c=substr(buffer,0,1);
if c=" then
do forever;
buffer1=input;
if (substr(buffer1,0,1)='c') 1 (substr(buffer1,5,1)=4 ') then do; if flag=0 then call list card(buffer); buffer=substr(buffer,o,it 11 1;t
 text=buffek; text_limit=length(text) -1; return;
end; /* else a continuation */
if flag=0 then call list_card(buffer);
else if flag >2 then
do;call error('only 2 continuations allcwed112); return; end;
call list_card(3uffer1);
buffer=su6str(buffer,o,i) 11 sm3str(buffer1,6,66); flag=flag+1; 1=1+66; end;
if c='c' then call list card(buffer);
else if substr(buffer70,3)='$g0' then
on; call list card(buffer); text='eof; ';
tixt_limit-leng1h(11x1)-1; bu1ffr1='$101; '; return; end;
else if substr(buffer,0,4)='$e0f, then
do; text='eof; '; text limit=length(text)-1; return; end;
else do; i=byte(buffer,1); ceinirol(i)= ,ccntrol(i);
call list card(buffer); 1=72; end; buffer,8uffer1=input; end;
end get_card;
/* the scanner procedures */
compute:
procedure (m,n,p);
/* converts decimal fractions to octal exponential (real) returns exponent in hold1 and mantissa in hold2 */ declare (i,j,k,l,m,n,p) fixed, c character;
j=0;
do 1=1 to 8;
c=n * 8;
if length(c)>m then do; k=byte(c) - "f0"; j=shl(j,3) 1 k;
do l=2 to length(c); k=k*10; end;
n=n * 8 - k; end;
else do; n=n * 8; j=shl(j,3); end; end;
n=shl(j,8); j=1;
do i=0 to 30;
if p<j then go to finish; j=shl(j,1); end;
finish: hold1=1; hold2=shr( (shr(n,i) 1 shl(p,32-i) ),9); if number. value > 0 then return;
/* have to reset for number < 0.5 4/
if shr(hold2,22)=1 then return;
hold1=4095; hold2=shl(hold2,1);
do i=1 to 24;
if shr(hold2,22)=1 then return;
hold1=hold1-1; hold2=shl(hold2,1); end;
call errorpunable to convert numberi,21;
output=,number= '1inumber_value111.'11substr(text,cp-m,m); end compute;
scan:
procedure;
declare(s1,s2,s3) fixed;
callcount(3) = callcount(3) + 1;
failsoft = true;
bcd = number_value = 0;
scan 1:
dc forever;
if cp > text_limit then call get_card;
else
do; /* discard last scanned value */
text_limit = text_limit - cp;
text = substr(text, cp);
cp = 0; end;
/* branch on next character in text *1 do case chartype(byte(text));
/* case 0 */
/* illegal characters fall here */
call error (*illegal character: ' ii substr(text, 0, 1));
/* case 1 */
/* blank */
do;
cp = 1;
do while byte(text, cp) = byte(") & cp <= text_limit; cp = cp + 1; end;
cp = cp — 1; end;
/* case 2 */
; /* not used in skeleton (but used in xcom) */
/* case 3 */
do; /* locate string and place in bcd */
token=string;
cn do cp=cp+1 to text_limit;
s1=byte(text,cp);
if 51=125 then do; cp=cp+1; return; end;
bcd=bcd if substr(text,cp,1); end; end;
/* case 4 */
do forever; /a a letter: identifiers and reserved words */
do cp = cp + 1 to text_limit;
if not_letter_or digit(bytf(text, cp)) then
do; /* end cr identifier */
if cp > 0 then bcd = bcd if substr(text, 0, cp); res_word: s1=length(bcd);
if si > 1 then if 51 <= reserved limit then
/* check for reserved words *7
do i - v_index(si-1) to v_index(s1) — 1;
if bcd = v(i) then
do;
token =
return; end; end;
/* reserved words exit higher: therefore <identififr>/ token = ident;
return; end; end;
/* end of card */
bcd = bcd ii text;
call get_card; cp = —1; end;
/* case 5 */
do; /* digit: a number */ token=number; nflag=0;
do forever;
do cp=cp in text_limit;
s1=byte(text,cp);
if s1=byte(,.,) then
do; cp=cp+1; s2,53=0; nflag=1; dc, forever;
do cp=cp to text limit;
s1=byte(text,05);
if 51 < "fo" then
do; if number value + s3 = 0
then huld1,hdld2=0; /* value is 0.0 */
else call compute(s2,s3,number_value);
return; end;
52=52+1; 53=10 * 53 + si - "fo"; end;
call get card; end; end; if si < "fo" then return;
number_value=10*number value + si - "f0"; end;
call get_card; end; end;
/* case 6 */
; /* case 6 not utilized */
/* case 7 */
do; /* special characters */ token = tx(byte(text));
cp = 1;
return; end;
/* case 8 */
/* not used in skeleton (but used in xcom) */
/* case 9 */
/* determination of period */
do; bcd=,.,; cp=cp+1;
do forever;
if byte(text,cp) = byte(.') then
do; bcd= bcd 11 i.,; cp=cp+1;
go to kes word; end;
else if byte(text,cpt -= byte(" ) then bcd=bcd iisubstr(text,cp,1); cp=cp+1; end; end;
end; /* of case on chartype */
cp = cp + 1; /* advance scanner and resume search for token */
end;
end scan;
/* time and date */
print time:
procedure (message, t);
declare message character, t fixed;
message = message 11 t/360000 ii 1:1 ii t mod 360000 / 6000 11 t mod 6000 / 100 11 1.';
t = t mod 100; /* decimal fraction */
if t < 10 then message = message 11 '0';
output = message 11 t 11 1.1;
end print_time;
print_date_and_time:
procedure (message, d, t);
declare message character, (d, t, year, day, m) fixed;
declare month(11) character initial ('january', 'february', 'march', co 'april', 'may', 'june', 'july', 'august', 'september', 'october', 'november', 'december'),
days(12) fixed initial (0, 31, 60, 91, 121, 152, 182, 213, 244, 274,
305, 335, 366);
year = d/1000 + 1900;
day = d mod 1000;
if (year & "3") ,= 0 then if day > 59 then day = day + 1; /* leap year*/
m = 1;
do while day > days(m); m = m + 1; end;
call print_time(message ii month(m-1) ii x1 11 pay—days(m-1) 11 ',
ii year 11 '. clock time = t);
end print_date_and_time;
/* initialization
initialization:
procedure;
eject page;
call print date_and ti"e (' syntax check -- stanford university -- skeleton
iii version tjf 1, datlof_generation, time_of_generation);
double space;
call print_date_and_time ('today is i, date, time);
double_space;
do i = 1 to nt;
s = v(i);
if s = '<number>' then number = i; else if s= '<string>, then string=i; else
if s = '<identifier>, then ident = i; else
if s = '/' then divide = i; else
if s = ' ' then eofile = i; else
if s = 11-1 -then stopit(i) = true; else
end;
if ident = nt then reserved_limit = length(v(nt-1));
else reserved limit = length(v(nt));
v(eofile) = 'eof';
stopit(eofile) = true;
chartype(byte(")) = 1;
do i = 0 to 255;
not_letter_or_digit(i) = true;
end;
do i = 0 to length(alphabet) - 1;
j = byte(alphabet, 1);
tx(j) = i;
njt_letter_or_digit(j) = false;
chartype(j) = 4;
end;
do i = 0 to 9;
j = byte('0123456739'7 i);
not letter op_digit(j) = false;
cha7type(jt = 5;
end;
do i = v_index(0) to v_index(1) - 1;
j = byte(vii));
tx(j) = i;
chartype(j) = 7;
end;
chartype(byte('/')) = 6;
/* first set up global variables controlling scan, then call it */ cp = 0; text_limit = -1;
text =
control(byte('c1))=false; /4: output code while compiling */ control(byte('l,)) = true;
control(byte('p'))=false; /* output productions */ control(byte(,r,))=false; /* output compiled code */ control(byte('s'))=false; /* output for syoccl table */
control(byte('t,))=false; /* output procedure table */ chartype(byte(1.'))=p;
chartype(byte('/'))=7;
chartype(125)=3; /* single quote */
buffer1=input;
do i=0 to 126; prt(i),lab(i)=0; lab(i+127),vcell_address(i)=0;
end;
do i=0 to consize; constant1(i),constant2(i)=0; end;
do i= 0 to 2876; code(i)=0; end;
code(2)=4095; code(3)=2; code(4)=1; constant1(1)=shl(4,12); constant2(1)=1;
constant1(2)=shl(3,12); constant2(2)=2;
1=4095 mod consize; constant1(i)=shl(2,12); constant2(i)=4095; code(5)=3712; /* 7200 octal */
code(6)=3840; /* 7400 octal */ code(7)=2944; /* 5600 octal */ code(27)=2845;
code(28)=2876; /* tab routine */ code(29)=2892; /* write string routine */
code(30)=2904; /* integer write routine */
code(31)=2931; /* integer read routine */
code(45),code(46)=4095; /* floating point i/o initialization */ code(50)=3968; /* 7600 octal array-subscriptor subprogram */ code(54)=4010; /* 7652 octal-multiplier */
code(55)=4028; /* 7674 cctal-divider */
code(56)=1; code(57)=3072; /* neg 1 in floating point */ code(61)=4056; /* 7730 octal-exponentiatnr */
dj i=0 to 30; code(i+2845)=float(i); end;
do i=0 to 27; code(i+2876)=write_string(i); end;
do i=0 to 39; code(i+2904)=integer io(1); end;
do 1=0 to 127; /* l3ad floating potnt package */
code(i+2944)=a56(i); code(i+3072)=a60(i);
code(i+3200)=a62(i); code(i+3328)=a64(i);
code(i+3456)=a66(i); code(1+3584)=a70(i);
code(i+3712)=a72(i); code(i+3840)=a74(i); end;
do i=0 to 109; code(i+3968)=subprogs(i); end;
/* set initial jump to main program */
code(128)=2945; /* jmp i 201 */ code(129)=130; cb=c8+2; . call scan;
/* initialize the parse stack */ sp = 1; parse_stack(sp) = eofile;
end initialization;
dump it:
procedure; /* dump out the statistics collected during this run */ double space;
/* put out the entry count for imp7irtant procedures */
output = 'stacking decisions= ' callcount(1);
output = 'scan = callcount(3);
output = 'free string area = freelimit - freebase; end dumpit;
stack_dump: procedure;
declare line character;
line = 'partial parse to this point is: ';
do i = 2 to sp;
if length(line) > 105 then
do; output=line;
line=x4; end;
line = line 11 x1 11 v(parse_stack(i)); end;
output = line;
end stack_dump;
/* procedures for synthesize */ advance page:
procedure (n);
/* shift code generation to next page if insufficient rcom
available on present one */
declare(n) fixed;
if (ct-cb-n-aflag) < 1 then do;
if aflag then do; code(cb)= 0; /* fext */
00
cb=cb+1; end;
code(cb)=2944+cb+1-page_base; /* 2944=56xx octal, a jump */ page_base=page_base+128; code(cb+1)=page_base; cb=page_base; ct=page base+127;
do i=0 to at; vcell_address(i)=0; end; at=0;
if aflag then do; code(cb)=2311; /* j•s i 7 */ cb=cb+1; end; end; end advance_page;
get acell: procedure(n) fixed;
/* returns next array cell block, starting top page 27 */
declare n fixed;
ca=ca-n; return ca;
end get_acell;
get pcell: procedure fixed;
/* sets core location for procedure m from 20 to 36 octal */
if pcell=27 then do; call error('too many subrrograms',2);
return; end;
pcell=pcell+1; return pcell-1;
end get_pcell;
get tcell:
procedure (n,t) fixed;
/* returns temporary storage from page zero depending on
whether that location contains an address or holds a
real or integer value */
declare (n,t) fixed;
loc(n)=tishl(tcell,4);
if tcell > parmcell then call error
('parameter storage overlayed,separate into additional statements',2); if t=1 then do; tcell=tcell+3;
if tcell>maxtcell then maxtcell=tcell;
return tcell-3; end;
if tcell+1 > maxtcell then maxtcell=tcell+1;
tcell=tcell+1; retupn tcell-1;
end get_tcell;
get vcell:
procedure(n) fixed;
/* returns next n cells from top of present page if possible */
declare n fixed; advance(n+1);
ct=ct-n; return ct+1;
end get_vcell;
store code:
procedupe (n) fixed;
/* store n in next available instruction location of code array
returns code location */ declare n fixed;
if aflag then advance(2); else advance(1); code(cb)=n; cb=cb+1; return cb-1;
end store_code;
aflag_check:
procedure;
/* sets aflag and emits jms to intrepter if aflag=0 */ if aflag=1 then return; advance(2); aflag=1;
if code(cb-i)=4096 then cb=cb-1; /* remain in intrepter */ else emit(2311); /* jms i 7 */
end aflag_check;
emit check:
p7ocedure(op,l);
/* check if indirect addressing required: op=operation,loc(l) */ declare(op,l,p,m) fixed;
p=shr(loc(l),4);
if p<128 then do;
if (loc(l)4")>0 then emit(pishl(op,9)ishl(1,8));/* indirect address 'v else emit(pishl(op,9)); /* direct address */
return; end;
if(p >= page base) (p < page_base+128)
then do;=--(p—page_base)ishl(1,7); /* variable on present page .*/ emit(mishl(op,9)); return; end;
/* check if address of variable on current page */
do i=0 to at;
if (vcell_address(i)&affff")=p then de; p=shp(vcell_address(i),16);
m=(p—page_base)ishl(3,7);
em1t(mishl(op,9)); return; end; end; /* enter reference to variable on current page and indirect address */
m=get_vcell(1); code(m)=p; vcell_addess(at)=shl(m,16)1p;
at=at+1; m=m1shl(3,7); emit(mishl(up,9));
end emit_check;
store_ascii_code:
procedure(n);
/* checks if write string subprogram still engaged and emits ascii character for teletype output */
declare n fixed;
if code(cb-1)=4097 then do; cb=cb-1;
advance(2); end;
else do; advance(3);
co
emit(2333); /* jms i 35 */ end; emit(n);
emit(4097); /* wext */
end store_ascii_code;
emit address:
ptocedure(l,back);
/* if l contains an address load the addressed space in the next position else load the address */ declare(l,back,m) fixed;
m=shr(loc(l),4);
if (loc(l)c"4")=0 then do;emit(m);
return; end;
do 1=0 to back-1; code(cb—i+1)=code(cb—i-1); end; code(cb—back)=512+m; code(cr—back+1)=1664+cb—page bas-1-2; if back=2 then if code(cb-3)=(1664+cb—page_base-1t then
code(cb-3)=cope(cb-3)+2;
cb=cb+3;
end emit_address;
emit string:
ptocedure(c);
/* output string c */
declare c character, (i,l,n) fixed; do i=0 to length(c)-1; n=byte(c);
if n=64 then emitcar(160); /* space */
else if n>192 & n<202 then
else if n>208 & n<218 then
else if n>225 & n<234 then
else if n>239 & n<250 then
else do;
emitcar(n); /* letter a - i */ emitcar(n-7); /* letter j - r */ emitcar(n-15); /* letter s - z */ emitcar(n-64); /* decimal digit */
call error('illegal string symbgl '11substr(c,0,1),2); found: end; c=substr(c,1); end;
end emit_string;
store constant:
procedure(n102,reo) fixed;
/* store constant in req number of cells-returns code location declare(l,n1,n2tm,req) fixed;
if (n1+n2)=0 then return 58; /* zero constant core location */ l=(nl+n2) mod consize;
do forever;
if constant2(l)=n2
then if (constant1(l) & "fff")=n1 then return shr(constant1(l),12);
if constant2(l) + constant1(l)=0 then go to enter;
if l=consize then l=0;
else l=l+1; end;
enter: /* constant not located - insert constant */ m=get_vcell(re0); constant2(l)=n2; constant1(l )=n1 1 shl(m,12);
if req>1 then do; code(m)=n1 & "fff"; code(m+1)=shr(n2,12);
 code(m+2)=n2 & "fff"; end;
else code(m)=n2 & "fff"; return m;
end store_constant;
set:
procedure(c) fixed;
/* return based on i thru n being integers
integer=0 , real-1 */
declare c character, k fixed;
k=byte(c);
if (k > 200) & (k < 214) then return 0; /* an integer */ else return 1; /* a real */ end set;
*1
/* fortran string character 1 u */
do j=0 to 14; /* check for " # $ % & ' ( ) * + - • / */
if substr(c,0,1)=substr(',1#$%&"()*+,-./' 1j71)
then do; emitcar(161+j); go to found; end; end; do j=0 to 6; /* check for : ; < = > ? @ */
if substr(c,0,1)=substr(':;<=>?@' ,j,1)
then do; emitcar(186+j); go to found; end; end;
hash:
procedure(c,l) fixed;
/* l= number of characters in c
returns entry into hash section of prt */
declare c character, (i,k,l) fixed;
k=l; i=0;
do while (i <= 2) & (1 < l); k=shl(k,8)1byte(c,i); i=i+1; end; return k mod 127;
end hash;
lookup:
procedure (c) fixed;
/* find identifier c and return prt location */
declare c character, (n,l) fixed;
l=hash(c,lfngth(c));
if prt(l)=0 then return 0; l=prt(l);
dc forever;
if symbol(prt(l) & "ff")=c then do; type=shr(prt(l),30); par1=shr(shl(prt(l),2),31); return l; end;
enter:
procedure (c,t) fixed;
/* c is identifier of type t to enter in prt, returns prt location */ declare c character, (l,m,n,t) fixed;
n=length(c);
if n > 6 then
do; output='* * * warning * *t;
call error('identifier length exceeds 6 characters',0); c=substr(c,0,6); end;
if st>255 then call error('symbol table limit exceeded1,21; l=hash(c,n);
 if prt(l)=0 then do; st=st+1; ppt(l)=pt; prt(pt)=st 1 shl(t,30);
symbol(st)=c; pt=pt+i; type=t; if type-0 then size(st)=1;
else size(st)=3; return pt—i; end;
l=prt(l); /* a collision occurred
do forever;
m=shr(shl(prt(l),3),23);
if m=0 then do; ppt(l)=prt(l) i shl(pt,20); st=st+1;
prt(pt)=st 1 shl(t,30); symen(st)=c; pt=pt+1; type=t;
if type=0 then size(st)=1;
l=shr(shl(prt(l),3),23);
co if prt(l)=0 then return 0; end; (r)
end lookup;
else size(st)=3; return pt-1; end;
l=m; end;
end enter;
enter1:
procedure(c,n) fixed;
/* enters variable in prt and gets code storage for that variable returns code location */
declare c character, (l,m,n) fixed;
m=enter(c,set(c));
if shr(prt(m),3°)=1 then do; l=get_vcell(3); loc(n)=shl(l,4)11; end; else do; l=get_vcell(1); loc(n)=shl(l,4)10; end; prt(m)=prt(m) 1 shl(l,8);
return m;
end enter1;
common check:
prozedure(l);
/* relocates variables into common section of prt
l refers to stacksize pointer (mp to sp) */
declare(l,m,p) fixed;
p=lookup(var(l));
if p=0 then
do; call error('common variable: '11var(l)111 /must be known',2); output='**place variable in a declaration statement'; end; if p<147 then return; /* already relocated */
if sc>19 then call error('excessive common variables'/2);
do 1=0 to 126; /* locate hash entry */
if prt(i)=p then 1=i; end;
if m=—i then do;
. call error('common variable: '11var(l)111 ,cannot be relocated',2); m=hash(var(l),len(th(var(l)));
output='***rename variable: 1110symbol(prt(prt(m))auff"); eno;
prt(m)=5c+127; symbol(sc)=var(l);
syabol(prt(p) a "ff")=1zzzzzzz'; /* retain for collision */ prt(sc+127)=(prt(p) a heo°fff°0") +sc; size(sc)=size(p-127); sc=sc+1; prt(p)=prt(p) aulffoooffu;
end common_check;
find proc:
p7ocedure(c) fixed;
/* locate identifier c in procedure table — returns location */ declare c character, i fixed;
do i=1 to pc; if psymbol(i)=c then return i; end;
return 0;
end find_proc;
set proc;
trocedure(cit) fixed;
/* place identifier c of type t in procedure tables-returns location */ declare c character, t fixed;
psymbol(pc)=c; ptable(pc)=shl(t,29) i shl(get pcell,12);
ptable(pc)=ptable(pc) i parmcell; pc=pc+1; return pc-1;
end set_proc;
find label:
procedure (n) fixed;
/* ensures entry for label n in label array
returns location in label array */
declare(l,cnt,n) fixed;
if n=0 then call error(1label zero cannot be used',2); l=n mod 127; cnt=0;
dc forever;
if (l=0) i (l=127) then l=1;
if lab(l)=0 then do; lab(l)=n; return l; end; if lab(l)=n then return l;
l=l+1; cnt=cnt41,
00
if cnt=127 then call error('excessive labels',2); end;
end find_label;
set lab:
procedure(l,position) fixed;
/* returns relative position cf code within present page
where future label insertions shall be made */
declare(l,positiono) fixed;
advance(2); m=get vcell(1);
code(m)=shl(l,16) t position;
return m-page_base;
end setlab;
subscript:
procedure(t,l) fixed;
/* compute subscript of array in code block with base l of type t subscripts passed in dim array */
declare(l,n,t,sum) fixed;
dt=dt+1;
n=406 - code(l); /* number cf dimensions */
if t<3 then t=1; /* integer arrty else real array */ sum=n+1+dim(dt+n-1)*t+l-code(l+n);
dt=dt+1;
if n=3 then do; sum=su1+(dim(dt)*code(l+1)+dim(dt-1)*code(l+2))t; dt=dt+2; end;
else if n=2 then do; sum=sum+(dim(ct-1)*t#code(l+1));
dt=dt+1; end;
return sum; end subscript;
insert check:
procedure(t);
/* checks data input for assignment compatability */ declarf(t) fixed;
if fixm(mp+1) > 0 then do; if t=1 then return; end;
else do; if t=0 then return; end; call error('variable and number type must agree',2);
end insert_check;
insert data:
procedure;
/* insert data into previously saved variables of dim array */ declare l fixed;
dim(0)=dim(0)+"100"; /* number counter */
type=dim(dt) & "f";
l=shr(dim(dt)18); /* core location */
do case type;
/* case 0 integer variable */
do; code(l)=fixv(mp+1); dt=dt+1; call insert_check(type); end; /* case 1 real variable */
do; call insert_check(type); go to insert real; end;
/* case 2 integer array */
do; l=subscript(type,l); call insert_check(type-2);
code(l)=fixv(mp+1); end;
/* case 3 real array */
do; l=subscript(type,l); call insert_chfck(type-2);
insert_real: code(l+1)=shr(fixn(mp+1),12) & "fff";
code(l)=fixv(mp+1); code(l+2)=(fixm(mp+1) & "fff"); end; end; end insert_data;
dumping:
procedure(from,last);
/* output prt cells from to last */
declare b character, (from,i,j,last,m,n,p) fixed; b='
do i=from to last;
output="; if symrol(i)-='zzzzzzz' then do; p=lookup(symbol(i)); m=read octal(shr(prt(p) & "fff00",8)); /* location */
if typ-e>1 & parm=0 then n=4096-code(shr(prt(p) & "fff00",8));
else no
output=' iii sym9cl(i) jibil i 11e11 m iibii parm it b ii
5hr(shl(prt(p),3),23)iibi typeiibiiniibiipiibiisize(p-127); end; end;
end dumping;
dump:
procedure;
/* list prt and symbol table entries */
double_space;
output='* * * combined prt/symbol table dump * * *i; output="; output=' identifier symbcl# base parameter 'ii
'collision type #dim prt location cells'; call dumping(0,sc-1);
call dumping(20,st);
end dump;
/* the synthesis algorithm for xpl
synthesize:
procedure(production_number);
declare production_number fixed;
declare (h,i.m,n,p,z) fixed;
/* this procedure is responsible for the semantics (code synthesis), if
any, of the skeleton compiler. its argw4ent is the number cf the co
production which will be applied in the pending reduction. the global variables mp and sp point to the bounds in the stacks of the right part of this production.
normally, this procedure will take the form of a giant case statement on production_numbep. however, the syntax checker has semantics (the termination of checking) only for production i. */
do case production number;
/* production zero */
/* .<master program> ::= <program> */
dc;if maxtcell > parmcell then call error
('parameter storage overlayed 8y temporary cells',2);
if control(byte('tm then do; ocuble_space;
output=, output='* * * procedure table * * *'; output=, ';
output=, procedure name type included=1 #parameters'il
parameter base core location';
do i=1 to pc-1; m=shr(shl(ptable(i),4),28);
if m=0 then n=0; else n=shr(shl(ptable(i)120),20);
output="; output=' ,iipsympol(i)11'
shr(ptable(i),29)1i" ,iishr(shl(ptable(i),3),31)ii
'i mi i' 'iinii° '1 i
read_octal(shr(shl(ptable(i),8),20)); end; end;
compiling=false; double space;
if stop then call error(sstop statemeni missing in main program1,0); if begin=0 then call error('missing ma*in program',2); n=control(byte('c')); fp=o;
if n then output="location code';
do i=0 to page_base+127;
m=code(i);
if m=4096 1 m=4097 then code(i)=0; /* reset fext & wext instructions */ if n then output=' '11read_octal(i)11' 111read octal(code(i));
if code(i)>4095 then call error('code emission error',?); end; do i=page base+128 to 2845;
if code(i)>4o95 then call error('code emission error' 2); if n & code (i) ,= 0 then output=' '11read_octal(i)11'
read_octal(code(i)); end; end;
/* <program> ::= <statement block> eno ; */
do;
prog:
do i=page block to page_base+127; /* backstuff labels */ m=shr(tode(i),16);
if m,=0 then do;
if lab(m+127)<2 then call error('missing label '11lab(m),1); if (code(i)&"1")=0 then n=shr(lab(m+127),16); /* front */
else n=lab(m+127)&"ffff"; /* rear */ code(i)=n; end; end;
page block=cb;
co
if cit > 2944 then call error
('program too large. arrays and floating point package overlayed1,0); if control(byte('s')) then call dump;
do i=0 to 126;
lab(i),lab(i+127)=0; if prt(i)>146 then prt(i)=0; end;
st=19: p1=147; eject_page; end:
/* <program> •:= <program> <statement block> end ; go to prog;
/* <statement block> ::= <statement list> */ begin=1;
/* <statement block> ::= <declaration list> <statement list> begin=1;
/* <statement block> ::= <procedure block> <statement list> */
do; if begin=0 then code(129)=cb; sflag=0;
if rflag then call error('missing return1,2); end;
/* <statement list> ::= <statement> ; */
tcell=62;
/* <statement list> ::= <statement list> <statement> ;
tcell=62;
*/
*/
/* <statement> ::= <if statement> */
/* <statement> ::= <basic statement> */
/* <statement> ::= <do statement> */
/* <statement> ::= <labeled statement> *1
/* <basic statement> ::= <assignment statement> *1
/* <basic statement> ::= <go statement> */
/* <basic statement> ::= <subroutine call> */
/* <basic statement> ::= <read statement> */
/* <basic statement> ::= <write statement> */
/* <basic statement> ::= return */
 do;m=entry & uffff,,; /* procedure entry point */
 if (m >= page_base) c (m < page_base+128) then n=1;
 /* if n=1 then entry on present page */ else n=o;
 if shr(entry,16)=0 then
 do; if n=1 then emit(2944+%1—page_base); /* jmp i */
 else do; advance(4);
 e4it(896+cb—page base+3); /* tad i */
 emit(1564+cb—paclbase+2); /* dca */
 emit(2044+cb—page_base+1); /* jmp i */
code(cb)=m; c3=cb+1; end;
rflag=0; return; end;
/* else must return function variable address during return */ if n=1 then do; advance(3);
emit(896+cb—page_fase+2); /* tad */ emit(2944+m—pagf_base); /* jmp i */ code(cb)=shr(entry,16); cb=cb+1; eno;
else do; advance(6);
emit(89(+cb—pace base+4); /* tad i entry */ fmit(1664+cb—pagr_base+3); /* nca */ emit(640+cb—page_pase+3); /* tad function address */
emit(2944+cb-page_base+1); /* jmp i */ code(cb)=m; cb=cb+2;
code(cb-1)=shr(entry,16); end;
rflag=0; end;
/* <basic statement> ::= stop */ do;emit(3842); /* hlt */
if sflag=0 then stop=0; end;
/* <if statement> ::= <arithmetic if> <double label> <number> */
do;if (loc(mp)&,1")=1 then do; advance(4);
emit(2311); /* jms i 7 */ emitck(5,mp); /* fgft */
emit(0000); /* fext */
emit(549); /* tad 45 */
p=get tcellcip,1);
emit(1536+p); /* dca */ end;
else emitck(1,mp); /* tad */
advance(5);
emit(4032); /* sma cla */
emit(2688+cb-page_base+3); /* jmp i */ emit(2944+cb-page base+1); /* jmp i */ code(cb)=loc(mp+11-cuffff0000"; /* expression < zero */
cb=cb+1;
emitck(1,mp); /* tad */
advance(5);
emit(4000); /* sza cla */
emit(2944+cb-page_base+3); /* jmp i */ emit(2944+cb-page_base+1); /* jmp i */ code(cb)=shl(loc(mp+1),16); /* expression = zero */ code(cb+1)=shlifind_label(fixv(sp)),16); /* expression > zero */ cb=cb+2; end;
/* <if statement> ::= <logical if> <basic statement> code(loc(mp))=cb;
/* <arithmetic if> ::= <if> <expression> */ loc(mp)=loc(mp+1);
/* <if> ::= if ( */
/* <double label> ::= ) <labell> <labell> */ loc(mp)=shl(loc(mp+1),16) i loc(sp);
/* <label1> ::= <number> 7 *1
loc(mp)=find_label(fixv(mp));
/* <expression> ::= <term> */
if aflag then do; emit(4096); /* fext */ aflag=0; end;
 /* <expression> ::= <expression> + <t5rm> */
do;z=(loc(mp) & "1") + (loc(sp) a do case z;
pc case 0 both integers */ do; emitck(1,mp); /* tad */
emitck(1,s0); /* tad */
p=get_tcell(mp,0);
emit(1536+p); /* dca */ end;
/* case 1 illegal */
call error(saritwaetic type incompatable,72);
/* case 2 both real */
do; aflagck;
emitck(5,mp); fget */ emitck(1,sp); /* fade) */ p=get_tcell(rp,1);
emit(3072+p); /* fput */
emit(4096); /* ffxt */ 4flag=0; end; end; end;
 /* <expression> ::= <expression> — <term> */
do;z=(loc(mp) c"1") + (loc(sp) & do case z;
/* case 0 both integers */
do; elitck(1,sp); /* tad */ emit(3616); cma */
emit(51o); /* tad 4 */ emitck(1,1p); /* tad */ p=gft_tcell(mp,0);
emit(1536+p); /* dca */ end;
/* case 1 illegal */
call error(1arithmetic type incompatable,,2);
/* case 2 both real */
00; aflagck;
emitck(5,mp); /* fget */ e1itck(2,sp); /* fsub */ p=get_tcell(mp,1);
emit(3072+p); /* fput */
emit(4096); /* fext */ aflag=0; end; end; end;
/* <expression> ::= + <term> */
do;loc(mp)=loc(mp+1); emit(4096); /* fext */ aflag=0; end;
/* <expression> ::= — <term> */ do;if (loc(sp) & "1")-=0
then do; fmitck(1,sp); /* tad */
emit(3616); /* cma */ emit(516); /* tad 4 */ p=get_tcell(mp,o);
emit(1536+p); /* dca */ end; do; aflagck;
emit(2618); /* fget 72 */ emitck(2,sp); /* fsub */ p=get_tcell(mp,1);
emit(3072+p); /* fput */
emit(4096); /* fext */ aflag=0; end; eno;
/* <term> ::= <primary> */
/* <term> ::= <primapy*> <primary> do;z=(loc(mp) & "1") + (loc(sp) & do case z;
/* case 0 both integers */ do; advance(7);
emit(2358); /* jms i 66 */ emita(mp,1); /* address */ emita(sp,2): /* address */ p=get tcell(mp,0);
emit(1536+p); /* dca */ end; /* case 1 illegal */
call errop(garithmetic type incompatable1,2);
/* case 2 both real */
do; aflagck;
emitck(5,mp); /* fget */ emitck(3,sp); /* fmpy */ p=get_tcell(mp,1);
emit(3072+p); /* fput */ end; end; end;
/* <term> ::= <term> / <primary> */ do;z=(lcc(mp) & "1") + (loc(sp) & "1"); do case z;
/* case 0 both integers */ do; advance(7);
emit(2359); /* jms i 67 */ emita(mp,1); /* address */ emita(sp,2); /* address */ p=get_tcell(mp,0);
emit(1536+p); /* dca */ end; /* case 1 illegal */
call error('arithmetic type incompatable',2);
/* case 2 bo-th real */
do; aflagck;
emitck(5,mp); /* fget */
emitck(4,sp); /* fdiv */
p=get_tcell(mp,1);
emit(3072+p); /* fput */ end; end; end;
/* <primary> ::= <secondary> */
/* <primary> ::= <primary*> * <secondary> */
do;if (loc(sp) &"1")=1 then call error
('exponent must evaluate to an integer',2);
if (loc(mp) "1")=0 then do;
if (tcell+2) > parmcell then call error
('code being overlayed. separate into additional statements1,2); emitck(1,sp); /* tad */
emit(3616); /* cma */
emit(516); /* tad 4 */ emit(1536+tcell+1); /* dca */ emitck(1,mp); /* tad */ emit(1536+tcell); /* dca */ advance(8);
loc(mp+1)=shl(cb,4); /* save next address */
emit(2358); /* jms i 66 */ emita(mp,1); /* address */ emit(tcell); /* address */ emit(1536+tcell); /* dca */ emit(1024+tcell+1); /* isz */ emit(269e+cb-pagf_b4se-5); /* jmp */
call get_tcell(mp,0); end;
else do;
if aflag then do; code(cb)=0; /* fext */
cb=cb+1; aflag=0; end;
advance(7);
emit(2365); /* jms 75 */ emita(sp,1); /* fxponent address */
emita(mp,2); /* number address */
p=get_tcell(mp,1);
aflagck;
 emit(3072+p); /* =put */ end; end;
/* <primary*> ::= <secondary> * */
/* <secondary> ::= <variable> */
if lookup(var(mp))=0 then
do; if var(mp+1)=byte('=') then return;
if find_proc(var(mp)) > 0 then return;
 call erkor('unknown variable: 111var(mp),2); end;
/* <secondary> <number> */
do;if dflagn=3 then return;
if fixm(mp)=0 then do; /* number less than 12 bits */ p=store_constant(0 fixv(nip),1);
loc(mp)=shl(p, 4) i 0; eno;
else do; p=store_constant(f xv(mp),(fixm(mp)&"ffffff")/3); loc(%1p)=shl(p, 4) 1 1; end; end;
/* <secondary> ( <expression> ) */ loc(mp)=loc(mp+i);
/* <secondary> ::= abs ( <expression> ) */ if (loc(sp) & "1")=0 then
do; emitck(1,sp-1); /* tad */ advance(3);
emit(3912); /* spa */
emit(3616); /* cma */
p=get tcell(md,0);
emit(1536+p); /* dca */ end; else do; aflagck;
advance(3);
(j0 emitck(5,sp-1); /* fget */ p=get tcell(mp,1); emitct(6,mp); /* fput */ emit(0000); /* fext */ emit(512+p+1); /* tad */ emit(3904); /* sma */ advance(5);
emit(2688+cb-page base+4); /* jmp */
em1t(3588); /* rae */ em1t(3648); /* cll */ eni1(35q2); /* kar */ emit(1536+p+1); /* dca */ end;
/* <secondary> ::= sor ( <expression> ) */
do;if (loc(sp) & 1,1,1)=0 then call error
(isqr requires real expressioni,2);
aflagck;
emitck(5,sp-1); /* fget */
emit(0001); /* sor */
p=get tcell(mp,1);
emitcr(6,mp); /* fput */ end;
/* <secondary> ::= sqrt ( <expression> )
do;if (loc(sp) & "1")=0 then call error
(isqrt requipes peal expres5ion',2);
aflagck;
e1itck(5,sp-1); /* fget */
emit(0002); /* sort */
p=get_tcell(mp,1);
emitck(6,mp); /* fput */ end;
/* <secondary> ::= float ( <expression> ) */
do;if (loc(sp-1) & "1")=1 then
do; call error('expression already declared real1,0); return; end;
emitck(1,sp-1); /* tad 4/
emit(2331); /* jms i 33 */
aflagck;
call get_tcell(mp,1);
emitck(6,mp); /4 fput 4/ end;
/* <logical if> ::= <if> <boolean expression> ) do;emit(512+loc(mp+1)); /* tad 4/
evit(514); /4 tad 2 */
p=get_vcell(1);
loc(mp)=p; /* save address for jmp */ advance(3);
emit(4000); /4 sza cla 4/
emit (2944+p-page_base); /* jmp */
tcell=62; end;
/* <boolean expression> ::= <boolean term>
/4 <boolean expression> ::= <boolean expression> .0r. <boolean term> do;emit(512+loc(mp)); /* tad */
emit(3857); /* mql */
emit(512+loc(sp)): /4 tad */ eyit(3905); /* mqa */
emit(1536+loc(mp)); end;
/4 <boolean term> ::= <bcolean primary> * /
/* <boolean term> ::= .not. <boolean primary> #1 do;loc(mp)=loc(mp+2);
emit(512+loc(mp)); /* tad */
emit(514); /4 tad */
emit(4000); /* sza cla 4/
emit(3713); /4 cla iac */
emit(1536+loc(mp)); /* dca */ end;
/* <boolean terr> ::= <boolean term> .and. <5colean primary> 4/ do;emit(5124-loc(mp)); /4 tad */
emit(loc(sp)); /* and */
4/
emit(1536+loc(mp)); /* dca */ end;
/* <boolean primary> ::= <logical expression> */
/* <bcolean primary> ::= ( <boolean expression> ) *1 loc(mp)=loc(mp+1);
/* <logical expression> ::= <expression> <relation> <expression> */
do;z=(loc(mp) & "1") + (loc(sp) & "1");
if loc(mp+1)>2 then do; m=loc(sp); loc(sp)=loc(mp); loc(mp)=m; end; emit(3776); /* cla cll */
do case z;
/* case 0 both integers */
do; emitck(1,sp); /* tad */ emit(3616); /* cma */ emit(516); /* tad 4 */ emitck(104p); /* tad */ eno;
/* case 1 illegal */
call erromirelation requires arithmetic type compatability',a; /* case 2 both real */
do; advance(();
co emit(2311); /* jms i 7 */ e1itck(5,mp); /* fget emitck(2,sp); /* fsub */ emit(0000); /* fext */
emit(549); /* tad 45 */ end; end;
advance(4);
do case (loc(mp+1) & "f"); /* case 0 .e0. & .me. */
if var(mp+1)=1.eq., then emit(4000); /* sza cla */
else emit(4008); /* sna cla */
/* case 1 .lt. & .gt. */
emit(4032); /* sma cla */ /* case 2 .le. & .ge. */
emit(4040); /* spa cla */ end;
m,loc(mp)-gft_tcell(mp,0); emit(2688+cb—page base+2); /* jmp */ emit(3713); /* cl t iac */ emit(1536+m); /* dca */ end;
/* <relation> ::= .lt. */ /* store a flag for logical expression */ loc(mp)=1;
/* <relation> ::= .le. set reverse subtraction */
loc(mp)=2 1 shl(178);
/* <relation> ::- .eq. */ loc(mp)=0;
/* <relation> ::= .ne. */
loc(mp)=0;
/* <relation> ::= .gt. set reverse subtraction */ loc(mp)=1 i shl(178);
/* <relation> ::= .ge. */
loc(mp)=2;
/* <labeled statement> ::= <label2> <statement> */ do;
labeled statement:
m=loc(mp); /* saved label */
if (lab(m+127)&uffff")=0 then do; lab(m+127)=lab(m+127)icb; return; end; advance(3):
emit(2944+cb—page base+1); /* jmp t */
code(c3)=lab(m+127) & "ffff"; /* backstuff do statement */ cb=cb+1;
 lab(m+127)=shl(shr(la3(m+127)716)716) i cb; end;
/* <labeled statement> ::= <label2> continue */ go to labeled_statement;
/* <label2> ::= <number> */
do;loc(mp)7m=find_label(fixv(mp));
co
co lab(m+127)=lab01+127) i shl(cb716); end;
/* <assignment statement> ::= <variable> <right part> */
do;if lookup(var(mp))=0 then do; m=enter1(var(mp)7mp);
 loc(mp)=shr(prt(m)&ufff00"74)itype; end;
if (loc(mp)&"1")+(luc(sp)&"1")=1 then
call error('assignment incompatable'72);
if (loc(mp)&"1")=1 then do; emitck(67mp); /* fput */
emit(4095); /* fext */
 aflag=0; end;
else emitck(37mp); /* dca */ end;
/* <right part> ::= = <expression> */
do;loc(mp)=loc(mp+1);
if (loc(mp) & "1")=1 then
do; advance(6); /* allows max of triple assignment */
aflagck;
emitck(57mp); /* fget */ end; else emitck(17mp); /* tad */ end;
/* <right part> ::= = <variable> <right part> */
do;if lookup(var(mp+1))=0 then do; m=enter1(var(mp+1),mp+1);
 loc(mp)=shr(prt(m)&ufff00"74)itype; end;
else loc(mp)=loc(mp+1);
loc(mp+1)=lcc(sp);
if (loc(mp)fa"1")+(loc(sp)&"1")=1 then
call error('assignment incompatable.,2);
if (loc(mp)s"1") =1 then emitck(6,mp); /* fput */
else do; emitck(3,mp); /* dca */
emitck(1,mp); /* tad */ end; end;
/* <variable> ::= <identifier> */
do;if var(sp-1)=icall, then do; /* parameterless subroutine call */ m=find proc(var(mp));
if m0 then m=set_proc(var(mp),2);
emit(2304+shr(ptable(m) & "fff000",12)); /* jms i */ return; end;
m=lookup(var(mp));
if dflag=5 then retupn; /* common statement */
if dflag=4 then do; /* data declaration */
if m=0 then call enter1(var(mp),mr);
dim(dt)=typei(prt(m) & "fffoo"); /* save type & core location */ dt=dt+1;
dim(0)=dim(0)+1; /* data variable counter */
return; end;
if dflag=3 then do;
if m,=0 then loc(mp)=(shl(shr(shl(prt(m),12),20),4)itype)ishl(parm,2); return; eno;
if m=0 then m=enter(var(mp),dflag);
else prt(m)=shr(shl(prt(m),2),2) i shl(dflag,30); if sflag=1 then return;
if type=1 then do; n=get_vcell(3); loc(mp)=shl(n,4)11; end; else cc; n=get_vcell(1); loc(mp)=shl(n,4)10; end; prt(•)=shl(n,8) i prt(m); end;
/* <variable> ::= <subscript head> <expression> ) */
do;n=(fixv(mp) c "f") +1; /* counter */ if dflag=3 then
do; /* not a declaration */
if lensth(var(sp-1)),=0 then
do; m=lookup(var(sp-1));
if m=0 then m=enter1(var(sp-1),sp-1); end; p=shr(fixv(mp),8); /* location if shr(fixv(mp) & "fo",4) <2 then
do; /* procedure call note cflag now i for subroutine
advance(3);
i=get vcell(1);
codf(t)=shr(loc(mp+1),4); emtt(640+i-page_base); /* tad */
emit(1536+next); /* dca */
if next-l<parmcell then parmcell=next-1; next=0; m=shr(shl(ptable(p), 4),28); /* parameters */
if m=0 then ptable(p)=ptable(p) i shl(n,24);
else if m-=n then
call epror('parametr count does not agree',2); n=shr(shl(ptable(p), 8),20); /* octal reference */ emit(2304+n); /* jms 1 to subprogram */
if cflag=0 then do; /* function callistope returned value ;=/ n=set(var(mp));
if n=0 then p=get tcell(1);
else p=get=tcell(3); emit(1664+p); /* dca */ loc(mp)=shl(p,4) i n+4; end; end;
else do; /* compute subscripts */
if (loc(mp+1) "1")=1 then call error
('subscripting requires integer expression',2); emitck(1,mp+1); /* tap */
if shr(shl(fixv(mp),24),28)=3 then e4it(3616);/* cma 4/ emit(1587); /* dca 63 */
m=52; dt=dt-1;
do while dt-=-1;
loc(mp+1)=01m(dt); emitck(1,mp+1); /* tad */ emit(1536+m); /* dca */ dt=dt-1; m=m+1; end;
advance(4);
m=shr(fixv(mp),8); /* prt address */ n=(shr(shl(prt(m),12),20)); /* array base */
if (prt(m) f. "20000000") > 0 then
do; emit(512+n); /* tad */
emit(1664+cb-page_base+2); /* dca */
emit(2354); /* jas i 62 */
cb=cb+1; end;
else do; emit(2354);
 eait(n); end; p=get_tcell(mp,0);
eait(1536+p); /* dca */ p=shr(shl(fixv(mp),24),28); loc(mp)=loc(mp)14+p-2; end;
return; end;
if dflag=4 then do; /* data declaration */ if fixv(mp+1)=0 then call error
('data variable subscript must be integer number',2); dim(dt)=fixv(mp+i);
dt=dt+1; return; end;
if (fixv(mp+1))=0 then call error('subscripts must be integfr nu1bers',2); dim(dt)=fixv(sp-1); /* insert last dimension into dim array */ p=lookup(var(mp));
if p=0 then do; if dflag-2 then p=enter(var(mp),set(var(mp))+2);
else p=ente(va(mp),dflag+2); typ:=shr(prt(p),30); end;
else do; prt(p)=prt(p)+"80000000"; /* desig parameter an aaray -/
m=1;
do 1=0 to dt; m=m * dim(i); end; size(p-127)=m+dt+2;
return; end;
if type=3 then do; dim(dt+1)=3; loc(mp)=1; end; else do; dim(dt+1)=1; loc(mp)=0; end; do i=1 to n-1; /* compute d sub i */
dim(dt+i+1)=dim(dt-i+1) * dim(dt+i); end; m=dr+n+1;
dim(m)=dim(dt+1);
do 1=0 to dt; /* compute total cells required for array */
dim(m)=dim(m) * dim(i); end;
z=get_acell(dim(m)+1+n); /* z is base of array block */ size(p-127)=0im(m)+n+1;
loc(mp)=loc(mp) i shl(z,4);
prt(p)=prt(p) i shl(z18);
code(z)=4095-n+1; /* nec octal numper=number of subscripts */
do 1=1 to n-1;
code(z+i)=dim(dt+i+1); end; h=0; do i=i to n; /* sum d sub i */
h=h+dim(dt+i); end;
code(z+n)=h; end;
/* <su3script head> ::= <identifier> (
do;m=lookup(var(mp));
if dflag=4 then do; /* data declaration */
if m=0 then call error(,data declarations require array be known',2); dim(dt)=typei(prt(m) & "fffoo"); /* save type & core location */ dt=dt+1;
dim(0)=dim(0)+1; /* data variaele counter */
return; end;
if dflag=3 then do; /* not a declaration */
if m=0 then
do; /* procedure call */
if next > 0 then call error
(,function calls within subprogram calls not allowed,,2); m=find_proc(var(mp));
if m=0 then m=set proc(var(mp),cflag+1);
next=ptable(m) & tifff";
if cflag=0 then fixv(mp)=shl(m,8); /* function */
else fixv(mp)="10"ishl(m,8); /* subroutine */ end; else 00; /* must compute supscript of array */
if m=0 then call errorpunknown array,,2);
fixv(mp)=shl(m, 8) i shl(type,4);
nt=0; end;
return; end; /* must dimension an array */
dt=0; fixv(1p)=0; end;
1* <subscript head> ::= <subscript head> <expression> , */
do;if dflag=3 then do; /* not a declaration */ if shr(fixv(mp) c"fo",4)<2 then
do; /* a procedure */
if length(var(sp-1)),=0 then do; m=lonkup(var(sp-1));
if m=0 then m=enter1(var(sp-1),sp-1); end; fixv(mp)=fixv(mp)+1; /* parameter count */
advance(3);
i=get_vcell(1);
code(i)=shr(loc(mp+1),4);
emit(540+i-page_base); /* tad */ eait(1536+next): 1* dca */
next=next-1; end;
else do; /* must compute subscript */
fixw1p)=fixv(mp) + 1; /* dimension counter */
if (loc(mp+1) c "1")=1 then call error
('subscripting requres integer expression',2); dim(dt)=loc(vp+1);
dt=dt+1; end;
return; end;
if dflag=4 then do; /* data declaration */
if length(var(mp+1))-=0 then call error
('data variable subscript must be integer number',2);
dim(dt)=fixv(mp+1);
dt=dt+1;
return; end;
if (fixv(mp+1))=0 then call error('subscripts must be integer numbers',2); di1(dt)=fixv(sp-1);
dt=dt+1;
fixv(mp)=fixv(mp)+ 1; end;
 /* <do statement> ::= <do head> */
do;if (loc(mp) c 'i")=o then step-shl(4,4); /* fixed point one */
else do; n=store constant(1,4194304,3); /* floating point one */
 step=she(n,4) 1 1; end;
do head:
 z=(loc(mp) c "1") + (step c 1") + (do_until c "1");
if z=3 then z=1;
else if z,=0 then call error('do expressions assignment incompatable1,2); do case z;
/* case 0 integer */
do; emitck(1,mp); /* tad */
m=loc(mp); /* save mp for passing parameters */
loc(mp)=step;
emitck(1,mp); i* tad */ loc(ip)=m;
emitck(3,mp); /* dca */
m=loc(mp);
loc(mp)=do until;
emitck(1,mp); /* tad */ emit(3616); /* cma */ loc(mp)=m;
emitck(1,mp); /* tad */ end; /* case 1 real */
do; advance(11);
aflagck;
emitck(5,mp); /* eget */ m=loc(mp);
loc(mp)=step;
emitck(1,mp); /* fadd */ loc(mp)=m;
emitck(6,mp); /* fput */ m=loc(mp);
loc(mp)=do until;
emitck(2,m15); /* fsub */ emit(568); /* fadd 70 */ em!t(0000); i fext */ loc(mp)=m;
emit(549); /* tad 45 */ eno; end;
advance(3);
emit(4032); /* sma cla */ p=setlab(save_label_address,1); emit(2944+p); /* jmp i */
code(save_first)=cb; end;
 /* <do statement> ::= <do head> , <expression> * / do;step=loc(sp); go to do_head; end;
/* xdo head> ::= <do variable> , <expression> do_until=loc(sp);
/* <do variable> ::= <do label> <variable> = <expression> 00,n=lookup(v4r(mp+1));
if n=0 then call enter1(var(mp+1),mp+1);
if (loc(mp+1) a "1") + (loc(sp) a "1")=1 then
call error(id0 variable assignment incompatable,,2); do case (loc(sp) & "1");
/* case 0 integer assignment */ do; emitck(1,sp); /* tad */ emitck(3,mp+1); /* dca */ end;
/* case 1 real assignment */ do; advance(6);
emit(2311); /* js i 7 */ emitck(5,sp); /* fget */ emitck(6,mp+1); /* fput */
emit(0000); /* fext */ end; end;
advance(2);
emit(2944+cb-page_base+1); /* jmp to first statement */
save_first=cb; cb=cb+1; /* stuff with address of first statement */ if lab(loc(mp)+127)-a=0 then do; call error(' ,2);
output='nested do loops must not end on same labeled statement', output=' --op- duplicate label'; output=' 1; end;
lab(loc(mp)+127)=cb; /* return for increment */
loc(mp)=loc(mp+1); /* save the variable */ end;
/* <do label> ::= do <number> */ loc(mp),save_label_address=find_label(fixv(sp));
/* <go statement> ::= <goto> <number> */ do;m=find_label(fixv(sp));
p=setlab(m,0);
emit(2944+p); /* jmp i */ end;
/* <go statement> ::= <go transfer> <end go> <variable> */
do;if (loc(sp) & "1")=1 then call error('variable must be integer type',2); emitck(1,sp); /* tad */
advance(dt+3);
emit(640+cb-page_base+3), /* tad */
emit(1664+cb-page_base+1); /* dca */
cb=cb+1;
emit(2944+cb-page_base); /* jmp - labels follow */
do i=0 to dt-1;
code(cb)=shl(dim(i),16); cb=cb+1; end; end;
/* <goto> ::= go to */
/* <goto> ::= goto */
/* <go transfer> ::= <goto> <paren> <number> */
do;dt=0; dim(dt)=find_label(fixv(sp)); dt=dt+1; end;
/* <go transfer> ::= <go transfer> <ccmma> <number> */
do;dim(dt)=find_label(fixv(sp)); dt=dt+1; end;
/* <paren> ::= ( */•
/* <comma> ::= 7 */
/* <end go> )
/* <declaration list> ::= <declaration> ; */ dflag=3;
/* <declaration list> ::= <declaration list> <declaration> ; */ dflag=3;
/* <declaration> ::= <declaration type> <variable> */ if dflag=5 then call common_check(sp);
/* <declaration> ::= <declaration type> <variable list> <variable> if dflag=5 then call common_check(sp);
/* <declaration> ::= <data declaration> <number> / */ do;call insert_data;
if (dim(0) ,iffii)--.=shr(dim(0),8) then call error
('number of variables and data do not match',2); end;
./* <declaration type> ::= dimension *1 dflag=2;
/* <declaration type> ::= integer */ dflag=0;
/* <declaration type> ::= real dflag=1;
/* <declaration type> ::= common */ dflag=5;
/* <variable list> ::= <variable> , */ if dflag=5 then call common_check(sp-1);
/* <variable list> ::= <variable list> <variable> */ if dflag=5 then call common_check(sp-1);
/* <data declaration> ::= <data head> / */ dt=1;
/* <data declaration> ::= <data declaration> <number> , call insert_data;
/* <data head> ::= <data> <variable> */
/* <data head> ::= <data> <variable list> <variable> * /
*/
/* <data> ::= data */
do;dt=1;
dim(0)=0; /* variable counter */ oflag=4; end;
/* <procedure block> ::= <procedure heaaing> */
do;
procedure_heading:
p=shr(fixv(vp)78); /* ptable entry */
p=shr(ptablf(p) & "fff000",12); /* octal reference on page zero ;'/ code(p)=entry & "fffn; end;
/* <procedure block> ::= <procedure heading> <declaration list> */ go to procedure_heading;
/* <procedure heading> ::= <paramless procedure> */
/* <procedure heading> ::= <procedure & parameters> */
/* <paramless procedure> ::= subroutine <identifier> ; */ do;entry=store_code(0000);
p=find_proc(var(sp-1)); sflag=2;
if p=0 then p=set_proc(var(sp-1),2);
else do; if shr(shl(ptable(m),4),28) -=0 then
call epror('#parameters does not agree with prior use',2); if shr(ptaple(p),29) 2 then
call error('procedure used as both function c subroutinf',2); end;
ptable(p)=ptable(p)+ "10000000"; /* indicate procedure known */ fixv(mp)=shl(p,8); end;
/* <procedure & parameters> ::= <procedure head> <identifier> ) */ d0;m=enter(var(sp-2),set(var(sp-2)));
prt(m)=prt(m) shl(1,29); prt(m)=prt(m)ishl(next,8);
if next-1 < parmcell then parmcell=next-1; next=0;
p=shr(fixv(mp),8); /* location of procedure name */
fixv(mr)=fixv(mp)+1;
n=fixv(mp) & "f";
ptable(p)=ptable(p)f "10000000"; /* indicate procedure known */
if (ptable(p) & "f000000")=0 then ptable(p)=ptable(r)ishl(n,24);
else if shp(shl(ptable(p),4),28) -= (fixv(mp) & "f") then
call error('parameter count does not agree with prior use',2); end;
/* <procedure head> ::= <procedure type> */
/* <procedure head> ::= <procedure head> <identifier> 1 */ do;m=enter(var(sp-1),set(var(sp-1)))%
prt(m)=prt(m) i shl(1,29);
fixv(mp)=fixv(mp)+1;
prt(m)=prt(m)ishl(next,8); next=next-1; end;
/* <procedure type> ::= function <identifier> ( */ do;entry=store_code(0000); sflag=1;
p=find_proc(var(sp-1));
if p=0 then p=set_proc( var(sp-1) 11);
else if shr(ptable(p)129) -=1 then
call errop(ippacedure used as both function & subroutine',2); next=ptable(p) e "fff";
fixv(mp)=shl(p,8) i shl(1,4);
m=enter1(var(sp-1),sp-1);
/* save octal location of variable for return */
entry=entry 1 shl(prt(m) & ufff00",8); end;
/* <procedure type> ::= subroutine <identifier> ( */
do;entry=store code(0000); sflag=2;
p=find_proctvar(sp-1));
if p=0 then p=set_pv3c( var(sp-1) 12);
else if shr(ptable{p),29) --2 then
call error('procedure used as both function & subroutine',2); next=ptable(p) e
fixv(mp)=shl(p,8) i shl(1,4); end;
/* <subroutine call> ::= <call> <variable> */
cflag=0;
/* <call> ::= call */ cflag=1;
/* <read statement> ::= <read head> <variable> ) */
do;
read:
m=lcokup(var(sp-1));
if m=0 then m=enter1(var(sp-1),sp-1);
emit(2310); /* pis i 6 */
aflagck;
if (loc(sp-1) &"1")=1 then do; emitck(6,sp-1); /* fput */ emit(4096); /* fext */ aflag=0; /* convert real to integer */ return; end;
emit(0000); /* fext */ aflag=0;
emit(2335); /* jms i 37 */
emitck(3,sp-1); /* dca */ end;
/* <read head> ::= read ( * /
/* <read head> ::= <read head> <variable> , go to read;
/* <write statement> ::= <write head> <expression> ) */
do;
write expression:
if (e0c(sp-1) e "1")=1 then do; aflagck;
emitck(5,sp-1); /* fget */
emit(0000); /* fext */ aflag=0; end; else do; emitck(1,sp-1); /* tad */
 emit(2334); /* jms i 36 */ end;
emit(2309); /* jms i 5 */ end;
/* <write statement> ::= <write head> <string> ) */ call emit_string(var(sp-1));
/* <write statement> ::= <write head> <tab expression> ) */
/* <write head> ::= write ( */
do;emitcar(141); /* return */
emitcar(138); /* line feed */ end;
/* <write head> ::= writeon ( */
/* <write head> ::= <write head> <expression> , go to write_expression;
/* <write head> ::= <write head> <string> */
call emit_string(var(sp-1));
/* <write head> ::= <write head> <tab expression> 1 */
/* <tab expression> ::= tab <expression> do;advance(2);
emit(2332); /* jms i 34 */
emit(4096-pixv(sp)); end; end; end synthesize;
/* syntactic parsing functions
right conflict:
procedure (left) bit(1);
declare left fixed;
/* this procedure is true if token is , a legal right context of left*/ return ("co" & shl(byte(c1(left), shr(token,2)), shl(token,1)
& "06")) = 0; end right_conflict;
recover:
procedure;
/* if this is the second successive call to recover, discard one symbol */ if failsoft then call scan;
failsoft = false;
do while , stopit(token);
call scan; /* to find something solid in the text */ end; do while right conflict (parse stack(sp));
if sp > 2 tren sp = sp - 1,- /* and in the stack */
else call scan; /* but don't go too far */ end;
output = 'resume:' h substr(pointer, text_limit-cp+margin_chop+7); end recover;
stacking:
procedure bit(1); /* stacking decision function =..,/
callcount(1) = callcount(1) + 1;
do forever; /* until return */
do case shr(byte(c1(parse_st4ck(sp)),shr(token,2)),shl(3-tokem,1)&6) ;
/* case 0 */
do; /* illegal symbol pair call error('illegal symbol v(token), 1);
call stack_dump;
call recover; end;
pair: ' ii v(parse_stack(sp)) h x1 ii
/* case 1 */
return true; /* stack token */
/* case 2 */
return false; /* don't stack it yet */
/* case 3 */
do; /* must check triples */
j = shl(parse_stack(sp-1), 16) + shl(parse_stack(sp), 8) + toke,
i = -1, k = nc1triples + 1; /* binary search of triples */
do while i + 1 < k; l = shr(i+k, 1);
if c1triples(l) > j then k = l;
else if c1triples(l) < j then i = l;
else return true; /* it is a valid triple 4/ end; return false; end;
end; /4 of do case 4/
end; /4 of do forever 44/
end stacking;
pr ok:
procedure(prd) bit(1);
14 decision procedure for context check of equal or imbedded right parts-x/ declare (h, i, j, pro) fixed;
do case context_case(prd);
/* case 0 -- no check required 4/
return true;
/* case 1 -- right context check 4/ return -i right_conflict (hdtb(prd));
/4 case 2 -- left context check 4/
do;h = hdtb(prd) - nt;
i = parse stack(sp - prlength(prd));
do j = left_inoex(h-1) to left_index(h) - 1;
if left_context(j) = i then return true; end;
return false; end;
/4 case 3 -- check triples -1/4/
do;h = hdtb(pro) - nt;
 i = shl(parse_stack(sp prlength(prd)), 8) + token;
do j = triple_index(h-1) to triple_inoex(h) - 1;
if context_triple(j) = i then return true; end;
return false; end; end; /4 of do case 4/ end pr ok;
/4 analysis algorithm
reduce:
procedure;
declare (i, j, prd) fixed;
/4 pack stack top into one word */
do i = sp - 4 to sp - 1;
j = shl(j, 8) + parse stack(i); end;
do prd = pr index(parse_ttack(sp)-1) to pr index(parse_stack(sp)) - 1; if (prm4.7k(prlength(prd)) & j) = prtb(p7d) then
if pr ok(prd) then
do; 7* an allowed reduction 4/
mp = sp - prlength(prd) + 1; mpp1 = mp + 1;
if control(byte('p')) then do;
5=1 ii v(hdtb(prd)) ii 1 ::= 1;
 do i=mp to sp; s=s ii v(parse_stack(i)) ii '; end;
output=s; end;
call synthesize(prdtb(prd)); sp = mp;
parse_stack(sp) = hdtb(prd); return; end; end;
/* look up has failed, error condition */ call error('no production is applicable',1); call stack dump;
failsoft =false;
call recover;
end reduce;
compilation_loop:
procedure;
compiling = true;
do while compiling; /* once around for each production (reduction) ,v
do while stacking;
sp = sp 1;
if sp = stacksize then
do;call error ('stack overflow *** checking aborted ***i, 2); return; /* thus aborting checking */ end;
parse_stack(sp) = token;
var(sp) = bcd;
if nflag=1 then do; fixv(sp)=hold1; /* real number */ fixm(sp)=hold2ishl(1,30); end;
else do; fixv(sp)=number_value; /* integer number */ fixm(sp)=0; eno; nflag=0;
call scan; end;
call reduce;
end; /* of do while compiling */
end compilation_loop;
print_summary:
procedure;
declare i fixed;
call print_date_and_time ('end of checking ', date, time);
output = 10;
output = card_count ii ' cards were checked.';
if error_count = 0 then output = 'no errors were detected.';
else if error count > i then
output = e7ror count ii ' errors (' ii severe errors
e severet were detected.';
else if severe_eprors = 1 then output = 'one severe error was detected.'; else output = 'one error was detected.';
if previous_erkor > 0 then
output = 'the last detected error was on line ' ii previous_error

/* Page 113 */

            || period;
        if control(byte('d')) then call dumpit;
        double_space;
        clock(3) = time;
        do i = 1 to 3;   /* watch out for midnight */
            if clock(i) < clock(i-1) then clock(i) = clock(i) + 8640000; end;
        call print_time ('total time in checker   ', clock(3) - clock(0));
        call print_time ('set up time             ', clock(1) - clock(0));
        call print_time ('actual checking time    ', clock(2) - clock(1));
        call print_time ('clean-up time at end    ', clock(3) - clock(2));
        if clock(2) > clock(1) then   /* watch out for clock being off */
        output = 'checking rate: ' || 6000*card_count/(clock(2)-clock(1))
            || ' cards per minute.';
    end print_summary;

main_procedure:
    procedure;
        clock(0) = time;  /* keep track of time in execution */
        call initialization;
        clock(1) = time;
        call compilation_loop;
        clock(2) = time;
        /* clock(3) gets set in print_summary */
        call print_summary;
    end main_procedure;

call main_procedure;
return severe_errors;

eof eof eof
