#include "../includes/ranking.h"
#include <sstream>
#include <cstdio>

void Ranking::merge(AccountRecord arr[], int left, int mid, int right) {
    int i = 0;
    int j = 0;
    int k = left;

    AccountRecord* leftArray = new AccountRecord[mid - left + 1];
    AccountRecord* rightArray = new AccountRecord[right - mid];

    for(int x = 0; x < mid - left + 1; x++) {
        leftArray[x] = arr[left + x];
    }

    for(int x = 0; x < right - mid; x++) {
        rightArray[x] = arr[mid + 1 + x];
    }

    while(i < mid - left + 1 && j < right - mid) {
        if(leftArray[i].score >= rightArray[j].score) {
            arr[k] = leftArray[i];
            i++;
        }
        else {
            arr[k] = rightArray[j];
            j++;
        }
        k++;
    }

    while(i < mid - left + 1) {
        arr[k] = leftArray[i];
        i++;
        k++;
    }

    while(j < right - mid) {
        arr[k] = rightArray[j];
        j++;
        k++;
    }

    delete[] leftArray;
    delete[] rightArray;
}

void Ranking::mergeSort(AccountRecord arr[],int left,int right) {

    if(left < right) {
        int mid = (left + right)/2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void Ranking::createRanking() {
    ifstream file("SignUp.txt");
    if(!file) {
        cout << "Error Opening SignUp.txt" << endl;
        return;
    }
    string line;
    int count = 0;

    while(getline(file, line)) {
        if(line != "") {
            count++;
        }
    }

    file.close();

    if(count == 0) {
        cout << "No Players Found!" << endl;
        return;
    }

    AccountRecord* arr = new AccountRecord[count];

    ifstream readFile("SignUp.txt");

    int index = 0;

    while(getline(readFile, line)) {

        if(line == "") {
            continue;
        }

        stringstream ss(line);

        getline(ss, arr[index].email, '|');
        getline(ss, arr[index].username, '|');
        getline(ss, arr[index].password, '|');

        string savedLevel;
        string savedKeys;
        string savedScore;

        getline(ss, savedLevel, '|');
        getline(ss, savedKeys, '|');
        getline(ss, savedScore);

        arr[index].level = stoi(savedLevel);
        arr[index].keys = stoi(savedKeys);
        arr[index].score = stoi(savedScore);

        index++;
    }

    readFile.close();
    mergeSort(arr, 0, count - 1);
    int rank = 1;
    cout << "\n========== PLAYER RANKING ==========\n";
    for(int i = 0; i < count; i++) {
        if(i>0 && arr[i].score != arr[i-1].score) rank++;
        cout << rank << ". "
             << arr[i].username
             << "     Score: "
             << arr[i].score << endl;
    }
    cout << "====================================\n";

    delete[] arr;

}