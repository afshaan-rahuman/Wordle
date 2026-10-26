#include "wordle.h"

int main(){
    WordleGame game;

    //Load the allowed guesses from the dictionary before starting.
    if (!game.initialize("words.txt")){
        cout << "Error: Could not load the dictionary.\n";
        cout << "Make sure words.txt exists and contains valid words.\n";
        return 1;
    }

    game.play();
    return 0;
}
