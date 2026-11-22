#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#include "Maze.h"

class MazeGenerator{
private:
    int n;
    int **maze;
    bool **visited;

    bool isSafe(int row, int col);

    void generateMazeRecursive(int startRow, int startCol);

    void selectStartEnd(Maze &m);

public:
    MazeGenerator(Maze &m);
    ~MazeGenerator();

    void generateMaze(Maze &m, int startRow, int startCol);
};

#endif