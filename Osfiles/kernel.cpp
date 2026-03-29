// kernel.cpp
#include "kernel.h"

Kernel::Kernel() {
    time = 0;
}

void Kernel::addProcess(int pid, int at, int bt) {
    PCB p;
    p.pid = pid;
    p.arrivalTime = at;
    p.burstTime = bt;
    p.remainingTime = bt;
    p.state = READY;

    processes.push_back(p);
}
void Kernel::runFCFS() {
    int completed = 0;
    int n = processes.size();

    queue<PCB*> readyQueue;
    int time = 0;
    PCB* current = nullptr;

    vector<pair<int,int>> gantt;

    while (completed < n) {

        // Add arriving processes
        for (auto &p : processes) {
            if (p.arrivalTime == time) {
                readyQueue.push(&p);
            }
        }

        // Pick next process
        if (current == nullptr && !readyQueue.empty()) {
            current = readyQueue.front();
            readyQueue.pop();
            current->state = RUNNING;

            cout << "Time " << time << ": Running Process "
                 << current->pid << endl;
        }

        // Execute
        if (current != nullptr) {
            gantt.push_back({current->pid, time});

            current->remainingTime--;

            if (current->remainingTime == 0) {
                current->completionTime = time + 1;
                current->turnaroundTime =
                    current->completionTime - current->arrivalTime;
                current->waitingTime =
                    current->turnaroundTime - current->burstTime;

                cout << "Process " << current->pid
                     << " finished at time " << time + 1 << endl;

                current->state = FINISHED;
                current = nullptr;
                completed++;
            }
        } else {
            gantt.push_back({-1, time}); // idle
        }

        time++;
    }

    // 📊 Print results
    cout << "\nPID\tAT\tBT\tCT\tTAT\tWT\n";
    for (auto &p : processes) {
        cout << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << "\t"
             << p.completionTime << "\t"
             << p.turnaroundTime << "\t"
             << p.waitingTime << endl;
    }

    // 🧾 Gantt Chart
    cout << "\nGantt Chart:\n";
    for (auto &g : gantt) {
        if (g.first == -1)
            cout << "| Idle ";
        else
            cout << "| P" << g.first << " ";
    }
    cout << "|\n";
    double totalWT = 0, totalTAT = 0;

for (auto &p : processes) {
    totalWT += p.waitingTime;
    totalTAT += p.turnaroundTime;
}

cout << "\n📊 Performance Metrics\n";
cout << "Average Waiting Time = " << totalWT / processes.size() << endl;
cout << "Average Turnaround Time = " << totalTAT / processes.size() << endl;

// CPU Utilization
int idleTime = 0;
for (auto &g : gantt) {
    if (g.first == -1) idleTime++;
}

double cpuUtil = 100.0 * (gantt.size() - idleTime) / gantt.size();
cout << "CPU Utilization = " << cpuUtil << "%\n";
}

void Kernel::runRR(int tq) {

    // 🔥 RESET (CRITICAL FIX)
    for (auto &p : processes) {
        p.remainingTime = p.burstTime;
        p.state = READY;
        p.inQueue = false;
        p.completionTime = 0;
        p.turnaroundTime = 0;
        p.waitingTime = 0;
    }

    int n = processes.size();
    int completed = 0;
    int time = 0;

    queue<PCB*> readyQueue;
    PCB* current = nullptr;

    vector<pair<int,int>> gantt;

    while (completed < n) {

        // ✅ Add arriving processes (ONLY ONCE)
        for (auto &p : processes) {
            if (p.arrivalTime <= time && !p.inQueue &&
                p.remainingTime > 0 && p.state == READY) {
                readyQueue.push(&p);
                p.inQueue = true;
            }
        }

        // ✅ Clean queue (remove finished)
        while (!readyQueue.empty() && readyQueue.front()->state == FINISHED) {
            readyQueue.pop();
        }

        // ✅ Pick next process
        if (current == nullptr && !readyQueue.empty()) {
            current = readyQueue.front();
            readyQueue.pop();

            current->inQueue = false;
            current->state = RUNNING;

            cout << "Time " << time << ": Running Process "
                 << current->pid << endl;
        }

        if (current != nullptr) {

            int runTime = min(tq, current->remainingTime);

            for (int i = 0; i < runTime; i++) {

                gantt.push_back({current->pid, time});
                current->remainingTime--;
                time++;

                // ✅ Add arrivals DURING execution
                for (auto &p : processes) {
                    if (p.arrivalTime <= time && !p.inQueue &&
                        p.remainingTime > 0 && p.state == READY) {
                        readyQueue.push(&p);
                        p.inQueue = true;
                    }
                }

                if (current->remainingTime == 0)
                    break;
            }

            // ✅ If finished
            if (current->remainingTime == 0) {

                current->completionTime = time;
                current->turnaroundTime = time - current->arrivalTime;
                current->waitingTime =
                    current->turnaroundTime - current->burstTime;

                cout << "Process " << current->pid
                     << " finished at time " << time << endl;

                current->state = FINISHED;
                current->inQueue = false;
                current = nullptr;
                completed++;
            }
            else {
                // ✅ Preempt and push back
                current->state = READY;
                current->inQueue = true;
                readyQueue.push(current);
                current = nullptr;
            }
        }
        else {
            // CPU idle
            gantt.push_back({-1, time});
            time++;
        }
    }

    // 📊 Print results
    cout << "\nPID\tAT\tBT\tCT\tTAT\tWT\n";
    for (auto &p : processes) {
        cout << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << "\t"
             << p.completionTime << "\t"
             << p.turnaroundTime << "\t"
             << p.waitingTime << endl;
    }

    // 📈 Gantt Chart
    cout << "\nGantt Chart:\n";
    for (auto &g : gantt) {
        if (g.first == -1)
            cout << "| Idle ";
        else
            cout << "| P" << g.first << " ";
    }
    cout << "|\n";
    double totalWT = 0, totalTAT = 0;

for (auto &p : processes) {
    totalWT += p.waitingTime;
    totalTAT += p.turnaroundTime;
}

cout << "\n📊 Performance Metrics\n";
cout << "Average Waiting Time = " << totalWT / processes.size() << endl;
cout << "Average Turnaround Time = " << totalTAT / processes.size() << endl;

// CPU Utilization
int idleTime = 0;
for (auto &g : gantt) {
    if (g.first == -1) idleTime++;
}

double cpuUtil = 100.0 * (gantt.size() - idleTime) / gantt.size();
cout << "CPU Utilization = " << cpuUtil << "%\n";
}
void Kernel::runMLFQ() {
    int time = 0, completed = 0, n = processes.size();

    queue<PCB*> Q0, Q1, Q2;
    PCB* current = nullptr;

    vector<pair<int,int>> gantt;

    while (completed < n) {

        // 🔹 Add arriving processes to Q0
        for (auto &p : processes) {
            if (p.arrivalTime <= time && !p.inQueue &&
                p.remainingTime > 0 && p.state != FINISHED) {
                p.level = 0;
                Q0.push(&p);
                p.inQueue = true;
            }
        }

        // 🔹 Clean queues
        while (!Q0.empty() && Q0.front()->state == FINISHED) Q0.pop();
        while (!Q1.empty() && Q1.front()->state == FINISHED) Q1.pop();
        while (!Q2.empty() && Q2.front()->state == FINISHED) Q2.pop();

        // 🔹 Pick highest priority
        if (current == nullptr) {
            if (!Q0.empty()) {
                current = Q0.front(); Q0.pop();
            }
            else if (!Q1.empty()) {
                current = Q1.front(); Q1.pop();
            }
            else if (!Q2.empty()) {
                current = Q2.front(); Q2.pop();
            }

            if (current != nullptr) {
                current->inQueue = false;
                current->state = RUNNING;

                cout << "Time " << time << ": Running P"
                     << current->pid << " (Q" << current->level << ")\n";
            }
        }

        if (current != nullptr) {

            int tq;
            if (current->level == 0) tq = 2;
            else if (current->level == 1) tq = 4;
            else tq = current->remainingTime;

            int runTime = min(tq, current->remainingTime);

            for (int i = 0; i < runTime; i++) {

                gantt.push_back({current->pid, time});
                current->remainingTime--;
                time++;

                // 🔹 Add arrivals during execution
                for (auto &p : processes) {
                    if (p.arrivalTime <= time && !p.inQueue &&
                        p.remainingTime > 0 && p.state != FINISHED) {
                        p.level = 0;
                        Q0.push(&p);
                        p.inQueue = true;
                    }
                }

                // 🔥 MID-QUANTUM PREEMPTION
                if (!Q0.empty() && current->level > 0) {
                    current->state = READY;
                    current->inQueue = true;

                    if (current->level == 1) Q1.push(current);
                    else Q2.push(current);

                    current = nullptr;
                    break;
                }

                if (current->remainingTime == 0) break;
            }

            // 🔥 END-QUANTUM PREEMPTION (THIS WAS YOUR MISSING PIECE)
            if (current != nullptr && !Q0.empty() && current->level > 0) {
                current->state = READY;
                current->inQueue = true;

                if (current->level == 1) Q1.push(current);
                else Q2.push(current);

                current = nullptr;
                continue;
            }

            // 🔹 If finished
            if (current != nullptr && current->remainingTime == 0) {
                current->completionTime = time;
                current->turnaroundTime = time - current->arrivalTime;
                current->waitingTime =
                    current->turnaroundTime - current->burstTime;

                cout << "Process " << current->pid
                     << " finished at time " << time << "\n";

                current->state = FINISHED;
                current->inQueue = false;
                current = nullptr;
                completed++;
            }
            else if (current != nullptr) {
                // 🔥 Demotion
                if (current->level == 0) {
                    current->level = 1;
                    Q1.push(current);
                }
                else if (current->level == 1) {
                    current->level = 2;
                    Q2.push(current);
                }
                else {
                    Q2.push(current);
                }

                current->state = READY;
                current->inQueue = true;
                current = nullptr;
            }
        }
        else {
            gantt.push_back({-1, time});
            time++;
        }

    }

    // 📊 Print results
    cout << "\nPID\tAT\tBT\tCT\tTAT\tWT\n";
    for (auto &p : processes) {
        cout << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << "\t"
             << p.completionTime << "\t"
             << p.turnaroundTime << "\t"
             << p.waitingTime << "\n";
    }

    // 📈 Gantt Chart
    cout << "\nGantt Chart:\n";
    for (auto &g : gantt) {
        if (g.first == -1) cout << "| Idle ";
        else cout << "| P" << g.first << " ";
    }
    cout << "|\n";
  

double totalWT = 0, totalTAT = 0;

for (auto &p : processes) {
    totalWT += p.waitingTime;
    totalTAT += p.turnaroundTime;
}

cout << "\n📊 Performance Metrics\n";
cout << "Average Waiting Time = " << totalWT / processes.size() << endl;
cout << "Average Turnaround Time = " << totalTAT / processes.size() << endl;

// CPU Utilization
int idleTime = 0;
for (auto &g : gantt) {
    if (g.first == -1) idleTime++;
}

double cpuUtil = 100.0 * (gantt.size() - idleTime) / gantt.size();
cout << "CPU Utilization = " << cpuUtil << "%\n";
}
void Kernel::runPagingFIFO(vector<int> pages, int frames) {

    vector<int> memory(frames, -1);
    queue<int> q;

    int pageFaults = 0;

    cout << "\nStep\tPage\tFrames\t\tStatus\n";
    cout << "-------------------------------------------\n";

    for (int i = 0; i < pages.size(); i++) {

        int page = pages[i];
        bool hit = false;

        // Check hit
        for (int j = 0; j < frames; j++) {
            if (memory[j] == page) {
                hit = true;
                break;
            }
        }

        string status;

        if (hit) {
            status = "HIT";
        } else {
            pageFaults++;
            status = "FAULT";

            if (q.size() < frames) {
                // Fill empty slot
                for (int j = 0; j < frames; j++) {
                    if (memory[j] == -1) {
                        memory[j] = page;
                        q.push(page);
                        break;
                    }
                }
            } else {
                int old = q.front();
                q.pop();

                // Replace
                for (int j = 0; j < frames; j++) {
                    if (memory[j] == old) {
                        memory[j] = page;
                        break;
                    }
                }

                q.push(page);
                status += " (replaced " + to_string(old) + ")";
            }
        }

        // Print row
        cout << i+1 << "\t" << page << "\t[ ";
        for (int j = 0; j < frames; j++) {
            if (memory[j] == -1) cout << "- ";
            else cout << memory[j] << " ";
        }
        cout << "]\t" << status << "\n";
    }

    cout << "\nTotal Page Faults (FIFO) = " << pageFaults << endl;
    double faultRate = (double)pageFaults / pages.size();
cout << "Page Fault Rate = " << faultRate << endl;
}
void Kernel::runPagingLRU(vector<int> pages, int frames){
vector<int> memory(frames, -1);
    unordered_map<int,int> lastUsed;

    int pageFaults = 0;

    cout << "\nStep\tPage\tFrames\t\tStatus\n";
    cout << "-------------------------------------------\n";

    for (int i = 0; i < pages.size(); i++) {

        int page = pages[i];
        bool hit = false;

        // Check hit
        for (int j = 0; j < frames; j++) {
            if (memory[j] == page) {
                hit = true;
                break;
            }
        }

        string status;

        if (hit) {
            status = "HIT";
        } else {
            pageFaults++;
            status = "FAULT";

            int replaceIndex = -1;

            // Empty slot
            for (int j = 0; j < frames; j++) {
                if (memory[j] == -1) {
                    replaceIndex = j;
                    break;
                }
            }

            if (replaceIndex == -1) {
                int lruTime = INT_MAX;
                int lruPage = -1;

                for (int j = 0; j < frames; j++) {
                    if (lastUsed[memory[j]] < lruTime) {
                        lruTime = lastUsed[memory[j]];
                        lruPage = memory[j];
                        replaceIndex = j;
                    }
                }

                status += " (replaced " + to_string(lruPage) + ")";
            }

            memory[replaceIndex] = page;
        }

        lastUsed[page] = i;

        // Print row
        cout << i+1 << "\t" << page << "\t[ ";
        for (int j = 0; j < frames; j++) {
            if (memory[j] == -1) cout << "- ";
            else cout << memory[j] << " ";
        }
        cout << "]\t" << status << "\n";
    }

    cout << "\nTotal Page Faults (LRU) = " << pageFaults << endl;
    double faultRate = (double)pageFaults / pages.size();
cout << "Page Fault Rate = " << faultRate << endl;
}
bool dfs(int node, vector<vector<int>>& graph,
         vector<int>& visited, vector<int>& recStack) {

    visited[node] = 1;
    recStack[node] = 1;

    for (int neighbor : graph[node]) {
        if (!visited[neighbor]) {
            if (dfs(neighbor, graph, visited, recStack))
                return true;
        }
        else if (recStack[neighbor]) {
            return true;  // cycle found
        }
    }

    recStack[node] = 0;
    return false;
}

void Kernel::detectDeadlock(int n, vector<vector<int>>& graph) {

    vector<int> visited(n, 0);
    vector<int> recStack(n, 0);

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            if (dfs(i, graph, visited, recStack)) {
                cout << "🔥 Deadlock detected!\n";
                return;
            }
        }
    }

    cout << "✅ No Deadlock\n";
}
int Kernel::getProcessCount() {
    return processes.size();
}
void Kernel::showProcesses() {
    cout << "\nPID\tAT\tBT\n";
    for (auto &p : processes) {
        cout << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << endl;
    }
}
bool Kernel::isSafe(int n, int m,
                   vector<vector<int>>& alloc,
                   vector<vector<int>>& need,
                   vector<int>& avail,
                   vector<int>& safeSeq) {

    vector<int> work = avail;
    vector<bool> finish(n, false);

    for (int count = 0; count < n; count++) {

        bool found = false;

        for (int i = 0; i < n; i++) {

            if (!finish[i]) {

                bool possible = true;

                for (int j = 0; j < m; j++) {
                    if (need[i][j] > work[j]) {
                        possible = false;
                        break;
                    }
                }

                if (possible) {
                    for (int j = 0; j < m; j++)
                        work[j] += alloc[i][j];

                    safeSeq.push_back(i);
                    finish[i] = true;
                    found = true;
                }
            }
        }

        if (!found) return false;
    }

    return true;
}
void Kernel::requestResources(int pid,
                             vector<int>& request,
                             int n, int m,
                             vector<vector<int>>& alloc,
                             vector<vector<int>>& need,
                             vector<int>& avail) {

    // Step 1: Check request <= need
    for (int j = 0; j < m; j++) {
        if (request[j] > need[pid][j]) {
            cout << "❌ Error: Request exceeds need\n";
            return;
        }
    }

    // Step 2: Check request <= available
    for (int j = 0; j < m; j++) {
        if (request[j] > avail[j]) {
            cout << "⏳ Resources not available, process must wait\n";
            return;
        }
    }

    // Step 3: Pretend allocation
    for (int j = 0; j < m; j++) {
        avail[j] -= request[j];
        alloc[pid][j] += request[j];
        need[pid][j] -= request[j];
    }

    // Step 4: Check safety
    vector<int> safeSeq;

    if (isSafe(n, m, alloc, need, avail, safeSeq)) {
        cout << "✅ Request granted\nSafe sequence: ";
        for (int x : safeSeq) cout << "P" << x << " ";
        cout << endl;
    }
    else {
        // rollback
        for (int j = 0; j < m; j++) {
            avail[j] += request[j];
            alloc[pid][j] -= request[j];
            need[pid][j] += request[j];
        }

        cout << "❌ Request denied (unsafe state)\n";
    }
}
void Kernel::runBanker(int n, int m,
                      vector<vector<int>>& alloc,
                      vector<vector<int>>& maxNeed,
                      vector<int>& avail) {

    vector<vector<int>> need(n, vector<int>(m));

    // Calculate NEED
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = maxNeed[i][j] - alloc[i][j];

    vector<int> safeSeq;

    if (isSafe(n, m, alloc, need, avail, safeSeq)) {
        cout << "✅ Initial state is SAFE\nSequence: ";
        for (int x : safeSeq) cout << "P" << x << " ";
        cout << endl;
    }
    else {
        cout << "❌ Initial state NOT safe\n";
        return;
    }

    // 🔥 Ask for request
    int pid;
    cout << "Enter process id for request: ";
    cin >> pid;

    vector<int> request(m);
    cout << "Enter request vector:\n";
    for (int j = 0; j < m; j++) cin >> request[j];

    requestResources(pid, request, n, m, alloc, need, avail);

}