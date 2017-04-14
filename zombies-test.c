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

int test1dor() {
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

int main() {
	// 4 basic processes as presented in the workshop

	int id = 0, g1 = 0, g2 = 0;

	g1 = fork();
	g2 = fork();

	if (g1 != 0 && g2 != 0)
		id = 0;
	if (g1 != 0 && g2 == 0)
		id = 1;
	if (g1 == 0 && g2 != 0)
		id = 2;
	if (g1 == 0 && g2 == 0)
		id = 3;

	// B and S - do not die on us just yet
	if (id == 1 || id == 3) {
		do_long_task(3000);
	}

	/* Give process 'F' (with id = 0) zombie sons and make him give them up
	 * to process 'S' (with id = 2).
	 * */
	if (id == 0 || id == 2) {
		// Set limit for B to be 7, for F to be 5
		if (id == 0) {
			set_max_zombies(5, getpid());
		} else if (id == 2) {
			set_max_zombies(7, getpid());
			do_long_task(3000);
		}

		if (id == 0) {
			int h1 = 0;
			int i = 0;
			for (; i < 3; i++) {
				h1 = fork();
				if (h1 == 0)
					exit(0);
			}
			do_long_task(1500);
			printf("test: Process F now has %d zombie sons. Expected: 3.\n",
					get_zombies_count(getpid()));

			int res1 = give_up_zombie(2, g1);
			if (res1 < 0) {
				printf("[Error %d: line 114]\n", errno);
			}
			printf("test: Process F now has %d zombie sons. Expected: 1.\n",
					get_zombies_count(getpid()));
			printf("test: Process S now has %d zombie sons. Expected: 2.\n",
					get_zombies_count(g1));
		}
	}

	return 0;
}
