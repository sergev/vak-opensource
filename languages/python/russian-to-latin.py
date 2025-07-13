#!/usr/bin/env python3
#
# Convert Russian text to Latin encoding.
# Copyright (C) 2025 Serge Vakulenko
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
import argparse
import sys
import os


def encode_russian_text(text):
    """
    Encodes Russian text according to the specified rules:
    1. Basic letter mapping
    2. Soft sign (ь) makes previous consonant acute
    3. Special vowels (е/ё/ю/я) with consonant context rules
    4. Acuted consonants lose acute when followed by i/j/ë/ö/ü/ä
    """

    # Rule 1: Basic letter mapping (lowercase)
    basic_mapping = {
        'а': 'a', 'б': 'b', 'в': 'v', 'г': 'g', 'д': 'd',
        'ж': 'ž', 'з': 'z', 'и': 'i', 'й': 'j', 'к': 'k',
        'л': 'l', 'м': 'm', 'н': 'n', 'о': 'o', 'п': 'p',
        'р': 'r', 'с': 's', 'т': 't', 'у': 'u', 'ф': 'f',
        'х': 'h', 'ц': 'c', 'ч': 'č', 'ш': 'š', 'щ': 'ş',
        'ъ': "'", 'ы': 'y', 'э': 'e'
    }

    # Rule 1: Basic letter mapping (uppercase)
    basic_mapping_upper = {
        'А': 'A', 'Б': 'B', 'В': 'V', 'Г': 'G', 'Д': 'D',
        'Ж': 'Ž', 'З': 'Z', 'И': 'I', 'Й': 'J', 'К': 'K',
        'Л': 'L', 'М': 'M', 'Н': 'N', 'О': 'O', 'П': 'P',
        'Р': 'R', 'С': 'S', 'Т': 'T', 'У': 'U', 'Ф': 'F',
        'Х': 'H', 'Ц': 'C', 'Ч': 'Č', 'Ш': 'Š', 'Щ': 'Ş',
        'Ъ': "'", 'Ы': 'Y', 'Э': 'E'
    }

    # Rule 2: Consonants that can be made acute with soft sign (lowercase)
    acute_consonants = {
        'б': 'b́', 'в': 'v́', 'г': 'ǵ', 'д': 'd́', 'з': 'ź',
        'к': 'ḱ', 'л': 'ĺ', 'м': 'ḿ', 'н': 'ń', 'п': 'ṕ',
        'р': 'ŕ', 'с': 'ś', 'т': 't́', 'ф': 'f́', 'х': 'h́',
        'ц': 'ć'
    }

    # Rule 2: Consonants that can be made acute with soft sign (uppercase)
    acute_consonants_upper = {
        'Б': 'B́', 'В': 'V́', 'Г': 'Ǵ', 'Д': 'D́', 'З': 'Ź',
        'К': 'Ḱ', 'Л': 'Ĺ', 'М': 'Ḿ', 'Н': 'Ń', 'П': 'Ṕ',
        'Р': 'Ŕ', 'С': 'Ś', 'Т': 'T́', 'Ф': 'F́', 'Х': 'H́',
        'Ц': 'Ć'
    }

    # Rule 3: Special vowels (lowercase and uppercase)
    special_vowels = {'е', 'ё', 'ю', 'я', 'Е', 'Ё', 'Ю', 'Я'}

    # Vowel mappings after consonants (consonant gets acute)
    vowel_after_consonant = {
        'е': 'e', 'ё': 'o', 'ю': 'u', 'я': 'a',
        'Е': 'E', 'Ё': 'O', 'Ю': 'U', 'Я': 'A'
    }

    # Vowel mappings in other cases
    vowel_standalone = {
        'е': 'ë', 'ё': 'ö', 'ю': 'ü', 'я': 'ä',
        'Е': 'Ë', 'Ё': 'Ö', 'Ю': 'Ü', 'Я': 'Ä'
    }

    # Set of consonants (Russian, lowercase and uppercase)
    consonants = {
        'б', 'в', 'г', 'д', 'ж', 'з', 'к', 'л', 'м', 'н',
        'п', 'р', 'с', 'т', 'ф', 'х', 'ц', 'ч', 'ш', 'щ',
        'Б', 'В', 'Г', 'Д', 'Ж', 'З', 'К', 'Л', 'М', 'Н',
        'П', 'Р', 'С', 'Т', 'Ф', 'Х', 'Ц', 'Ч', 'Ш', 'Щ'
    }

    result = []
    i = 0

    while i < len(text):
        char = text[i]

        # Handle soft sign (ь/Ь)
        if char.lower() == 'ь':
            if result:
                # Check if previous character can be made acute
                prev_char_lower = text[i-1].lower() if i > 0 else ''
                if prev_char_lower in acute_consonants:
                    # Replace with acute version, preserving case
                    if text[i-1].isupper():
                        result[-1] = acute_consonants_upper[text[i-1]]
                    else:
                        result[-1] = acute_consonants[prev_char_lower]
            i += 1
            continue

        # Handle special vowels (е/ё/ю/я)
        if char in special_vowels:
            # Check if previous character is a consonant
            if i > 0:
                prev_char = text[i-1]
                if prev_char in consonants:
                    # Make previous consonant acute and use consonant vowel mapping
                    if result and prev_char.lower() in acute_consonants:
                        # Replace the last character with acute version, preserving case
                        if prev_char.isupper():
                            result[-1] = acute_consonants_upper[prev_char]
                        else:
                            result[-1] = acute_consonants[prev_char.lower()]
                    result.append(vowel_after_consonant[char])
                else:
                    # Use standalone vowel mapping
                    result.append(vowel_standalone[char])
            else:
                # At beginning of text, use standalone mapping
                result.append(vowel_standalone[char])

        # Handle basic mapping
        elif char in basic_mapping:
            result.append(basic_mapping[char])
        elif char in basic_mapping_upper:
            result.append(basic_mapping_upper[char])

        # Handle characters not in mapping (preserve as is)
        else:
            result.append(char)

        i += 1

    # Rule 4: Remove acute from consonants when followed by i/j/ë/ö/ü/ä
    final_result = []
    losing_acute_chars = {'i', 'j', 'ë', 'ö', 'ü', 'ä', 'I', 'J', 'Ë', 'Ö', 'Ü', 'Ä'}

    # Create reverse mapping from acute to non-acute consonants
    acute_to_normal = {}
    for normal, acute in acute_consonants.items():
        acute_to_normal[acute] = basic_mapping[normal]
    for normal, acute in acute_consonants_upper.items():
        acute_to_normal[acute] = basic_mapping_upper[normal]

    for i, char in enumerate(result):
        # Check if current character is acute consonant and next character causes acute loss
        if char in acute_to_normal and i + 1 < len(result) and result[i + 1] in losing_acute_chars:
            final_result.append(acute_to_normal[char])
        else:
            final_result.append(char)

    return ''.join(final_result)


def process_text(text):
    """Process text and output encoded result"""
    lines = text.splitlines()
    for line in lines:
        if line.strip():  # Only process non-empty lines
            encoded = encode_russian_text(line)
            print(encoded)
        else:
            print()  # Preserve empty lines


def process_file(filename):
    """Process a single file"""
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            content = f.read()
            process_text(content)
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.", file=sys.stderr)
        return False
    except IOError as e:
        print(f"Error reading file '{filename}': {e}", file=sys.stderr)
        return False
    return True


def show_help():
    """Show help message when stdin is tty"""
    help_text = """Russian to Latin Encoder

Usage: russian-to-latin [-h] [filename...]

Encodes Russian text to Latin script using specific transliteration rules.

Arguments:
  filename...    Input files to process. If no files specified, reads from stdin.

Options:
  -h, --help     Show this help message and exit.

Encoding Rules:
  1. Basic letter mapping (а→a, б→b, в→v, etc.)
  2. Soft sign (ь) makes previous consonant acute (бь→b́)
  3. Special vowels (е/ё/ю/я) with consonant context rules
  4. Acuted consonants lose acute when followed by i/j/ë/ö/ü/ä

Examples:
  russian-to-latin file.txt           # Process file
  echo "привет" | russian-to-latin    # Process stdin
  russian-to-latin file1.txt file2.txt # Process multiple files
"""
    print(help_text)


def main():
    """Main function with CLI argument parsing"""
    parser = argparse.ArgumentParser(
        prog='russian-to-latin',
        description='Encode Russian text to Latin script',
        add_help=False  # We'll handle -h manually
    )

    parser.add_argument('-h', '--help', action='store_true',
                       help='Show help message and exit')
    parser.add_argument('filenames', nargs='*',
                       help='Input files to process')

    # Parse arguments
    try:
        args = parser.parse_args()
    except SystemExit:
        return

    # Handle help
    if args.help:
        show_help()
        return

    # Check if stdin is a tty and no files provided
    if not args.filenames and sys.stdin.isatty():
        show_help()
        return

    # Process files or stdin
    if args.filenames:
        # Process each file
        success = True
        for filename in args.filenames:
            if not process_file(filename):
                success = False
        if not success:
            sys.exit(1)
    else:
        # Process stdin
        try:
            content = sys.stdin.read()
            process_text(content)
        except KeyboardInterrupt:
            sys.exit(1)


def interactive_mode():
    """Interactive mode for testing (legacy)"""
    print("Russian Text Encoder - Interactive Mode")
    print("Enter Russian text to encode (or 'quit' to exit):")

    while True:
        try:
            user_input = input("\nRussian text: ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nGoodbye!")
            break

        if user_input.lower() == 'quit':
            print("Goodbye!")
            break

        if not user_input:
            print("Please enter some text.")
            continue

        encoded = encode_russian_text(user_input)
        print(f"Encoded: {encoded}")


# Test examples
if __name__ == "__main__":
    # Check if running in test mode
    if len(sys.argv) > 1 and sys.argv[1] == '--test':
        # Test cases to demonstrate the rules
        test_cases = [
            "привет",      # Basic mapping
            "Привет",      # Basic mapping with uppercase
            "день",        # Soft sign rule
            "ДЕНЬ",        # Soft sign rule uppercase
            "мёд",         # Special vowel after consonant
            "Мёд",         # Special vowel after consonant mixed case
            "ель",         # Special vowel standalone
            "ЕЛЬ",         # Special vowel standalone uppercase
            "семья",       # Mixed rules
            "СЕМЬЯ",       # Mixed rules uppercase
            "съесть",      # Hard sign
            "ОБЪЕКТ",      # Mixed example uppercase
            "тётя",        # Rule 4: acute loss (тё → to, not t́ö)
            "дядя",        # Rule 4: acute loss (дя → da, not d́ä)
            "нить",        # Rule 4: acute loss (ни → ni, not ńi)
            "кий",         # Rule 4: acute loss (кий → kij, not ḱij)
        ]

        print("Test cases:")
        for test in test_cases:
            encoded = encode_russian_text(test)
            print(f"{test} -> {encoded}")

        print("\n" + "="*50 + "\n")

        # Run interactive mode
        interactive_mode()
    else:
        # Run CLI mode
        main()
