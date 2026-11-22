#include "../includes/Maze.h"
#include<iostream>
#include<windows.h>
#include<cstdlib>
using namespace std;

Maze::Maze(int size){
    n = size;

    startingRow = startingCol = -1;
    endingRow = endingCol = -1;
    currentRow = currentCol = -1;

    maze = new int*[n];

    for(int i = 0; i < n; i++){
        maze[i] = new int[n];
    }

    initialize();
}

Maze::Maze() {}

void Maze::initialize(){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            maze[i][j] = 1;
        }
    }

    startingRow = startingCol = -1;
    endingRow = endingCol = -1;
}

void Maze::setCurrentRow(int row){
    currentRow = row;
}

void Maze::setCurrentCol(int col){
    currentCol = col;
}

void Maze::print(){

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    system("cls");

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){

            if(i == currentRow && j == currentCol)
                cout << "🥸";

            else if(i == endingRow && j == endingCol)
                cout << " O";

            else if(maze[i][j] == 1)
                cout << "██";

            else
                cout << "  ";
        }

        cout << endl;
    }
}

void Maze::setSize(int size){

    for(int i = 0; i < n; i++){
        delete[] maze[i];
    }

    delete[] maze;

    n = size;

    maze = new int*[n];

    for(int i = 0; i < n; i++){
        maze[i] = new int[n];
    }

    currentRow = currentCol = -1;

    initialize();
}

int Maze::getSize(){
    return n;
}

int** Maze::getMaze(){
    return maze;
}

int Maze::getStartingRow(){
    return startingRow;
}

int Maze::getStartingCol(){
    return startingCol;
}

int Maze::getEndingRow(){
    return endingRow;
}

int Maze::getEndingCol(){
    return endingCol;
}

void Maze::setStartingRow(int row){
    startingRow = row;
}

void Maze::setStartingCol(int col){
    startingCol = col;
}

void Maze::setEndingRow(int row){
    endingRow = row;
}

void Maze::setEndingCol(int col){
    endingCol = col;
}

Maze::~Maze(){
    for(int i = 0; i < n; i++){
        delete[] maze[i];
    }

    delete[] maze;
}
