#include <iostream>
#include <vector>
#include <string>

//Assignment 4 - EcoSim
//Neil McDonald
//w0225318


using namespace std;


class Game;
class Organism;
class Zombie;
class Human;


//Constants for moves
const int LEFT = 1;
const int RIGHT = 2;
const int DOWN = 3;
const int UP = 4;

//Constant for grid x and y dimension
const int CITY_DIMENSION = 20;


//Constants for the number of each organism to initialize the city with - hardcoded
const int ZOMBIE_COUNT = 50;
const int HUMAN_COUNT = 100;


//the time steps it takes for each organism to breed
const int ZOMBIE_BREED_TIME = 8;
const int HUMAN_BREED_TIME = 3;


//the time steps it takes for a Zombie to starve
const int ZOMBIE_STARVE_TIME = 3;


//number to indicate the type of each organism
const int ZOMBIE = 1;
const int HUMAN = 2;


typedef Organism* OrganismPtr;
typedef Game* GamePtr;


class Game {


    friend class Organism;
    friend class Human;
    friend class Zombie;


private:
    OrganismPtr city[CITY_DIMENSION][CITY_DIMENSION];
    int timeStepCount;
    int generateRandomNumber(int startRange, int endRange) const;


public:
    Game();
    void startGame();
    void takeTimeStep();
    void printCity() const;


};


class Organism {


protected:
    GamePtr currGame;
    int x;
    int y;
    int timeTillBreed;
    int timeStepCount;


    vector<int> getMovesToEmptyCells(int x, int y) const;
    bool isValidCoordinate(int x, int y) const;

    void getCoordinate(int& x, int& y, int move) const;


public:
    Organism(): currGame(nullptr), x(0), y(0), timeTillBreed(0), timeStepCount(0){}
    Organism(GamePtr currGame, int x, int y);
    virtual void breed() = 0;
    virtual void move();
    virtual int getType() = 0;
    virtual bool starves() { return false; }


};


class Zombie: public Organism {


private:

    int timeTillStarve;


    vector<int> getMovesToHumans(int x, int y) const;


public:

    Zombie(): Organism(), timeTillStarve(0){}
    Zombie(GamePtr currGame, int x, int y);
    void breed();
    void move();
    int getType(){ return ZOMBIE; }
    bool starves(){ return timeTillStarve == 0; }


};


class Human: public Organism {


public:
    Human(): Organism(){}
    Human(GamePtr currGame, int x, int y);
    void breed();
    int getType(){ return HUMAN; }


};


int Game::generateRandomNumber(int startRange, int endRange) const {
    return rand() % (endRange - startRange + 1) + startRange;
}


Game::Game(){

    timeStepCount = 0;
    for (int x = 0; x < CITY_DIMENSION; x++)
        for (int y = 0; y < CITY_DIMENSION; y++)
            city[x][y] = nullptr;
}


void Game::startGame(){

    int x,y;
    int ZombieCount = 0;
    int HumanCount = 0;


    while (ZombieCount < ZOMBIE_COUNT){
        x = generateRandomNumber(0, CITY_DIMENSION - 1);
        y = generateRandomNumber(0, CITY_DIMENSION - 1);
        if (city[x][y] != nullptr) continue;
        city[x][y] = new Zombie(this, x, y);
        ZombieCount++;
    }


    while (HumanCount < HUMAN_COUNT){
        x = generateRandomNumber(0, CITY_DIMENSION - 1);
        y = generateRandomNumber(0, CITY_DIMENSION - 1);
        if (city[x][y] != nullptr) continue;
        city[x][y] = new Human(this, x, y);
        HumanCount++;
    }
}


void Game::takeTimeStep(){

    timeStepCount++;


    for (int x = 0; x < CITY_DIMENSION; x++){
        for (int y = 0; y < CITY_DIMENSION; y++){
            if (city[x][y] == nullptr) continue;
            if (city[x][y]->getType() == ZOMBIE)
                city[x][y]->move();
        }
    }


    for (int x = 0; x < CITY_DIMENSION; x++){
        for (int y = 0; y < CITY_DIMENSION; y++){
            if (city[x][y] == nullptr) continue;
            if (city[x][y]->getType() == HUMAN)
                city[x][y]->move();
        }
    }


    for (int x = 0; x < CITY_DIMENSION; x++){
        for (int y = 0; y < CITY_DIMENSION; y++){
            if (city[x][y] == nullptr) continue;
            city[x][y]->breed();
        }
    }


    for (int x = 0; x < CITY_DIMENSION; x++){
        for (int y = 0; y < CITY_DIMENSION; y++){
            if (city[x][y] == nullptr) continue;
            if (city[x][y]->starves()){
                delete city[x][y];
                city[x][y] = nullptr;
            }
        }
    }
}


void Game::printCity() const {

    int humanCounter = 0;
    int zombieCounter = 0;

    for (int x = 0; x < CITY_DIMENSION; x++){
        for (int y = 0; y < CITY_DIMENSION; y++){
            if (city[x][y] == nullptr)
                cout << '-';
            else if (city[x][y]->getType() == HUMAN)
            {
                cout << 'H';
                humanCounter++;
            }
            else if (city[x][y]->getType() == ZOMBIE)
            {
                cout << 'Z';
                zombieCounter++;
            }
        }

        cout << endl;
    }
    cout << "Human Count: " << humanCounter << " Zombie Count: " << zombieCounter;
}


vector<int> Organism::getMovesToEmptyCells(int x, int y) const {
    vector<int> movesToEmptyCells;
    int tempX, tempY;
    for (int move = LEFT; move <= UP; move++){
        tempX = x;
        tempY = y;
        getCoordinate(tempX, tempY, move);
        if (!isValidCoordinate(tempX, tempY)) continue;
        if (currGame->city[tempX][tempY] == nullptr)
            movesToEmptyCells.push_back(move);
    }
    return movesToEmptyCells;
}


bool Organism::isValidCoordinate(int x, int y) const {
    if (x < 0 || x >= CITY_DIMENSION || y < 0 || y >= CITY_DIMENSION)
        return false;
    return true;
}


void Organism::getCoordinate(int& x, int& y, int move) const {
    if (move == LEFT) x--;
    if (move == RIGHT) x++;
    if (move == DOWN) y--;
    if (move == UP) y++;
}


Organism::Organism(GamePtr currGame, int x, int y){
    this->currGame = currGame;
    this->x = x;
    this->y = y;
    timeTillBreed = 0;
    timeStepCount = currGame->timeStepCount;
}


void Organism::move(){
    if (timeStepCount == currGame->timeStepCount) return;
    timeStepCount++;
    timeTillBreed--;
    int randomMove = currGame->generateRandomNumber(LEFT, UP);
    int newX = x;
    int newY = y;
    getCoordinate(newX, newY, randomMove);
    if (isValidCoordinate(newX, newY)){
        if (currGame->city[newX][newY] != nullptr) return;
        currGame->city[x][y] = nullptr;
        currGame->city[newX][newY] = this;
        x = newX;
        y = newY;
    }
}


vector<int> Zombie::getMovesToHumans(int x, int y) const {
    vector<int> movesToHumans;
    int tempX, tempY;
    for (int move = LEFT; move <= UP; move++){
        tempX = x;
        tempY = y;
        getCoordinate(tempX, tempY, move);
        if (!isValidCoordinate(tempX, tempY)) continue;
        if (currGame->city[tempX][tempY] == nullptr) continue;
        if (currGame->city[tempX][tempY]->getType() == HUMAN)
            movesToHumans.push_back(move);
    }
    return movesToHumans;
}


Zombie::Zombie(GamePtr currGame, int x, int y): Organism(currGame, x, y){
    timeTillStarve = ZOMBIE_STARVE_TIME;
    timeTillBreed = ZOMBIE_BREED_TIME;
}



void Zombie::breed(){
    if (timeTillBreed > 0) return;
    vector<int> movesToHumans = getMovesToHumans(x, y);
    timeStepCount++;
    timeTillStarve = ZOMBIE_STARVE_TIME;
    int randomMove = movesToHumans[currGame->generateRandomNumber(0, movesToHumans.size() - 1)];
    int HumanX = x;
    int HumanY = y;
    getCoordinate(HumanX, HumanY, randomMove);
    delete currGame->city[HumanX][HumanY];
    currGame->city[HumanX][HumanY] = new Zombie(currGame, HumanX, HumanY);
    x = HumanX;
    y = HumanY;

}


void Zombie::move(){
    if (timeStepCount == currGame->timeStepCount) return;
    vector<int> movesToHumans = getMovesToHumans(x, y);
    if (movesToHumans.size() == 0){
        Organism::move();
        timeTillStarve--;
        return;
    }
    timeStepCount++;
    timeTillStarve = ZOMBIE_STARVE_TIME;
    int randomMove = movesToHumans[currGame->generateRandomNumber(0, movesToHumans.size() - 1)];
    int HumanX = x;
    int HumanY = y;
    getCoordinate(HumanX, HumanY, randomMove);
    delete currGame->city[HumanX][HumanY];
    currGame->city[HumanX][HumanY] = this;
    currGame->city[x][y] = nullptr;
    x = HumanX;
    y = HumanY;
}


Human::Human(GamePtr currGame, int x, int y): Organism(currGame, x, y){
    timeTillBreed = HUMAN_BREED_TIME;
}


void Human::breed(){
    if (timeTillBreed > 0) return;
    vector<int> validMoves = getMovesToEmptyCells(x, y);
    if (validMoves.size() == 0) return;
    int randomMove = validMoves[currGame->generateRandomNumber(0, validMoves.size() - 1)];
    int newX = x;
    int newY = y;
    getCoordinate(newX, newY, randomMove);
    currGame->city[newX][newY] = new Human(currGame, newX, newY);
    timeTillBreed = HUMAN_BREED_TIME;
}


int main(){
    char userInput;
    Game game;

    cout << "WELCOME TO ZOMBIE VS. HUMANS" << endl;
    cout << "Please enter to start the game, anything else to end" << endl;
    cin.get(userInput);

    if (userInput != '\n'){
        cout << "Ending Program" << endl;
        exit(0);
    }


    game.startGame();
    game.printCity();
    cout << endl;


    cout << "Please enter to generate a timestep, anything else to abort: " << endl;
    cin.get(userInput);


    while(userInput == '\n'){
        game.takeTimeStep();
        game.printCity();
        cout << endl;
        cout << "Please enter to generate a timestep, anything else to abort: " << endl;
        cin.get(userInput);
    }

    cout << "Ending Program" << endl;

    return 0;
}

