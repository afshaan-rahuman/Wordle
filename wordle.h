#ifndef WORDLE_H
#define WORDLE_H

#include <iostream>
#include <string>
using namespace std;

//Represents a single five-letter word used by the game.
class Word{
private:
    string value;

public:
    Word();
    Word(string value);

    string getValue() const;
    bool isWellFormed() const;
    bool operator==(const Word& other) const;
};

//Stores dictionary words in a dynamically sized array and provides methods to load and validate words.
class WordDictionary{
private:
    string* wordList;
    int wordCount;
    int capacity;

    void expandStorage();

public:
    WordDictionary();
    WordDictionary(const WordDictionary& other);
    WordDictionary& operator=(const WordDictionary& other);
    ~WordDictionary();

    bool loadFromFile(const string& filename);
    bool isValidWord(const Word& word) const;
    Word getRandomWord() const;
};

//Stores the guess and its feedback for one turn.
class GuessResult{
public:
    string guess;
    string feedback;
    bool won;

    friend ostream& operator<<(ostream& out, const GuessResult& result);
};

//Controls the game flow, attempts, and win/loss conditions.
class WordleGame{
private:
    WordDictionary dictionary;
    Word secret;
    int attempts;
    bool finished;

    GuessResult evaluate(const Word& guess) const;

public:
    WordleGame();

    bool initialize(const string& filename);
    void play();
};

#endif