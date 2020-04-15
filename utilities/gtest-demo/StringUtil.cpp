#include "StringUtil.h"
#include "CharUtil.h"

namespace stringutil {
   std::string head(const std::string& word) {
      return word.substr(0, 1);
   }

   std::string tail(const std::string& word) {
      if (word.length() == 0) return "";
      return word.substr(1);
   }

   std::string zeroPad(const std::string& text, unsigned int toLength) {
      auto zerosNeeded = toLength - text.length();
      return text + std::string(zerosNeeded, '0');
   }

   std::string upperFront(const std::string& string) {
      return std::string(1, charutil::upper(string.front()));
   }
}
