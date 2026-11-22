#ifndef MAZE_H
#define MAZE_H

class Maze{
private:
    int n;
    int **maze;
    int currentRow;
    int currentCol;
    int startingRow, startingCol;
    int endingRow, endingCol;

public:
    Maze(int size);
    Maze();

    void initialize();
    void print();

    void setSize(int n);
    int getSize();
    int** getMaze();

    int getStartingRow();
    int getStartingCol();
    int getEndingRow();
    int getEndingCol();

    void setStartingRow(int row);
    void setStartingCol(int col);
    void setEndingRow(int row);
    void setEndingCol(int col);

    void setCurrentRow(int row);
    void setCurrentCol(int col);

    ~Maze();
};

#endif
