// kernel.h
#ifndef KERNEL_H
#define KERNEL_H

#include <bits/stdc++.h>
using namespace std;

enum State {
    NEW,
    READY,
    RUNNING,
    FINISHED
};

struct PCB {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    State state;

    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
 int level = 0;      // 🔥 for MLFQ (Q0, Q1, Q2)
    int waitTime = 0;
    bool inQueue = false;   // 🔥 IMPORTANT
};
class Kernel {
private:
    int time;
    queue<PCB*> readyQueue;
    vector<PCB> processes;

public:
    Kernel();
    void addProcess(int pid, int arrival, int burst);
    void runFCFS();
    void runRR(int tq);
    void runMLFQ();
    void runPagingFIFO(vector<int>pages,int frames);
void runPagingLRU(vector<int> pages, int frames);
void detectDeadlock(int n, vector<vector<int>>& graph);
int getProcessCount();
void showProcesses();
void runBanker(int n, int m,
               vector<vector<int>>& alloc,
               vector<vector<int>>& maxNeed,
               vector<int>& avail);

bool isSafe(int n, int m,
            vector<vector<int>>& alloc,
            vector<vector<int>>& need,
            vector<int>& avail,
            vector<int>& safeSeq);

void requestResources(int pid,
                      vector<int>& request,
                      int n, int m,
                      vector<vector<int>>& alloc,
                      vector<vector<int>>& need,
                      vector<int>& avail);

};


#endif