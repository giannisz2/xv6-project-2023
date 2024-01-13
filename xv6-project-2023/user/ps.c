#include "user.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "kernel/stat.h"


int main(void) {
    struct pstat* pstat = malloc(sizeof(struct pstat));

    if(getpinfo(pstat) < 0) {
        printf("Failure in ps file.\n");
        return -1;
    }
    printpinfo(pstat);
    return 0;
}