// 16519829 zy19829 Masrur SOBIROV

#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <chrono>
#include <thread>

using namespace std;

size_t LevenDist(const string &s1, const string &s2);
void startGame(ifstream &file);

// RANDOM NUBMER GENERATOR
unsigned seed = chrono::system_clock::now().time_since_epoch().count();
auto rng = default_random_engine(seed);

class WordLadder
{
    private:
        string startWord, endWord;
        vector<string> solution;   // solution to word ladder 
        
    public:       
        WordLadder(vector<string> wrds) // costruct a word ladder
        {
            this->startWord = wrds.back(); // set start word
            solution.push_back(startWord); // push back the starting word as first word in solution

            // search for a word which has no letters matching with starting word
            for(auto const& str: wrds) 
            {
                size_t diffr = LevenDist(startWord, str); // compute and save the difference
                if(diffr == startWord.length()) // if whole word is not similar to starting word
                {
                    endWord = str;  // set ending word
                    break;
                }
            }
        }
        // setters and getters:
        string getEndWord()
        {
            return this->endWord;
        } 
        
        string getStartWord()
        {
            return this->startWord;
        }
        // add word given by user to solution of ladder 
        void addWord(string word)
        {
            solution.push_back(word);
        }
        vector<string> getSolution()
        {
            return solution;
        }
        
};

class Game
{
    private:
        size_t level = 1;
        size_t difficulty = 0;
        string hint;
        vector<string> words;

    public:
        Game() // construct
        {   // get difficulty for the game

            std::cout << "Choose difficulty for your game:\n'1' - easy\n'2' - medium\n'3' - hard" << endl;
            std::cin >> difficulty;

            if(difficulty > 3 || difficulty < 1)
            {
                std::cout << "Invalid input!" << endl;
                exit(EXIT_FAILURE);
            }
        }
        // set words user can use to 'n'-letter words where 'n' is difficulty + 2. EX: difficulty = 1, 1+2 = 3-letter words.
        void setWords(ifstream &file)
        {
            string line;
            while(file.good())
            {
                file >> line; // read words
                if(line.length() == difficulty + 2)
                {
                    words.push_back(line); // save n-letter words
                }
            }

            shuffle(begin(words), end(words), rng); // shuffle randomly the words
        }
        // fuzzy search a most similar word to current word in ladder and show it to user as hint
        void getHint(WordLadder &ladder)
        {
            size_t temp = 100; // temporary number
            
            shuffle(begin(words), end(words), rng);

            for(auto const& str: words) 
            {
                size_t diffr = LevenDist(ladder.getSolution().back(), str); // compute and save the difference between last word in solution and str
                if(diffr < temp && diffr != 0) // if new word has less difference
                {
                    hint = str; // then save the most similar string
                    temp = diffr;
                }
            }
            
            cout << "HINT: " << hint << endl; // show the hint (possible next step for ladder)
        }
        // print current state of word ladder
        void printCurrState(WordLadder &ladder)
        {
            std::cout << "\n" << ladder.getStartWord() << " -> ";
            for(size_t i = 1; i < ladder.getSolution().size(); i++)
            {
                std::cout << ladder.getSolution().at(i) << " -> ";
            }
            std::cout << "... -> " << ladder.getEndWord() << endl;
        }
        // when finished print solution to a word ladder
        void printSolution(WordLadder &ladder)
        {
            std::cout << ladder.getStartWord() << " -> ";
            for(size_t i = 1; i < ladder.getSolution().size(); i++)
            {
                std::cout << ladder.getSolution().at(i) << " -> ";
            }
            std::cout << ladder.getEndWord() << "\n" << endl;
        }

        void shuffleWords()
        {
            shuffle(begin(words), end(words), rng);
        }
        // getters and setters
        vector<string> getWords()
        {
            return this->words;
        }

        size_t getDifficulty()
        {
            return this->difficulty;
        }

        size_t getLevel()
        {
            return this->level;
        }
        
        void incrLevel()
        {
            level++;
        }
};

int main(int argc, char * argv[])
{
    if(argc != 1)
    {
        std::cout << "Usage: ./wordladder" << endl;   
        return 1;
    }  
    
    ifstream wordFile("wordlist.txt");  // Open the word list/dictionary

    if(wordFile.fail())
    {   
        std::cout << "Cannot open file\n"; // Opening the file failed
        return 1;       
    }

    std::cout << "\nWORD LADDER!\nGet from one word to another by only changing one letter each turn.\nWords with changed letters must be existing correct words.\n\n";
    std::cout << "For example:\nSEEK -> ... -> FIND\nSEEK -> SEED -> SEND -> FEND -> FIND\n\n";
    std::cout << "Type 'H' to get a possible word if you're stuck.\n";
    std::cout << "Type 'Q' to finish the game and see your result.\n" << endl;

    startGame(wordFile);       
    
    wordFile.close();
               
    return 0;
}

void startGame(ifstream &file)
{
    string input;
    bool exists;

    Game game; // create a game object
    game.setWords(file); // find and set words to n-letter words from word list

    while(1)
    { 
        std::cout << "\nLEVEL " << game.getLevel() << ":\n";

        game.shuffleWords(); // shuffle the words to have random starting/ending words
        
        WordLadder ladder(game.getWords());  // create a word ladder with the words from game

        while(LevenDist(ladder.getSolution().back(), ladder.getEndWord()) != 1) // while the current last word in solution and ending word has more than 1 differences
        {
            game.printCurrState(ladder); // current state of ladder
            
            std::cin >> input;

            while(input == "H") 
            {
                game.getHint(ladder); // show a hint
                std::cin >> input;
            }
            if(input == "Q") // quit
            {
                std::cout << "You finished " << game.getLevel()-1 << " level(s) on difficulty: " << game.getDifficulty() << endl;
                return;
            }

            exists = 0;
            if(LevenDist(input, ladder.getSolution().back()) == 1) // if input correctly has only one letter changed
            {
                for(size_t i = 0; i < game.getWords().size(); i++)
                {
                    if(input == game.getWords().at(i)) // if input is a correct existing word
                    {
                        ladder.addWord(input); // add it to solution of ladder
                        exists = 1;
                        break;
                    }
                }

                if(!exists) // word doesn't exist
                {
                    std::cout << "\nIncorrect word." << endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
            else 
            {
                std::cout << "\nOnly one letter must be changed!" << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        std::cout << "\nLevel Completed!!!" << endl;

        game.printSolution(ladder); // print solution to ladder
        game.incrLevel();  // increment the level of the game
        ladder.getSolution().clear();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

// Levenshtein distance algorithm used from: https://rosettacode.org/wiki/Levenshtein_distance#C.2B.2B
// Takes two strings and returns the Levenshtein distance between them.
size_t LevenDist(const std::string & s1, const std::string & s2) 
{
    const size_t m(s1.length());
    const size_t n(s2.length());

    if (m == 0) return n;
    if (n == 0) return m;

    size_t * costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; k++) costs[k] = k;

    size_t i = 0;
    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j) 
        {
            size_t upper = costs[j + 1];
            if ( * it1 == * it2) 
            {
                costs[j + 1] = corner;
            } 
            else 
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    size_t result = costs[n];
    delete[] costs;

    return result;
}
