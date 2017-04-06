#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include "zombies.h"

int main() {
    int pid = fork();
    if (pid != 0) {
    	set_max_zombies(3, pid);
//        printf("father - %d\n", get_max_zombies());
    }
    if (pid == 0) {
        int max_son = get_max_zombies();
//        printf("heung min son - %d\n", max_son);
    }

    return 0;
}
