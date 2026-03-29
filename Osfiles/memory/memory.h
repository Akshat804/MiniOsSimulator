#ifndef MEMORY_H
#define MEMORY_H

#include <bits/stdc++.h>
using namespace std;

class MemoryManager {
private:
    int numFrames;
    vector<int> frames;          
    vector<bool> referenceBit;   
    int pointer;                

    map<int,int> TLB;           

public:
    MemoryManager(int framesCount);

    int accessPage(int pid, int page);
    int clockReplace(int page);

    void printFrames();
};

#endif