

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

int sameArrivalTieBreak(PCB P[], int src, int n) {
    int lowest = src;
    for (int i = src + 1; i < n; ++i) {
        if (P[i].iArrival == P[src].iArrival) {
            if (P[i].iBurst < P[lowest].iBurst) lowest = i;
        } else
            break;
    }

    return lowest;
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
    PCB TerminatedArray[100];

    int iNumberOfProcess;
    printf("========SHORTEST JOB FIRST========\n");
    printf("Please input number of Process (modulo 11): ");
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
    // Gọi hàm này để xác định process có Burst Time nhỏ nhất
    int iStartingProcess = sameArrivalTieBreak(Input, 0, iNumberOfProcess % 11);

    // Đưa process đầu tiên vào trong ReadyQueue
    pushProcess(&iReady, ReadyQueue, Input[iStartingProcess]);
    removeProcess(&iRemain, iStartingProcess, Input);

    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    // Trường hợp process đầu tiên không bắt đầu vào thời điểm 0
    // Ta push idle state vào TerminatedArray
    if (ReadyQueue[0].iStart != 0) {
        idle.iStart = 0;
        idle.iArrival = 0;
        idle.iFinish = ReadyQueue[0].iStart;
        idle.iWaiting = 0;

        pushProcess(&iTerminated, TerminatedArray, idle);
        iIdle++;
    }

    if (iNumberOfProcess % 11 == 1) {
        pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
        removeProcess(&iReady, 0, ReadyQueue);
    }

    else {
        while (iTerminated - iIdle < iNumberOfProcess % 11) {
            // Khi các process ở Input đến sau thời điểm kết thúc của process
            // chuẩn bị được chọn, ta đẩy vào ReadyQueue
            while (iRemain > 0 && Input[0].iArrival <= ReadyQueue[0].iFinish) {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
            }

            // Khi trong hàng đợi Ready có 1 process, và vẫn còn process
            // trong Input, ta biết rằng các process còn lại trong Input vẫn
            // chưa arrive.
            // Ta chèn idle state vào ReadyQueue
            if (iReady == 1 && iRemain > 0) {
                idle.iArrival = ReadyQueue[0].iFinish;
                idle.iStart = idle.iArrival;
                idle.iBurst = Input[0].iArrival - ReadyQueue[0].iFinish;
                idle.iFinish = idle.iStart + idle.iBurst;
                idle.iWaiting = 0;

                pushProcess(&iReady, ReadyQueue, idle);
                iIdle++;
            }
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);

            // Push các process trong ReadyQueue vào TerminatedArray
            // Chừa lại đúng 1 process trong ReadyQueue để tính toán thời
            // gian cho các process sau
            while (iReady > 0) {
                // Dispatch: Chọn process có burst thấp nhất
                quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);

                ReadyQueue[0].iStart = TerminatedArray[iTerminated - 1].iFinish;
                ReadyQueue[0].iResponse =
                    ReadyQueue[0].iStart - ReadyQueue[0].iArrival;

                ReadyQueue[0].iFinish =
                    ReadyQueue[0].iStart + ReadyQueue[0].iBurst;

                ReadyQueue[0].iTaT =
                    ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
                ReadyQueue[0].iWaiting =
                    ReadyQueue[0].iTaT - ReadyQueue[0].iBurst;

                // Trong trường hợp process phía sau đến
                // Ta cũng đẩy vào ReadyQueue
                while (iRemain > 0 &&
                       Input[0].iArrival <= ReadyQueue[0].iFinish) {
                    pushProcess(&iReady, ReadyQueue, Input[0]);
                    removeProcess(&iRemain, 0, Input);
                }

                if (iReady > 1) {
                    pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
                    removeProcess(&iReady, 0, ReadyQueue);
                } else
                    break;
            }
        }
    }

    printf("\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    printf("\n\nTerminated:\n");
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iTerminated, TerminatedArray);

    return 0;
}
