#include <windows.h>
#include <iostream>
#include <conio.h>
#include "../includes/Game.h"

using namespace std;

Game::Game(Player &p)
    : player(p), maze(9)
{
    generator = nullptr;
    solver = nullptr;

    currentLevel = 1;

    currentRow = 1;
    currentCol = 1;

    totalMoves = 0;
    minMoves = 0;
}

Game::~Game()
{
    delete generator;
    delete solver;
}

void Game::saveProgress()
{
    player.saveProgress();
}

void Game::startLevel(int level)
{
    currentLevel = level;

    int size = 9 + (level - 1) * 2;

    maze.setSize(size);

    delete generator;
    delete solver;

    generator = new MazeGenerator(maze);

    generator->generateMaze(maze, 1, 1);

    currentRow = maze.getStartingRow();
    currentCol = maze.getStartingCol();

    maze.setCurrentRow(currentRow);
    maze.setCurrentCol(currentCol);

    solver = new MazeSolver(maze);

    solver->solve();

    minMoves = solver->getPathSize() - 1;

    totalMoves = 0;

    playLevel();
}

void Game::movePlayer(int rowChange, int colChange){
    int newRow = currentRow + rowChange;
    int newCol = currentCol + colChange;

    if(newRow >= 0 && newRow < maze.getSize() &&
       newCol >= 0 && newCol < maze.getSize() &&
       maze.getMaze()[newRow][newCol] == 0){

        currentRow = newRow;
        currentCol = newCol;
        totalMoves++;

        maze.setCurrentRow(currentRow);
        maze.setCurrentCol(currentCol);
        maze.print();
    }
}

void Game::showHint()
{
    if(player.getKeys() <= 0)
    {
        cout << "\nNo Hint Keys Available!";
        return;
    }

    char hint = solver->getHint(currentRow, currentCol);

    if(hint == 'E')
    {
        cout << "\nHint Not Available From This Position!";
        return;
    }

    player.setKeys(player.getKeys() - 1);

    saveProgress();

    cout << "\nNext Move: ";

    if(hint == 'U')
        cout << "UP";

    else if(hint == 'D')
        cout << "DOWN";

    else if(hint == 'L')
        cout << "LEFT";

    else if(hint == 'R')
        cout << "RIGHT";
}

void Game::showPath()
{
    if(player.getKeys() <= 3)
    {
        cout << "\nNot Enough Keys!";
        return;
    }
    solver->displayShortestPath();

    player.setKeys(player.getKeys() - 3);

    cout << "\nPress any key to continue...";
    _getch();

    maze.setCurrentRow(currentRow);
    maze.setCurrentCol(currentCol);

    maze.print();
}

void Game::autoPlay()
{
    if(maze.getSize() != 9)
    {
        cout << "\nAuto Play is only available for 9x9 demo!";
        return;
    }

    int row = currentRow;
    int col = currentCol;

    if(!solver->startAutoPlay(row, col))
    {
        cout << "\nAuto Play cannot start from this position!";
        return;
    }

    while(solver->autoPlayStep(row, col))
    {
        currentRow = row;
        currentCol = col;

        maze.setCurrentRow(row);
        maze.setCurrentCol(col);

        maze.print();

        Sleep(400);
    }

    cout << "\nAuto Play Completed!";
    cout << "\nPress any key...";
    _getch();
}

void Game::completeLevel()
{
    int earnedScore = 0;

    if(totalMoves > 0)
        earnedScore = (minMoves * 100) / totalMoves;

    player.setScore(player.getScore() + earnedScore);

    player.setKeys(player.getKeys() + 2);

    if(currentLevel > player.getLevel())
        player.setLevel(currentLevel);

    saveProgress();

    cout << "\n\n========== LEVEL COMPLETED ==========";
    cout << "\nLevel: " << currentLevel;
    cout << "\nMinimum Moves: " << minMoves;
    cout << "\nYour Moves: " << totalMoves;
    cout << "\nScore Earned: " << earnedScore;
    cout << "\nTotal Score: " << player.getScore();
    cout << "\nHint Keys: " << player.getKeys();
    cout << "\n=====================================";

    if(currentLevel == 10)
        cout << "\nCongratulations! You completed all levels!";
    
    cout << "\n\nPress any key to return...";
    _getch();
}

void Game::playLevel()
{
    char choice;

    while(true)
    {
        maze.setCurrentRow(currentRow);
        maze.setCurrentCol(currentCol);

        maze.print();

        cout << "\nLevel: " << currentLevel;
        cout << "\nMoves: " << totalMoves;
        cout << "\nHint Keys: " << player.getKeys();


        cout << "\nH = Hint";
        cout << "\nP = Show Path";
        cout << "\nQ = Quit Level\n";

        int key = _getch();

        if (key == 224 || key == 0) {
            key = _getch();

        if (key == 72)       // Up
            movePlayer(-1, 0);
        else if (key == 80)  // Down
            movePlayer(1, 0);
        else if (key == 75)  // Left
            movePlayer(0, -1);
        else if (key == 77)  // Right
            movePlayer(0, 1);
        }

        else if(choice == 'h' || choice == 'H')
        {
            showHint();
            Sleep(1200);
        }

        else if(choice == 'p' || choice == 'P')
        {
            showPath();
        }

        else if(choice == 'q' || choice == 'Q')
        {
            saveProgress();
            return;
        }

    

        if(currentRow == maze.getEndingRow() &&
           currentCol == maze.getEndingCol())
        {
            completeLevel();
            //Sleep(3500);
            return;
        }
    }
}

void Game::nextLevel()
{
    if(player.getLevel() >= 10)
    {
        cout << "\nYou have already unlocked all levels!";
        Sleep(3000);
        return;
    }

    if(currentLevel > player.getLevel())
    {
        cout << "\nComplete the current level first!";
        Sleep(3000);
        return;
    }

    startLevel(currentLevel + 1);
}

void Game::replayLevel()
{
    int level;

    cout << "\nEnter Level to Replay: ";
    cin >> level;

    if(level < 1 || level > player.getLevel())
    {
        cout << "\nYou have not unlocked this level!";
        Sleep(3000);
        return;
    }

    startLevel(level);
}

void Game::selectLevel()
{
    int level;

    cout << "\nUnlocked Levels: 1 - "
         << player.getLevel() + 1;

    cout << "\nEnter Level: ";
    cin >> level;

    if(level < 1 || level > player.getLevel() + 1 ||
       level > 10)
    {
        cout << "\nInvalid or Locked Level!";
        return;
    }

    startLevel(level);
}

void Game::showInstructions(){
    system("cls");

    cout << "\n========== HOW TO PLAY ==========\n";
    cout << "\n1. Use Arrow Keys to move:";
    cout << "\n   Up    (↑) - Move Up";
    cout << "\n   Down  (↓) - Move Down";
    cout << "\n   Left  (←) - Move Left";
    cout << "\n   Right (→) - Move Right";

    cout << "\n\n2. Reach the destination (O) to complete the level.";
    cout << "\n3. Avoid walls.";
    cout << "\n4. Use Hint to get direction (costs 1 key).";
    cout << "\n5. Use Show Path (costs 3 keys).";
    cout << "\n6. Each completed level rewards 2 hint keys.";
    cout << "\n7. Complete all 10 levels to win!";

    cout << "\n\nPress any key to return...";
    _getch();
}

void Game::showRanking()
{
    Ranking ranking;

    ranking.createRanking();

    cout << "\nPress any key to continue...";
    _getch();
}

void Game::demoMode()
{
    maze.setSize(7);

    delete generator;
    delete solver;

    generator = new MazeGenerator(maze);

    generator->generateMaze(maze, 1, 1);

    currentRow = maze.getStartingRow();
    currentCol = maze.getStartingCol();

    maze.setCurrentRow(currentRow);
    maze.setCurrentCol(currentCol);

    solver = new MazeSolver(maze);
    solver->solve();

    int row = currentRow;
    int col = currentCol;

    if(!solver->startAutoPlay(row, col))
    {
        cout << "\nDemo could not start!";
        return;
    }

    while(solver->autoPlayStep(row, col))
    {
        maze.setCurrentRow(row);
        maze.setCurrentCol(col);

        maze.print();

        Sleep(400);
    }

    cout << "\n\nDemo Completed!";
    cout << "\nPress any key...";
    _getch();
}

void Game::run()
{
    int choice;

    while(true)
    {
        system("cls");

        cout << "\n========== MAZE GAME ==========";

        cout << "\nPlayer: " << player.getUsername();
        cout << "\nScore: " << player.getScore();
        cout << "\nHint Keys: " << player.getKeys();
        cout << "\nCompleted Levels: " << player.getLevel();

        cout << "\n\n1. Start / Select Level";
        cout << "\n2. Next Level";
        cout << "\n3. Replay Level";
        cout << "\n4. Demo Mode";
        cout << "\n5. Instructions";
        cout << "\n6. Global Ranking";
        cout << "\n7. Logout";

        cout << "\n\nEnter Choice: ";
        cin >> choice;

        if(choice == 1)
            selectLevel();

        else if(choice == 2)
            nextLevel();

        else if(choice == 3)
            replayLevel();

        else if(choice == 4)
            demoMode();

        else if(choice == 5)
            showInstructions();

        else if(choice == 6)
            showRanking();

        else if(choice == 7)
        {
            saveProgress();
            break;
        }

        else
        {
            cout << "\nInvalid Choice!";
            Sleep(1000);
        }
    }
}
