

#include <stdio.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2

#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst, iOrgBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT, iBegan, iFinished;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; ++i) {
        printf("\nEnter PID: ");
        scanf("%d", &P[i].iPID);

        printf("\nEnter Arrival Time: ");
        scanf("%d", &P[i].iArrival);

        printf("\nEnter Burst Time: ");
        scanf("%d", &P[i].iBurst);

        P[i].iBegan = 0;
        P[i].iFinished = 0;
        P[i].iOrgBurst = P[i].iBurst;
    }
}

void printProcess(int n, PCB P[]) {
    printf("\n======== PROCESS LIST========\n");
    printf("||%6s||%10s||%10s||%10s||%10s||%12s||\n", "PID", "Start", "Finish",
           "Waiting", "Response", "Turnaround");

    for (int i = 0; i < n; ++i) {
        if (P[i].iPID == -1 || P[i].iFinished == 0) continue;
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

int numOfSameArrival(PCB P[], int n) {
    int cnt = 1;

    for (int i = 1; i < n; ++i) {
        if (P[i].iArrival == P[0].iArrival) {
            ++cnt;
        } else
            return cnt;
    }

    return cnt;
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
        if (P[i].iPID == -1 || P[i].iFinished == 0) continue;
        avg += P[i].iWaiting;
        numOfProcess++;
    }

    printf("\nAWT = %f", avg / numOfProcess);
}

void calculateATaT(int n, PCB P[]) {
    double avg = 0;
    int numOfProcess = 0;

    for (int i = 0; i < n; ++i) {
        if (P[i].iPID == -1 || P[i].iFinished == 0) continue;
        avg += P[i].iTaT;
        numOfProcess++;
    }

    printf("\nATaT = %f", avg / numOfProcess);
}

int main() {
    PCB Input[11];
    PCB TerminatedArray[100];  // TerminatedArray đóng vai trò làm gantt

    int iNumberOfProcess;
    printf("========SHORTEST REMAINING TIME FIRST========\n");
    printf("Please input number of Process (modulo 11): ");
    scanf("%d", &iNumberOfProcess);

    if (iNumberOfProcess % 11 == 0) {
        printf("The number of process is not valid.");
        return -1;
    }

    int iRemain = iNumberOfProcess % 11, iReady = 0, iTerminated = 0,
        iCompleted = 0, iTime = 0;

    PCB idle, temp;
    idle.iPID = -1;

    inputProcess(iNumberOfProcess % 11, Input);
    quickSort(Input, 0, iNumberOfProcess % 11 - 1, SORT_BY_ARRIVAL);

    // Trong số các process đứng đầu sau sort (có arrival bằng nhau)
    //, ta cần sắp xếp lại theo burst time
    int iNumOfSameArrival = numOfSameArrival(Input, iRemain);
    quickSort(Input, 0, iNumOfSameArrival - 1, SORT_BY_BURST);

    // Trường hợp process đầu tiên không bắt đầu vào thời điểm 0
    // Ta push idle state vào TerminatedArray
    if (Input[0].iArrival != 0) {
        idle.iStart = 0;
        idle.iArrival = 0;
        idle.iFinish = Input[0].iArrival;
        idle.iWaiting = 0;

        pushProcess(&iTerminated, TerminatedArray, idle);
        iTime += idle.iFinish;
    }

    idle.iStart = 0;

    // Cần 1 biến xác định index của process vừa chạy trước đó
    int lastRunning = -1;

    while (iCompleted < iNumberOfProcess % 11) {
        int idx = -1;

        // Cứ mỗi đơn vị time, ta lại tìm process đã arrive có burst bé nhất
        for (int i = 0; i < iRemain; ++i) {
            if (iTime >= Input[i].iArrival &&
                (idx == -1 || Input[i].iBurst < Input[idx].iBurst)) {
                idx = i;
            }
        }
        if (lastRunning == -1) lastRunning = idx;

        // Trường hợp CPU trống
        if (idx == -1) {
            // Nếu mới bắt đầu idle, ta set các giá trị arrival và start
            if (idle.iStart == 0) {
                idle.iArrival = idle.iStart = iTime;
            }
            // Nếu trước đó đã idle, ta chỉ cần cập nhật time
            else
                iTime++;
        }
        // Trường hợp đã xác định được process cần chạy
        else {
            // Nếu iStart của idle khác 0
            //=> Trước đó CPU đang idle, cần thêm vào gantt
            if (idle.iStart != 0) {
                idle.iFinish = iTime;
                pushProcess(&iTerminated, TerminatedArray, idle);

                // Reset idle
                idle.iStart = 0;
            }

            // Nếu process hiện tại có burst cao hơn idx
            if (lastRunning != idx) {
                // Đẩy process trước đó vừa chạy vào trong gantt, nhưng không
                // xóa chúng trong Input
                temp = Input[lastRunning];
                temp.iFinish = iTime;
                pushProcess(&iTerminated, TerminatedArray, temp);

                lastRunning = idx;
                continue;
            }
            // Nếu process hiện tại = process trước đó
            else {
                // Trường hợp iBegan = 0: Process vừa mới được bắt đầu chạy
                if (Input[idx].iBegan == 0) {
                    Input[idx].iStart = Input[idx].iFinish = iTime;
                    Input[idx].iBegan++;
                    continue;
                }

                // Cập nhật time và burst
                ++iTime;
                Input[idx].iBurst--;
                // Nếu hết burst, ta cập nhật các chỉ số, rồi push vào gantt
                if (Input[idx].iBurst == 0) {
                    Input[idx].iFinish = iTime;
                    Input[idx].iTaT = Input[idx].iFinish - Input[idx].iArrival;
                    Input[idx].iResponse =
                        Input[idx].iStart - Input[idx].iArrival;
                    Input[idx].iWaiting =
                        Input[idx].iTaT - Input[idx].iOrgBurst;
                    Input[idx].iFinished = 1;

                    pushProcess(&iTerminated, TerminatedArray, Input[idx]);
                    removeProcess(&iRemain, idx, Input);

                    lastRunning = -1;
                    iCompleted++;
                }
            }
        }
    }

    printf("\n===== SRTF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    printf("\n\nTerminated:\n");
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iTerminated, TerminatedArray);

    return 0;
}
