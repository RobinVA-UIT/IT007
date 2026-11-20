// Quantum time = 3

#include <stdio.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2

#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst, iOrgBurst;
    int iStart, iOrgStart, iFinish, iWaiting, iResponse, iTaT, iBegan;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; ++i) {
        printf("\n-----------------------");
        printf("\nEnter PID: ");
        scanf("%d", &P[i].iPID);

        printf("\nEnter Arrival Time: ");
        scanf("%d", &P[i].iArrival);

        printf("\nEnter Burst Time: ");
        scanf("%d", &P[i].iBurst);

        P[i].iBegan = 0;
        P[i].iOrgBurst = P[i].iBurst;
    }
}

void printProcess(int n, PCB P[]) {
    printf("\n======== PROCESS LIST========\n");
    printf("||%6s||%10s||%10s||%10s||%10s||%12s||\n", "PID", "Start", "Finish",
           "Waiting", "Response", "Turnaround");

    for (int i = 0; i < n; ++i) {
        printf("||%6d||%10d||%10d||%10d||%10d||%12d||\n", P[i].iPID,
               P[i].iOrgStart, P[i].iFinish, P[i].iWaiting, P[i].iResponse,
               P[i].iTaT);
    }
}
void exportGanttChart(int n, PCB P[]) {
    printf("\n");
    for (int i = 0; i < n; ++i) {
        char label[16];
        if (P[i].iPID == -1)
            snprintf(label, sizeof label, "Idle");
        else
            snprintf(label, sizeof label, "P%d", P[i].iPID);
        printf("|%-8s", label);
    }
    printf("|\n");

    printf("%-9d", P[0].iStart);
    for (int i = 0; i < n; ++i) {
        printf("%-9d", P[i].iFinish);
    }
    printf("\n");
}
void pushProcess(int* n, PCB P[], PCB Q) {
    P[*(n)] = Q;
    (*n)++;
}

void removeProcess(int* n, int index, PCB P[]) {
    if (*n < 1) return;
    if (*n == 1) {
        (*n)--;
        return;
    }

    for (int i = index + 1; i < (*n); ++i) {
        P[i - 1] = P[i];
    }

    (*n)--;
}

static int getKey(PCB p, int iCriteria) {
    switch (iCriteria) {
        case SORT_BY_PID:
            return p.iPID;
        case SORT_BY_START:
            return p.iStart;
        case SORT_BY_BURST:
            return p.iBurst;
        case SORT_BY_ARRIVAL:
            return p.iArrival;
        default:
            return 0;
    }
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low >= high) return;

    switch (iCriteria) {
        case SORT_BY_PID:
            break;
        case SORT_BY_START:
            break;
        case SORT_BY_BURST:
            break;
        case SORT_BY_ARRIVAL:
            break;
        default:
            printf("\nCriteria is not valid.\n");
            return;
    }

    int i = low, j = high;
    PCB pivot = P[low + (high - low) / 2];
    PCB temp;
    while (i <= j) {
        while (getKey(P[i], iCriteria) < getKey(pivot, iCriteria)) ++i;
        while (getKey(P[j], iCriteria) > getKey(pivot, iCriteria)) --j;

        if (i <= j) {
            temp = P[i];
            P[i++] = P[j];
            P[j--] = temp;
        }
    }

    if (low < j) quickSort(P, low, j, iCriteria);
    if (high > i) quickSort(P, i, high, iCriteria);
}

void addingReadyProcess(PCB Input[], PCB ReadyQ[], int* iInput, int* iReady,
                        int iTime) {
    for (int i = 0; i < (*iInput); ++i) {
        if (Input[i].iArrival <= iTime) {
            pushProcess(iReady, ReadyQ, Input[i]);
            removeProcess(iInput, i, Input);

            --i;
        }
    }
}

void calculateAWT(int n, PCB P[]) {
    double avg = 0;

    for (int i = 0; i < n; ++i) {
        avg += P[i].iWaiting;
    }

    printf("\nAWT = %f", avg / n);
}

void calculateATaT(int n, PCB P[]) {
    double avg = 0;

    for (int i = 0; i < n; ++i) {
        avg += P[i].iTaT;
    }

    printf("\nATaT = %f", avg / n);
}

int main() {
    PCB Input[10];
    PCB ReadyQueue[11];
    PCB TerminatedArray[10];
    PCB Gantt[100];

    int iNumberOfProcess;
    printf("========ROUND ROBIN (Quantum Time = 3)========\n");
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    if (iNumberOfProcess < 1 || iNumberOfProcess > 10) {
        printf("The number of process is not valid.");
        return -1;
    }

    int iRemain = iNumberOfProcess % 11, iReady = 0, iTerminated = 0,
        iGantt = 0, iTime = 0, iQ = 3;

    // PCB idle để chỉ trạng thái idle của CPU, khi process mới chưa đến
    PCB idle, temp;
    idle.iPID = -1;

    inputProcess(iNumberOfProcess % 11, Input);
    // Ban đầu ta sort theo Arrival Time.
    // Đến sớm thì được xếp trước.
    quickSort(Input, 0, iNumberOfProcess % 11 - 1, SORT_BY_ARRIVAL);

    // Trường hợp process đầu tiên không bắt đầu vào thời điểm 0
    // Ta push idle state vào gantt chart
    if (Input[0].iArrival != 0) {
        idle.iStart = 0;
        idle.iArrival = 0;
        idle.iFinish = Input[0].iArrival;  // Idle kết thúc khi process đến
        idle.iWaiting = 0;

        pushProcess(&iGantt, Gantt, idle);
        iTime += idle.iFinish;
    }

    idle.iStart = 0;

    temp.iPID = -2;

    int firstRun = 1;
    while (iTerminated < iNumberOfProcess % 11) {
        // Bắt đầu scheduling
        // Đẩy process từ Input vào Ready
        if (firstRun) {
            addingReadyProcess(Input, ReadyQueue, &iRemain, &iReady, iTime);
            firstRun = 0;
        }
        // Nếu Ready trống, nhưng vẫn còn process trong Input
        //==> CPU rảnh, chèn idle vào gantt
        else if (iReady == 0 && iRemain > 0) {
            idle.iStart = iTime;

            idle.iFinish = Input[0].iArrival;

            pushProcess(&iGantt, Gantt, idle);

            iTime = idle.iFinish;

            // Thêm mới process vào Ready
            addingReadyProcess(Input, ReadyQueue, &iRemain, &iReady, iTime);
        }

        // Nếu là process chưa từng được run
        //=> Set start time thành thời gian hiện tại
        if (ReadyQueue[0].iBegan == 0) {
            ReadyQueue[0].iStart = ReadyQueue[0].iOrgStart = iTime;

            ReadyQueue[0].iBegan++;
        }

        // Nếu burst time vừa đủ hoặc bé hơn Quantum time
        //==> Push vào gantt, không push lại vào Ready queue nữa
        if (ReadyQueue[0].iBurst <= iQ) {
            iTime += ReadyQueue[0].iBurst;

            // Thêm process trong Input
            // Vì trong quá trình running, process mới có thể đến
            addingReadyProcess(Input, ReadyQueue, &iRemain, &iReady, iTime);

            ReadyQueue[0].iFinish = iTime;

            // Tính toán các chỉ số
            ReadyQueue[0].iResponse =
                ReadyQueue[0].iOrgStart - ReadyQueue[0].iArrival;

            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

            ReadyQueue[0].iWaiting =
                ReadyQueue[0].iTaT - ReadyQueue[0].iOrgBurst;

            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            pushProcess(&iGantt, Gantt, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
        }

        // Trường hợp burst time cao hơn quantum time
        // Ta thêm process vào gantt, nhưng vẫn đẩy lại vào Ready với
        // burst thấp hơn.
        else {
            iTime += iQ;
            ReadyQueue[0].iBurst -= iQ;

            addingReadyProcess(Input, ReadyQueue, &iRemain, &iReady, iTime);

            pushProcess(&iReady, ReadyQueue, ReadyQueue[0]);

            // TH1: Nếu sau khi add process vào Ready, nhưng số lượng trong
            // Ready queue vẫn bằng 2
            //=> Chưa thêm vào gantt, cho chạy tiếp.

            // TH2: Ready queue có số lượng khác 2
            //=> Nhường CPU cho process khác, đẩy process hiện tại vào gantt
            if (iReady != 2) {
                ReadyQueue[0].iFinish = iTime;
                pushProcess(&iGantt, Gantt, ReadyQueue[0]);

                ReadyQueue[iReady - 1].iStart = iTime;
            }

            removeProcess(&iReady, 0, ReadyQueue);
        }
    }

    printf("\n\n===== RR Scheduling (Quantum time = 3) =====\n");
    exportGanttChart(iGantt, Gantt);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    printf("\n\nTerminated:\n");
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iTerminated, TerminatedArray);

    return 0;
}
