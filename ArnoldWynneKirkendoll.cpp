/*
Students' Names:      Benjamin Arnold, Thomas Wynne, Michael Kirkendoll
Course:               CSC242 Intro to Programming Concepts
Instructor:           Professor Rich Yonts
Assignment:           Assignment 5 — Streams Projects P8.2 and P8.7
File Name:            assign5.cpp
Date:                 Nov 23, 2025

Program Description
Purpose:
    This program completes two problems from Chapter 8.
    1) Spell check a file using a dictionary file such as /usr/share/dict/words.
    2) Encrypt or decrypt a text file using a keyword based monoalphabetic cipher.

User Interface:
    You can run with command line arguments or follow prompts.

    Spell check:
        assign5 spell <dictionary_path> <input_path>

    Cipher:
        assign5 crypt -k<KEYWORD> [-e | -d] <input_path> <output_path>

    If you run without arguments the program shows a small menu and asks for what it needs.

Program Output:
    Spell check prints each word from the input that is not found in the dictionary.
    Cipher writes the transformed text to the output file and shows a confirmation message.

Functional Specification
Inputs:
    Paths to files and a keyword for the cipher.
Processing:
    P8.2: Load dictionary words into a vector. Read input words. Clean and lowercase them.
          If a word is not in the dictionary vector, print it.
    P8.7: Build a 26 letter cipher order from the keyword. Map letters to encrypt or decrypt.
Outputs:
    List of unknown words for P8.2. A new file with encrypted or decrypted text for P8.7.

Course Outcomes Demonstrated:
    3) Use arrays (26 element letter maps) and vectors (dictionary words).
    5) Define and call functions and show value and reference parameters.

Source:
    Horstmann, C. S. (2017). Big C++: Late Objects, Enhanced eText (3rd ed.). Wiley.
===============================================================================

PSEUDOCODE (language agnostic)

MAIN
    if no command line arguments
        show menu
        if user chooses spell
            ask for dictionary file path
            ask for input file path
            runSpellCheck(dictionaryPath, inputPath)
        else if user chooses crypt
            ask for E or D
            ask for keyword
            ask for input file path
            ask for output file path
            runCrypt(mode, keyword, inputPath, outputPath)
        end if
        stop
    else
        if first argument is "spell"
            get dictionary_path and input_path
            runSpellCheck(dictionary_path, input_path)
        else if first argument is "crypt"
            read -k keyword and -e or -d and file paths
            runCrypt(mode, keyword, input_path, output_path)
        else
            print help
        end if
        stop
    end if

SPELL CHECK
    load dictionary file words into vector words (lowercase)
    open input file
    for each token in input
        w <- cleanWord(token)  // keep letters and one apostrophe, lowercase
        if w is not empty and w not in words
            print w
        end if
    end for

CIPHER
    keywordUpper <- normalize keyword to uppercase letters only
    keyOrder <- remove duplicates from keywordUpper, keep first occurrence
    for letter from 'Z' down to 'A'
        if letter not in keyOrder
            append letter to keyOrder
        end if
    end for
    make encMap[26] where plain 'A' + i maps to keyOrder[i]
    make decMap[26] which is the inverse map
    open input file and output file
    for each character ch in input
        if letter
            if mode is Encrypt then write encChar(ch) else write decChar(ch)
        else
            write ch as is
        end if
    end for

END PSEUDOCODE

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>

// ----------------- Helper functions -----------------

// Make a lowercase copy of s
std::string toLowerCopy(const std::string& s) {
    std::string t = s;
    for (char& c : t) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return t;
}

// Clean a raw token into a simple word: keep letters and at most one middle apostrophe.
// Convert to lowercase. Trim apostrophes from ends.
std::string cleanWord(const std::string& raw) {
    std::string out;
    out.reserve(raw.size());
    int apostrophes = 0;
    for (char ch : raw) {
        unsigned char u = static_cast<unsigned char>(ch);
        if (std::isalpha(u)) {
            out.push_back(static_cast<char>(std::tolower(u)));
        } else if (ch == '\'' && apostrophes == 0) {
            out.push_back(ch);
            apostrophes = 1;
        }
    }
    if (!out.empty() && out.front() == '\'') out.erase(out.begin());
    if (!out.empty() && out.back() == '\'') out.pop_back();
    return out;
}

// Linear search in a vector of strings.
// Returns true if target is present.
bool containsWord(const std::vector<std::string>& words, const std::string& target) {
    for (const std::string& w : words) {
        if (w == target) return true;
    }
    return false;
}

// Load every token from a text file as a lowercase word into outWords.
// Returns false on file error.
bool loadDictionaryLower(const std::string& dictionaryPath, std::vector<std::string>& outWords) {
    std::ifstream in(dictionaryPath);
    if (!in) return false;
    std::string token;
    while (in >> token) {
        outWords.push_back(toLowerCopy(token));
    }
    return true;
}

// ----------------- P8.2 Spell Check -----------------

void runSpellCheck(const std::string& dictionaryPath, const std::string& inputPath) {
    std::vector<std::string> dictionaryWords;
    if (!loadDictionaryLower(dictionaryPath, dictionaryWords)) {
        std::cerr << "Could not open dictionary file: " << dictionaryPath << "\n";
        return;
    }

    std::ifstream in(inputPath);
    if (!in) {
        std::cerr << "Could not open input file: " << inputPath << "\n";
        return;
    }

    std::cout << "\nWords not found in dictionary:\n";

    std::string token;
    while (in >> token) {
        std::string w = cleanWord(token);
        if (!w.empty() && !containsWord(dictionaryWords, w)) {
            std::cout << w << "\n";
        }
    }
}

// ----------------- P8.7 Cipher from Keyword -----------------

// Keep only letters and uppercase them
std::string normalizeKeyword(const std::string& keyword) {
    std::string out;
    for (char ch : keyword) {
        if (std::isalpha(static_cast<unsigned char>(ch))) {
            out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
        }
    }
    return out;
}

// Build the cipher order: unique letters from keyword, then Z down to A skipping used.
std::string buildKeyOrder(const std::string& keyword) {
    std::string key = normalizeKeyword(keyword);
    bool used[26] = { false };
    std::string order;
    order.reserve(26);

    for (char ch : key) {
        int idx = ch - 'A';
        if (idx >= 0 && idx < 26 && !used[idx]) {
            used[idx] = true;
            order.push_back(ch);
        }
    }
    for (char ch = 'Z'; ch >= 'A'; --ch) {
        int idx = ch - 'A';
        if (!used[idx]) {
            used[idx] = true;
            order.push_back(ch);
        }
        if (ch == 'A') break; // avoid wrap on systems where char is signed
    }
    return order; // length should be 26
}

// Fill encMap where encMap[i] is cipher for plain 'A' + i
void makeEncMap(const std::string& keyOrder, char encMap[26]) {
    for (int i = 0; i < 26; ++i) encMap[i] = keyOrder[i];
}

// Fill decMap where decMap[j] is plain letter for cipher 'A' + j
void makeDecMap(const std::string& keyOrder, char decMap[26]) {
    for (int i = 0; i < 26; ++i) {
        int j = keyOrder[i] - 'A';
        decMap[j] = static_cast<char>('A' + i);
    }
}

// Encrypt one character. Keep case.
char encryptChar(char ch, const char encMap[26]) {
    unsigned char u = static_cast<unsigned char>(ch);
    if (std::isalpha(u)) {
        bool lower = std::islower(u) != 0;
        char up = static_cast<char>(std::toupper(u));
        int idx = up - 'A';
        char mapped = encMap[idx];
        if (lower) mapped = static_cast<char>(std::tolower(static_cast<unsigned char>(mapped)));
        return mapped;
    }
    return ch;
}

// Decrypt one character. Keep case.
char decryptChar(char ch, const char decMap[26]) {
    unsigned char u = static_cast<unsigned char>(ch);
    if (std::isalpha(u)) {
        bool lower = std::islower(u) != 0;
        char up = static_cast<char>(std::toupper(u));
        int idx = up - 'A';
        char mapped = decMap[idx];
        if (lower) mapped = static_cast<char>(std::tolower(static_cast<unsigned char>(mapped)));
        return mapped;
    }
    return ch;
}

enum class CryptMode { Encrypt, Decrypt };

void runCrypt(CryptMode mode, const std::string& keyword,
              const std::string& inputPath, const std::string& outputPath) {
    if (keyword.empty()) {
        std::cerr << "Keyword is required.\n";
        return;
    }

    std::string keyOrder = buildKeyOrder(keyword);
    if (keyOrder.size() != 26) {
        std::cerr << "Could not build a 26 letter key order. Check the keyword.\n";
        return;
    }

    char encMap[26];
    char decMap[26];
    makeEncMap(keyOrder, encMap);
    makeDecMap(keyOrder, decMap);

    std::ifstream in(inputPath, std::ios::binary);
    if (!in) {
        std::cerr << "Could not open input file: " << inputPath << "\n";
        return;
    }
    std::ofstream out(outputPath, std::ios::binary);
    if (!out) {
        std::cerr << "Could not open output file: " << outputPath << "\n";
        return;
    }

    char ch;
    while (in.get(ch)) {
        if (mode == CryptMode::Encrypt) {
            out.put(encryptChar(ch, encMap));
        } else {
            out.put(decryptChar(ch, decMap));
        }
    }

    std::cout << "Done. Output saved to: " << outputPath << "\n";
}

// ----------------- Command line and prompts -----------------

void printHelp() {
    std::cout << "Usage:\n";
    std::cout << "  assign5 spell <dictionary_path> <input_path>\n";
    std::cout << "  assign5 crypt -k<KEYWORD> [-e | -d] <input_path> <output_path>\n";
    std::cout << "Examples:\n";
    std::cout << "  assign5 spell /usr/share/dict/words essay.txt\n";
    std::cout << "  assign5 crypt -kFEATHER -e plain.txt enc.txt\n";
    std::cout << "  assign5 crypt -kFEATHER -d enc.txt plain_out.txt\n";
}

int main(int argc, char* argv[]) {
    // No arguments. Show menu and ask for inputs.
    if (argc <= 1) {
        std::cout << "Assignment 5 Menu\n";
        std::cout << "  1) Spell check a file\n";
        std::cout << "  2) Encrypt or decrypt a file\n";
        std::cout << "Enter 1 or 2: ";
        int choice = 0;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            std::string dictionaryPath, inputPath;
            std::cout << "Enter dictionary file path: ";
            std::getline(std::cin, dictionaryPath);
            std::cout << "Enter input file path to check: ";
            std::getline(std::cin, inputPath);
            runSpellCheck(dictionaryPath, inputPath);
            return 0;
        } else if (choice == 2) {
            std::string keyword, inputPath, outputPath;
            std::cout << "Encrypt or Decrypt (E or D): ";
            char modeChar = 'E';
            std::cin >> modeChar;
            CryptMode mode = (modeChar == 'D' || modeChar == 'd') ? CryptMode::Decrypt : CryptMode::Encrypt;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Enter keyword: ";
            std::getline(std::cin, keyword);
            std::cout << "Enter input file path: ";
            std::getline(std::cin, inputPath);
            std::cout << "Enter output file path: ";
            std::getline(std::cin, outputPath);
            runCrypt(mode, keyword, inputPath, outputPath);
            return 0;
        } else {
            printHelp();
            return 0;
        }
    }

    // With arguments.
    std::string command = argv[1];
    if (command == "spell") {
        if (argc >= 4) {
            runSpellCheck(argv[2], argv[3]);
        } else {
            std::cerr << "Not enough arguments for spell.\n";
            printHelp();
        }
        return 0;
    }
    else if (command == "crypt") {
        std::string keyword;
        CryptMode mode = CryptMode::Encrypt;
        std::string inputPath, outputPath;

        int i = 2;
        for (; i < argc; ++i) {
            std::string a = argv[i];
            if (a.rfind("-k", 0) == 0) {
                keyword = a.substr(2);
            } else if (a == "-e") {
                mode = CryptMode::Encrypt;
            } else if (a == "-d") {
                mode = CryptMode::Decrypt;
            } else {
                inputPath = a;
                ++i;
                if (i < argc) outputPath = argv[i];
                break;
            }
        }

        if (keyword.empty() || inputPath.empty() || outputPath.empty()) {
            std::cerr << "Missing arguments for crypt.\n";
            printHelp();
            return 0;
        }

        runCrypt(mode, keyword, inputPath, outputPath);
        return 0;
    }
    else {
        printHelp();
        return 0;
    }
}

/*
Reflection
    We kept prompts simple and clear. We used a lowercase conversion and a basic cleanWord
    so punctuation does not cause false misses in the spell check.
    We used arrays of length 26 for letter maps and a vector for dictionary words.
    The helper functions show pass by value and pass by reference results where it makes sense.

Teamwork Note
    All group members contributed to design, testing, and review.
    The peer evaluation form is submitted separately.

Sample runs
    assign5 spell /usr/share/dict/words essay.txt
    assign5 crypt -kFEATHER -e plain.txt enc.txt
    assign5 crypt -kFEATHER -d enc.txt plain_out.txt

APA Citation
    Horstmann, C. S. (2017). Big C++: Late Objects, Enhanced eText (3rd ed.).
    Wiley Global Education US.
*/



