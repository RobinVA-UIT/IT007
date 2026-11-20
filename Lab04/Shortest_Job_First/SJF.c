

#include <stdio.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2

#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; ++i) {
        printf("\n---------------------");
        printf("\nEnter PID: ");
        scanf("%d", &P[i].iPID);

        printf("\nEnter Arrival Time: ");
        scanf("%d", &P[i].iArrival);

        printf("\nEnter Burst Time: ");
        scanf("%d", &P[i].iBurst);
    }
}

void printProcess(int n, PCB P[]) {
    printf("\n======== PROCESS LIST========\n");
    printf("||%6s||%10s||%10s||%10s||%10s||%12s||\n", "PID", "Start", "Finish",
           "Waiting", "Response", "Turnaround");

    for (int i = 0; i < n; ++i) {
        if (P[i].iPID == -1) continue;
        printf("||%6d||%10d||%10d||%10d||%10d||%12d||\n", P[i].iPID,
               P[i].iStart, P[i].iFinish, P[i].iWaiting, P[i].iResponse,
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

int countSameArrivals(PCB P[], int n) {
    int numOfSameArrivals = 1;
    for (int i = 1; i < n; ++i) {
        if (P[i].iArrival == P[0].iArrival)
            ++numOfSameArrivals;
        else
            break;
    }

    return numOfSameArrivals;
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

void calculateAWT(int n, PCB P[]) {
    double avg = 0;

    int numOfProcess = 0;

    for (int i = 0; i < n; ++i) {
        if (P[i].iPID == -1) continue;
        avg += P[i].iWaiting;
        numOfProcess++;
    }

    printf("\nAWT = %f", avg / numOfProcess);
}

void calculateATaT(int n, PCB P[]) {
    double avg = 0;
    int numOfProcess = 0;

    for (int i = 0; i < n; ++i) {
        if (P[i].iPID == -1) continue;
        avg += P[i].iTaT;
        numOfProcess++;
    }

    printf("\nATaT = %f", avg / numOfProcess);
}

int main() {
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[100];  // gantt chart

    int iNumberOfProcess;
    printf("========SHORTEST JOB FIRST========\n");
    printf("Please enter number of Process: ");
    scanf("%d", &iNumberOfProcess);

    if (iNumberOfProcess % 11 == 0) {
        printf("The number of process is not valid.");
        return -1;
    }

    int iRemain = iNumberOfProcess % 11, iReady = 0, iTerminated = 0, iIdle = 0;

    PCB idle;
    idle.iPID = -1;

    inputProcess(iNumberOfProcess % 11, Input);
    quickSort(Input, 0, iNumberOfProcess % 11 - 1, SORT_BY_ARRIVAL);

    // Trong trường hợp các process cùng đến sớm nhất có Burst Time khác nhau
    // 1. Tìm số lượng process có cùng arrivals
    // 2. Sort từ 0 đến cnt-1, với tiêu chí SORT_BY_BURST
    int iCntSameArrivals = countSameArrivals(Input, iNumberOfProcess % 11);
    quickSort(Input, 0, iCntSameArrivals - 1, SORT_BY_BURST);

    // Vì là process đầu, và không trưng dụng, ta set các thông số cho process
    // này
    Input[0].iStart = Input[0].iArrival;
    Input[0].iFinish = Input[0].iStart + Input[0].iBurst;
    Input[0].iResponse = Input[0].iStart - Input[0].iArrival;
    Input[0].iWaiting = Input[0].iResponse;
    Input[0].iTaT = Input[0].iFinish - Input[0].iArrival;

    // Trường hợp process đầu tiên không arrive vào thời điểm 0
    // Ta push idle state vào TerminatedArray
    if (Input[0].iArrival != 0) {
        idle.iStart = 0;
        idle.iArrival = 0;
        idle.iFinish = Input[0].iArrival;
        idle.iWaiting = 0;

        pushProcess(&iTerminated, TerminatedArray, idle);
        iIdle++;
    }

    // Đưa process đầu tiên vào trong ReadyQueue
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);

    while (iTerminated - iIdle < iNumberOfProcess % 11) {
        while (iRemain > 0 && Input[0].iArrival <= ReadyQueue[0].iFinish) {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
        removeProcess(&iReady, 0, ReadyQueue);

        // Nếu ko có process nào Ready, nhưng Remain vẫn còn
        //  => CPU trống, add idle vào ReadyQueue
        if (iReady == 0 && iRemain > 0) {
            idle.iStart = TerminatedArray[iTerminated - 1].iFinish;
            idle.iFinish = Input[0].iArrival;

            pushProcess(&iReady, ReadyQueue, idle);

            ++iIdle;
        } else {
            while (iReady > 0) {
                // Tiếp tục sort theo burst time
                // Vì trước khi kết thúc 1 lần lặp while, sẽ có process mới được
                // push
                quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);

                ReadyQueue[0].iStart = TerminatedArray[iTerminated - 1].iFinish;
                ReadyQueue[0].iFinish =
                    ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
                ReadyQueue[0].iResponse =
                    ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
                ReadyQueue[0].iTaT =
                    ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
                ReadyQueue[0].iWaiting =
                    ReadyQueue[0].iTaT - ReadyQueue[0].iBurst;

                // Push process mới vào ReadyQueue
                while (iRemain > 0 &&
                       Input[0].iArrival <= ReadyQueue[0].iFinish) {
                    pushProcess(&iReady, ReadyQueue, Input[0]);
                    removeProcess(&iRemain, 0, Input);
                }

                // Đưa process ở đầu ReadyQueue vào gantt
                pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
                removeProcess(&iReady, 0, ReadyQueue);
            }
        }
    }

    printf("\n\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    printf("\n\nTerminated:\n");
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iTerminated, TerminatedArray);

    return 0;
}
