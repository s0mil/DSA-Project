
#ifndef GAME_H
#define GAME_H

#include "Maze.h"
#include "MazeGenerator.h"
#include "MazeSolver.h"
#include "player.h"
#include "ranking.h"

class Game{

private:

    Player &player;

    Maze maze;

    MazeGenerator *generator;
    MazeSolver *solver;

    int currentLevel;
    int currentRow;
    int currentCol;
    int totalMoves;
    int minMoves;

    void startLevel(int level);
    void playLevel();
    void movePlayer(int rowChange, int colChange);

    void showHint();
    void showPath();
    void autoPlay();

    void completeLevel();
    void saveProgress();

    void selectLevel();
    void replayLevel();
    void nextLevel();

    void showInstructions();
    void showRanking();

    void demoMode();

public:

    Game(Player &p);
    ~Game();

    void run();
};

#endif
