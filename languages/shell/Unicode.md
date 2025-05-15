Hi there!
You're right that the first 128 characters in Unicode came from ASCII. Unicode's story extends far beyond those initial characters, though, with numerous blocks that emerged from various national and international standards. Let me walk you through some of the major Unicode blocks and their origins.

## ASCII (U+0000 to U+007F)
The first 128 characters are indeed from ASCII (American Standard Code for Information Interchange), developed in the 1960s primarily for English text.

## Latin-1 Supplement (U+0080 to U+00FF)
This block comes from ISO 8859-1 (also called Latin-1), which extended ASCII for Western European languages with characters like é, ü, and ñ. This was widely used in early Western European computing.

## Latin Extended-A/B (U+0100 to U+024F)
These blocks contain characters for Central and Eastern European languages, largely derived from various ISO 8859 standards (ISO 8859-2, ISO 8859-3, etc.) that were developed in the 1980s.

## Greek and Coptic (U+0370 to U+03FF)
Based on the ISO 8859-7 standard with additional historic letters. The Modern Greek letters were standardized first, with ancient Greek and Coptic characters added later.

## Cyrillic (U+0400 to U+04FF)
Initially based on ISO 8859-5 for Slavic languages using the Cyrillic script, later expanded to support more languages of the former Soviet Union and Mongolia.

## Hebrew (U+0590 to U+05FF)
Based on ISO 8859-8, with extensions for complete Biblical Hebrew support.

## Arabic (U+0600 to U+06FF)
Originally derived from ISO 8859-6, but significantly expanded to support various Arabic script languages across the Middle East, North Africa, and parts of Asia.

## Devanagari (U+0900 to U+097F)
Based on ISCII (Indian Script Code for Information Interchange), which was developed by the Indian government in the 1980s as a standard for Indic scripts.

## CJK Unified Ideographs (U+4E00 to U+9FFF)
One of the largest blocks, containing characters used in Chinese, Japanese, and Korean writing systems. This massive unification project combined characters from various national standards:
- Chinese GB standards (Simplified Chinese)
- Taiwan's Big5 encoding (Traditional Chinese)
- Japan's JIS X 0208 standard
- Korea's KS X 1001 standard

The unification process (called "Han unification") was politically complex, as different countries sometimes had different views on character identity.

## Hiragana and Katakana (U+3040 to U+30FF)
Based on Japanese national standards JIS X 0208 and JIS X 0201, these blocks contain the Japanese syllabic scripts.

## Hangul Syllables (U+AC00 to U+D7AF)
Based on Korean national standard KS X 1001, but using a different encoding approach that dynamically composes Hangul syllables.

## Notable Later Additions

### Mathematical Symbols (U+2200 to U+22FF)
Drawn from various mathematical notation standards and practices.

### Emoji (Multiple blocks, including U+1F600 to U+1F64F)
One of the more recent and culturally significant additions, originally from Japanese mobile phone carriers' character sets (especially DoCoMo's emoji), standardized starting in 2010.

### Lesser-Used Scripts
Unicode's mission to encode all writing systems led to the inclusion of ancient scripts (Cuneiform, Hieroglyphs), minority scripts (Cherokee, Yi), and constructed scripts (Klingon). Many of these came from academic sources rather than previous encoding standards.

The Unicode Consortium continues to add new blocks with each version, focusing on historical scripts, specialized symbols, and expanding existing blocks to cover more linguistic variations. The journey from ASCII's 128 characters to Unicode's current 149,000+ characters reflects computing's evolution from English-centric to truly global.
