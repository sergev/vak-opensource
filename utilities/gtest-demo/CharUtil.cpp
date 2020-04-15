#include "CharUtil.h"
#include <string>
#include <cctype>

using namespace std;

namespace charutil {
   bool isVowel(char letter) {
      return string("aeiouy").find(tolower(letter)) != string::npos;
   }

   char upper(char c) {
      return std::toupper(static_cast<unsigned char>(c));
   }

   char lower(char c) {
      return std::tolower(static_cast<unsigned char>(c));
   }
}
