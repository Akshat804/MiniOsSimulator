#include "kernel.h"

int main() {
    Kernel os;

    string command;

    cout << "🔥 Mini OS Simulator Started\n";
    cout << "Type 'help' for commands\n";

    while (true) {
        cout << "\nOS> ";
        cin >> command;

        if (command == "help") {
            cout << "\nAvailable Commands:\n";
            cout << "create <pid> <arrival> <burst>\n";
            cout << "schedule fcfs\n";
            cout << "schedule rr <tq>\n";
            cout << "schedule mlfq\n";
            cout << "memory fifo\n";
            cout << "memory lru\n";
            cout << "deadlock check\n";
            cout << "status\n";
            cout << "clear\n";
            cout << "exit\n";
        }

        else if (command == "create") {
            int pid, at, bt;
            cin >> pid >> at >> bt;
            os.addProcess(pid, at, bt);
            cout << "✅ Process created\n";
        }
        else if (command == "status") {
    cout << "\n📊 System Status\n";
    cout << "Processes loaded: " << os.getProcessCount() << endl;

    os.showProcesses();   // 🔥 ADD THIS LINE
}

        else if (command == "schedule") {
            string algo;
            cin >> algo;

            if (algo == "fcfs") os.runFCFS();
            else if (algo == "rr") {
                int tq; cin >> tq;
                os.runRR(tq);
            }
            else if (algo == "mlfq") os.runMLFQ();
        }

        else if (command == "memory") {
            string algo;
            cin >> algo;

            int n;
            cout << "Enter number of pages: ";
            cin >> n;

            vector<int> pages(n);
            cout << "Enter pages:\n";
            for (int i = 0; i < n; i++) cin >> pages[i];

            int frames;
            cout << "Enter frames: ";
            cin >> frames;

            if (algo == "fifo") os.runPagingFIFO(pages, frames);
            else if (algo == "lru") os.runPagingLRU(pages, frames);
        }

        else if (command == "deadlock") {
            string type;
            cin >> type;

            if (type == "check") {
                int n;
                cout << "Enter number of processes: ";
                cin >> n;

                vector<vector<int>> graph(n);

                int edges;
                cout << "Enter number of dependencies: ";
                cin >> edges;

                cout << "Enter edges (u v means u waits for v):\n";
                while (edges--) {
                    int u, v;
                    cin >> u >> v;
                    graph[u].push_back(v);
                }

                os.detectDeadlock(n, graph);
            }
        }
        else if (command == "banker") {

    int n, m;
    cout << "Processes: ";
    cin >> n;

    cout << "Resources: ";
    cin >> m;

    vector<vector<int>> alloc(n, vector<int>(m));
    vector<vector<int>> maxNeed(n, vector<int>(m));
    vector<int> avail(m);

    cout << "Allocation matrix:\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            cin >> alloc[i][j];

    cout << "Max matrix:\n";
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            cin >> maxNeed[i][j];

    cout << "Available:\n";
    for (int j = 0; j < m; j++)
        cin >> avail[j];

    os.runBanker(n, m, alloc, maxNeed, avail);
}

        else if (command == "status") {
            cout << "\n📊 System Status\n";
            cout << "Processes loaded: " << os.getProcessCount() << endl;
        }

        else if (command == "clear") {
            system("cls");  // use "clear" if on Linux
        }

        else if (command == "exit") {
            cout << "🔻 Shutting down OS...\n";
            break;
        }

        else {
            cout << "❌ Invalid command\n";
        }
    }

    return 0;
}