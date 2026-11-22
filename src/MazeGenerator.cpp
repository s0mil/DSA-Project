#include "../includes/MazeGenerator.h"
#include<cstdlib>
#include<ctime>

MazeGenerator::MazeGenerator(Maze &m){
    n = m.getSize();
    maze = m.getMaze();

    visited = new bool*[n];

    for(int i = 0; i < n; i++){
        visited[i] = new bool[n];

        for(int j = 0; j < n; j++){
            visited[i][j] = false;
        }
    }
}

MazeGenerator::~MazeGenerator(){
    for(int i = 0; i < n; i++){
        delete[] visited[i];
    }

    delete[] visited;
}

bool MazeGenerator::isSafe(int row, int col){
    if(row < 0 || col < 0 || row >= n || col >= n || visited[row][col] == true)
        return false;

    return true;
}

void MazeGenerator::generateMaze(Maze &m, int startRow, int startCol){

    static bool seeded = false;

    if(!seeded){
        srand(time(0));
        seeded = true;
    }

    m.initialize();

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            visited[i][j] = false;
        }
    }

    generateMazeRecursive(startRow, startCol);

    selectStartEnd(m);
}

void MazeGenerator::generateMazeRecursive(int startRow, int startCol){

    if(!isSafe(startRow, startCol)){
        return;
    }

    int dir[4] = {0, 1, 2, 3};

    for(int i = 3; i > 0; i--){
        int j = rand() % (i + 1);

        int temp = dir[i];
        dir[i] = dir[j];
        dir[j] = temp;
    }

    maze[startRow][startCol] = 0;
    visited[startRow][startCol] = true;

    for(int i = 0; i < 4; i++){

        if(dir[i] == 0){

            if(!isSafe(startRow-2, startCol)) continue;

            maze[startRow-1][startCol] = 0;
            generateMazeRecursive(startRow-2, startCol);
        }

        else if(dir[i] == 1){

            if(!isSafe(startRow+2, startCol)) continue;

            maze[startRow+1][startCol] = 0;
            generateMazeRecursive(startRow+2, startCol);
        }

        else if(dir[i] == 2){

            if(!isSafe(startRow, startCol-2)) continue;

            maze[startRow][startCol-1] = 0;
            generateMazeRecursive(startRow, startCol-2);
        }

        else{

            if(!isSafe(startRow, startCol+2)) continue;

            maze[startRow][startCol+1] = 0;
            generateMazeRecursive(startRow, startCol+2);
        }
    }
}

void MazeGenerator::selectStartEnd(Maze &m){

    int sr, sc, er, ec;

    do{
        sr = rand() % n;
        sc = rand() % n;
    }while(maze[sr][sc] != 0);

    do{
        er = rand() % n;
        ec = rand() % n;
    }while(maze[er][ec] != 0 ||
           (er == sr && ec == sc) ||
           (abs(er - sr) + abs(ec - sc) == 1));

    m.setStartingRow(sr);
    m.setStartingCol(sc);

    m.setEndingRow(er);
    m.setEndingCol(ec);
}