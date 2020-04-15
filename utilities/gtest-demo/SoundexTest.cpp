#include "gtest/gtest.h"
#include "Soundex.h"

using namespace testing;

class SoundexEncoding: public Test {
public:
   Soundex soundex;
};

TEST_F(SoundexEncoding, RetainsSoleLetterOfOneLetterWord) {
   ASSERT_EQ(soundex.encode("A"), "A000");
}

TEST_F(SoundexEncoding, PadsWithZerosToEnsureThreeDigits) {
   ASSERT_EQ(soundex.encode("I"), "I000");
}

TEST_F(SoundexEncoding, ReplacesConsonantsWithAppropriateDigits) {
   ASSERT_EQ(soundex.encode("Ax"), "A200");
}

TEST_F(SoundexEncoding, IgnoresNonAlphabetics) {
   ASSERT_EQ(soundex.encode("A#"), "A000");
}

TEST_F(SoundexEncoding, ReplacesMultipleConsonantsWithDigits) {
   ASSERT_EQ(soundex.encode("Acdl"), "A234");
}

TEST_F(SoundexEncoding, LimitsLengthToFourCharacters) {
   ASSERT_EQ(soundex.encode("Dcdlb").length(), 4u);
}

TEST_F(SoundexEncoding, IgnoresVowelLikeLetters) {
   ASSERT_EQ(soundex.encode("BaAeEiIoOuUhHyYcdl"), "B234");
}

TEST_F(SoundexEncoding, CombinesDuplicateEncodings) {
   ASSERT_EQ(soundex.encodedDigit('b'), soundex.encodedDigit('f'));
   ASSERT_EQ(soundex.encodedDigit('c'), soundex.encodedDigit('g'));
   ASSERT_EQ(soundex.encodedDigit('d'), soundex.encodedDigit('t'));

   ASSERT_EQ(soundex.encode("Abfcgdt"), "A123");
}

TEST_F(SoundexEncoding, UppercasesFirstLetter) {
   ASSERT_EQ(soundex.encode("abcd")[0], 'A');
}

TEST_F(SoundexEncoding, IgnoresCaseWhenEncodingConsonants) {
   ASSERT_EQ(soundex.encode("BCDL"), soundex.encode("Bcdl"));
}

TEST_F(SoundexEncoding, CombinesDuplicateCodesWhen2ndLetterDuplicates1st) {
   ASSERT_EQ(soundex.encode("Bbcd"), "B230");
}

TEST_F(SoundexEncoding, DoesNotCombineDuplicateEncodingsSeparatedByVowels) {
   ASSERT_EQ(soundex.encode("Jbob"), "J110");
}
