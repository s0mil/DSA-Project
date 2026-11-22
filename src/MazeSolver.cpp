#include "../includes/MazeSolver.h"
#include <iostream>
#include <windows.h>

using namespace std;

MazeSolver::MazeSolver(Maze& m)
{
    maze = m.getMaze();
    n = m.getSize();

    startRow = m.getStartingRow();
    startCol = m.getStartingCol();

    endRow = m.getEndingRow();
    endCol = m.getEndingCol();

    pathSize = 0;
    autoPlayIndex = 0;

    visited = new bool*[n];

    for(int i = 0; i < n; i++){
        visited[i] = new bool[n];

        for(int j = 0; j < n; j++){
            visited[i][j] = false;
        }
    }

    pathRow = new int[n * n];
    pathCol = new int[n * n];
}

MazeSolver::~MazeSolver()
{
    for(int i = 0; i < n; i++){
        delete[] visited[i];
    }

    delete[] visited;

    delete[] pathRow;
    delete[] pathCol;
}

bool MazeSolver::solve()
{
    pathSize = 0;
    autoPlayIndex = 0;

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            visited[i][j] = false;
        }
    }

    return solveRecursive(startRow, startCol);
}

bool MazeSolver::solveRecursive(int row, int col)
{
    if(row < 0 || col < 0 || row >= n || col >= n)
        return false;

    if(maze[row][col] == 1 || visited[row][col])
        return false;

    visited[row][col] = true;

    pathRow[pathSize] = row;
    pathCol[pathSize] = col;

    pathSize++;

    if(row == endRow && col == endCol)
        return true;

    if(solveRecursive(row + 1, col))
        return true;

    if(solveRecursive(row, col + 1))
        return true;

    if(solveRecursive(row - 1, col))
        return true;

    if(solveRecursive(row, col - 1))
        return true;

    pathSize--;

    return false;
}

int MazeSolver::getPathSize()
{
    return pathSize;
}

int MazeSolver::getPathRow(int index)
{
    return pathRow[index];
}

int MazeSolver::getPathCol(int index)
{
    return pathCol[index];
}

char MazeSolver::getHint(int row, int col)
{
    for(int i = 0; i < pathSize - 1; i++){

        if(pathRow[i] == row && pathCol[i] == col){

            if(pathRow[i + 1] > row)
                return 'D';

            if(pathRow[i + 1] < row)
                return 'U';

            if(pathCol[i + 1] > col)
                return 'R';

            if(pathCol[i + 1] < col)
                return 'L';
        }
    }

    return 'E';
}

bool MazeSolver::startAutoPlay(int row, int col)
{
    for(int i = 0; i < pathSize; i++){

        if(pathRow[i] == row && pathCol[i] == col){
            autoPlayIndex = i;
            return true;
        }
    }

    return false;
}

bool MazeSolver::autoPlayStep(int &row, int &col)
{
    if(autoPlayIndex >= pathSize - 1)
        return false;

    autoPlayIndex++;

    row = pathRow[autoPlayIndex];
    col = pathCol[autoPlayIndex];

    return true;
}

void MazeSolver::displayShortestPath()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    system("cls");

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){

            if(i == startRow && j == startCol){
                SetConsoleTextAttribute(hConsole, 7);
                cout << "🥸";
            }

            else if(i == endRow && j == endCol){
                SetConsoleTextAttribute(hConsole, 7);
                cout << " O";
            }

            else if(maze[i][j] == 1){
                SetConsoleTextAttribute(hConsole, 7);
                cout << "██";
            }

            else{
                bool onPath = false;

                for(int k = 0; k < pathSize; k++){
                    if(pathRow[k] == i && pathCol[k] == j){
                        onPath = true;
                        break;
                    }
                }

                if(onPath){
                    SetConsoleTextAttribute(hConsole, 12);
                    cout << "██";
                }
                else{
                    SetConsoleTextAttribute(hConsole, 7);
                    cout << "  ";
                }
            }
        }

        cout << endl;
    }

    SetConsoleTextAttribute(hConsole, 7);
}
