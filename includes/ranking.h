#ifndef RANKING_H
#define RANKING_H

#include<iostream>
#include<fstream>
#include<string>

using namespace std;

struct AccountRecord {
    string email;
    string username;
    string password;
    int level;
    int keys;
    int score;
};

class Ranking {
private:
    void merge(AccountRecord arr[], int left, int mid, int right);
    void mergeSort(AccountRecord arr[], int left, int right);

public:
    void createRanking();
};

#endif