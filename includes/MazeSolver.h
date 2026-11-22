#ifndef MAZESOLVER_H
#define MAZESOLVER_H

#include "Maze.h"

class MazeSolver
{
private:
    int** maze;
    int n;

    int startRow, startCol;
    int endRow, endCol;

    bool** visited;

    int* pathRow;
    int* pathCol;
    int pathSize;

    int autoPlayIndex;

    bool solveRecursive(int row, int col);

public:
    MazeSolver(Maze& m);
    ~MazeSolver();

    bool solve();

    int getPathSize();
    int getPathRow(int index);
    int getPathCol(int index);

    char getHint(int row, int col);

    bool startAutoPlay(int row, int col);
    bool autoPlayStep(int &row, int &col);

    void displayShortestPath();
};

#endif
