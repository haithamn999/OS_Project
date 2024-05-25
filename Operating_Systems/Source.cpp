#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <cstring>

using namespace std;

struct Process {
    int id;
    int burst_time;
    int arrival_time;
    int start_time;
    int finish_time;
    int completionTime;
    int turnTime;
    int waitingTime;
};

struct gantt {
    int processId;
    int startTime;
    int endTime;
};

void FCFS(Process processes[], int n, int context_switch) {
    int* wt = new int[n];
    int* tat = new int[n];
    int total_wt = 0, total_tat = 0;
    int total_active_time = 0;
    int total_time = 0;

    // Calculate waiting time
    int* service_time = new int[n];
    service_time[0] = processes[0].arrival_time;
    wt[0] = 0;
    processes[0].start_time = processes[0].arrival_time;
    processes[0].finish_time = processes[0].start_time + processes[0].burst_time;

    for (int i = 1; i < n; i++) {
        service_time[i] = max(service_time[i - 1] + processes[i - 1].burst_time + context_switch, processes[i].arrival_time);
        wt[i] = service_time[i] - processes[i].arrival_time;
        if (wt[i] < 0) {
            wt[i] = 0;
        }
        processes[i].start_time = service_time[i];
        processes[i].finish_time = processes[i].start_time + processes[i].burst_time;
    }
    delete[] service_time;

    // Calculate turnaround time
    for (int i = 0; i < n; i++) {
        tat[i] = processes[i].burst_time + wt[i];
    }

    cout << "Processes " << " Burst time " << " Arrival time "
         << " Waiting time " << " Turn around time\n";

    for (int i = 0; i < n; i++) {
        total_wt += wt[i];
        total_tat += tat[i];
        total_active_time += processes[i].burst_time;
        cout << " " << processes[i].id << "\t\t" << processes[i].burst_time << "\t\t" 
             << processes[i].arrival_time << "\t\t" << wt[i] << "\t\t " << tat[i] << endl;
    }

    cout << "Average waiting time = " << fixed << setprecision(2) << (float)total_wt / (float)n << endl;
    cout << "Average turn around time = " << fixed << setprecision(2) << (float)total_tat / (float)n << endl;

    total_time = processes[0].arrival_time;
    for (int i = 0; i < n; i++) {
        if (i > 0) {
            total_time += context_switch;
        }
        total_time = max(total_time, processes[i].start_time) + processes[i].burst_time;
    }

    float utilization = ((float)total_active_time / total_time) * 100;
    cout << "CPU Utilization: " << fixed << setprecision(2) << utilization << "%" << endl;

    cout << "\nGantt Chart:\n";
    int current_time = processes[0].arrival_time;

    cout << "|";
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].start_time) {
            cout << " Idle |";
            current_time = processes[i].start_time;
        }
        cout << " P" << processes[i].id << " |";
        current_time = processes[i].finish_time;
        if (i < n - 1) {
            cout << " CS |";
            current_time += context_switch;
        }
    }
    cout << endl;

    current_time = processes[0].arrival_time;
    cout << current_time;
    for (int i = 0; n > 0 && i < n; i++) {
        if (current_time < processes[i].start_time) {
            cout << setw(6) << processes[i].start_time;
            current_time = processes[i].start_time;
        }
        cout << setw(6) << processes[i].finish_time;
        current_time = processes[i].finish_time;
        if (i < n - 1) {
            current_time += context_switch;
            cout << setw(6) << current_time;
        }
    }
    cout << endl;

    delete[] wt;
    delete[] tat;
}

void roundRobinScheduling(Process processes[], int n, int time_quantum, int context_switch) {
    gantt* ganttChart = new gantt[100];
    int ganttIndex = 0;
    int* burstArr = new int[n];
    int totalBurstTime = 0;

    for (int i = 0; i < n; i++) {
        burstArr[i] = processes[i].burst_time;
        totalBurstTime += processes[i].burst_time;
    }

    queue<int> q;
    int current_time = 0;
    q.push(0);
    int completed = 0;
    int* mark = new int[n]();
    mark[0] = 1;

    while (completed != n) {
        int index = q.front();
        q.pop();

        if (burstArr[index] == processes[index].burst_time) {
            processes[index].start_time = max(current_time, processes[index].arrival_time);
            current_time = processes[index].start_time;
        }

        gantt segment = { index + 1, current_time, 0 };

        int exec_time = min(time_quantum, burstArr[index]);
        burstArr[index] -= exec_time;
        current_time += exec_time;

        segment.endTime = current_time;
        ganttChart[ganttIndex++] = segment;

        if (burstArr[index] == 0) {
            processes[index].completionTime = current_time;
            processes[index].turnTime = processes[index].completionTime - processes[index].arrival_time;
            processes[index].waitingTime = processes[index].turnTime - processes[index].burst_time;
            completed++;
        }

        for (int i = 1; i < n; i++) {
            if (burstArr[i] > 0 && processes[i].arrival_time <= current_time && mark[i] == 0) {
                mark[i] = 1;
                q.push(i);
            }
        }
        if (burstArr[index] > 0) q.push(index);

        if (q.empty()) {
            for (int i = 1; i < n; i++) {
                if (burstArr[i] > 0) {
                    mark[i] = 1;
                    q.push(i);
                    break;
                }
            }
        }

        if (!q.empty()) {
            current_time += context_switch;
            ganttChart[ganttIndex++] = { -1, segment.endTime, current_time }; // -1 indicates a context switch in Gantt chart
        }
    }

    cout << "Processes " << " Burst time " << " Arrival time "
         << " Waiting time " << " Turn around time\n";

    for (int i = 0; i < n; i++) {
        cout << " " << processes[i].id << "\t\t" << processes[i].burst_time << "\t\t" 
             << processes[i].arrival_time << "\t\t" << processes[i].waitingTime << "\t\t" 
             << processes[i].turnTime << endl;
    }

    float totalWaiting = 0, totalTurn = 0;
    for (int i = 0; i < n; i++) {
        totalWaiting += processes[i].waitingTime;
        totalTurn += processes[i].turnTime;
    }

    float avgWaiting = totalWaiting / n;
    float avgTurn = totalTurn / n;
    float cpuUtilization = (float)totalBurstTime / current_time * 100;
    cout << endl;

    cout << "Average Waiting Time = " << avgWaiting << endl;
    cout << "Average Turnaround Time = " << avgTurn << endl;
    cout << "CPU Utilization = " << cpuUtilization << "%" << endl;

    cout << "\nGantt Chart:\n";
    for (int i = 0; i < ganttIndex; i++) {
        if (ganttChart[i].processId == -1) {
            cout << "| CS ";
        } else {
            cout << "| P" << ganttChart[i].processId << " ";
        }
    }
    cout << "|\n";
    cout << ganttChart[0].startTime;
    for (int i = 0; i < ganttIndex; i++) {
        cout << setw(5) << ganttChart[i].endTime;
    }
    cout << endl;

    delete[] burstArr;
    delete[] mark;
    delete[] ganttChart;
}

int main() {
    ifstream infile("data.txt");
  

    int context_switch, time_quantum, n = 0;
    infile >> context_switch >> time_quantum;

    vector<Process> processes;
    int arrival_time, burst_time;
    while (infile >> arrival_time >> burst_time) {
        Process p;
        p.id = ++n;
        p.arrival_time = arrival_time;
        p.burst_time = burst_time;
        processes.push_back(p);
    }

    infile.close();

    cout << "Choose Scheduling Algorithm:\n1. First-Come, First-Served (FCFS)\n2. Round-Robin (RR)\n";
    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        FCFS(processes.data(), n, context_switch);
        break;
    case 2:
        roundRobinScheduling(processes.data(), n, time_quantum, context_switch);
        break;
    
    }

    return 0;
}
