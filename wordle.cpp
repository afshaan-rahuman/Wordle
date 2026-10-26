#include "wordle.h"
#include <fstream>
#include <cstdlib>
#include <ctime>

Word::Word() {
    value = "";
}

Word::Word(string value) {
    this->value = value;

    //Convert lowercase ASCII letters to uppercase.
    for (int i = 0; i < value.length(); i++) {
        if (value[i] >= 'a' && value[i] <= 'z') {
            value[i] = value[i] - 'a' + 'A';
        }
    }
}

string Word::getValue() const {
    return value;
}

bool Word::isWellFormed() const {
    //Checks if the word is exactly five letters long.
    if (value.length() != 5) {
        return false;
    }

    //Checks if there are any non-alphabetic characters.
    for (int i = 0; i < 5; i++) {
        if (!((value[i] >= 'A' && value[i] <= 'Z') ||
              (value[i] >= 'a' && value[i] <= 'z'))) {
            return false;
        }
    }

    return true;
}

bool Word::operator==(const Word& other) const {
    return value == other.value;
}

WordDictionary::WordDictionary() {
    wordList = NULL;
    wordCount = 0;
    capacity = 0;
}

WordDictionary::WordDictionary(const WordDictionary& other) {
    wordCount = other.wordCount;
    capacity = other.capacity;
    wordList = capacity > 0 ? new string[capacity] : NULL;

    for (int i = 0; i < wordCount; i++) {
        wordList[i] = other.wordList[i];
    }
}

WordDictionary& WordDictionary::operator=(const WordDictionary& other) {
    if (this != &other) {
        string* copiedWords =
            other.capacity > 0 ? new string[other.capacity] : NULL;

        for (int i = 0; i < other.wordCount; i++) {
            copiedWords[i] = other.wordList[i];
        }

        delete[] wordList;
        wordList = copiedWords;
        wordCount = other.wordCount;
        capacity = other.capacity;
    }

    return *this;
}

WordDictionary::~WordDictionary() {
    delete[] wordList;
}

void WordDictionary::expandStorage() {
    int newCapacity = capacity == 0 ? 16 : capacity * 2;
    string* expandedWords = new string[newCapacity];

    for (int i = 0; i < wordCount; i++) {
        expandedWords[i] = wordList[i];
    }

    delete[] wordList;
    wordList = expandedWords;
    capacity = newCapacity;
}

bool WordDictionary::loadFromFile(const string& filename) {
    //ifstream reads the dictionary directly from words.txt.
    ifstream file(filename.c_str());

    if (!file.is_open()) {
        return false;
    }

    string text;

    //The >> operator reads one word at a time, separated by whitespace.
    while (file >> text) {
        Word word(text);

        if (!word.isWellFormed()) {
            continue;
        }

        string normalized = word.getValue();
        bool alreadyStored = false;

        for (int i = 0; i < wordCount; i++) {
            if (wordList[i] == normalized) {
                alreadyStored = true;
                break;
            }
        }

        //Grow the array only when another unique valid word needs storage.
        if (!alreadyStored) {
            if (wordCount == capacity) {
                expandStorage();
            }

            wordList[wordCount] = normalized;
            wordCount++;
        }
    }

    file.close();
    return wordCount > 0;
}

bool WordDictionary::isValidWord(const Word& word) const {
    if (!word.isWellFormed()) {
        return false;
    }

    //Search the loaded dictionary using a simple linear search.
    for (int i = 0; i < wordCount; i++) {
        if (wordList[i] == word.getValue()) {
            return true;
        }
    }

    return false;
}

Word WordDictionary::getRandomWord() const {
    //rand() selects a random entry from the loaded dictionary.
    int index = rand() % wordCount;
    return Word(wordList[index]);
}

ostream& operator<<(ostream& out, const GuessResult& result) {
    out << result.guess << "\n";

    //Show feedback for each guessed letter.
    for (int i = 0; i < result.feedback.length(); i++) {
        if (result.feedback[i] == 'G') {
            out << "[G]";
        } else if (result.feedback[i] == 'Y') {
            out << "[Y]";
        } else {
            out << "[-]";
        }
    }

    //We return o in order to support cascading.
    return out;
}

WordleGame::WordleGame() {
    attempts = 0;
    finished = false;
}

bool WordleGame::initialize(const string& filename) {
    if (!dictionary.loadFromFile(filename)) {
        return false;
    }

    //Seed the random-number generator before choosing the secret word.
    srand(time(NULL));
    secret = dictionary.getRandomWord();
    attempts = 0;
    finished = false;

    return true;
}

GuessResult WordleGame::evaluate(const Word& guess) const {
    string target = secret.getValue();
    string input = guess.getValue();

    GuessResult result;
    result.guess = input;
    result.feedback = string(5, '-');
    result.won = (guess == secret);

    //Remember which secret-word positions have already been matched.
    //This prevents repeated letters in a guess from being counted twice.
    bool used[5] = {false, false, false, false, false};

    //Pass 1: mark letters that match the secret word at the same position.
    for (int i = 0; i < 5; i++) {
        if (input[i] == target[i]) {
            result.feedback[i] = 'G';
            used[i] = true;
        }
    }

    //Pass 2: find remaining letters that occur at a different position.
    for (int i = 0; i < 5; i++) {
        if (result.feedback[i] == 'G') {
            continue;
        }

        for (int j = 0; j < 5; j++) {
            if (!used[j] && input[i] == target[j]) {
                result.feedback[i] = 'Y';
                used[j] = true;
                break;
            }
        }
    }

    return result;
}

void WordleGame::play() {
    if (finished) {
        cout << "This game has already ended.\n";
        return;
    }

    cout << "\nGuess the five-letter word!\n";
    cout << "G = correct position, Y = wrong position, "
         << "- = absent\n";

    while (attempts < 6 && !finished) {
        string input;

        cout << "\nAttempt " << attempts + 1 << "/6: ";

        if (!(cin >> input)) {
            cout << "\nInput ended. Exiting game.\n";
            finished = true;
            return;
        }

        Word guess(input);

        //Invalid entries do not consume one of the six attempts.
        if (!guess.isWellFormed()) {
            cout << "Enter exactly five alphabetic letters.\n";
            continue;
        }

        if (!dictionary.isValidWord(guess)) {
            cout << "That word is not in the dictionary. Try again.\n";
            continue;
        }

        attempts++;
        GuessResult result = evaluate(guess);
        cout << result << '\n';

        if (result.won) {
            cout << "Congratulations! You guessed the word.\n";
            finished = true;
        }
    }

    if (!finished) {
        cout << "Game over! The word was " << secret.getValue() << ".\n";
        finished = true;
    }
}
