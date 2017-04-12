#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include "zombies.h"

void do_long_task(int d) {
	short i;
	int j;
	for (j = 0; j < d; j++)
		for (i = 1; i != 0; i++) {
			;
		}
}

int main() {
	int pid = fork();
	if (pid != 0) {
		set_max_zombies(3, pid);
		printf("Im your father and i have limited your max_zombies to 3\n");
		do_long_task(1500);
	} else if (pid == 0) {
		pid_t pid2 = getpid();

		while (get_max_zombies() < 0)
			;
		printf("Im child now my max is: %d\n", get_max_zombies());

		printf("Num of zombie sons is: %d\n", get_zombies_count(pid2));

		printf("trying to fork...\n");
		int grandchild_pid = fork();

		if (grandchild_pid == 0) {
			printf("First fork OK now I die\n");
			return 0;
		}
		do_long_task(1500);
		printf("Num of zombie sons is: %d\n", get_zombies_count(pid2));

		printf("trying to fork again...\n");
				int grandchild_pid2 = fork();

				if (grandchild_pid2 == 0) {
					printf("Second fork OK now I die\n");
					return 0;
				}
				do_long_task(1500);
				printf("Num of zombie sons is: %d\n", get_zombies_count(pid2));


				int grandchild_pid3 = fork();

				if (grandchild_pid3 == 0) {
					printf("Third fork OK now I die\n");
					return 0;
				}
				do_long_task(1500);
				printf("Num of zombie sons is: %d\n", get_zombies_count(pid2));

		if (grandchild_pid != 0) {
			printf("first Z is: %d\n", get_zombie_pid(0));
			printf("second Z is: %d\n", get_zombie_pid(1));
			printf("third Z is: %d\n", get_zombie_pid(2));
			printf("fourth Z is: %d (should be -1)\n", get_zombie_pid(3));
		}
	}

	return 0;
}
