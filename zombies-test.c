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
		set_max_zombies(0, pid);
		printf("Im your father and i have limited your max_zombies to 0\n");
		do_long_task(1500);
	}	else if (pid == 0)	{
		while (get_max_zombies() < 0);
		printf("Im child now my max is: %d\n", get_max_zombies());
		printf("trying to fork...\n");
		if (fork() < 0) {
			printf("Good job, Im unable to fork :(\n");
		} else {
			printf("[ERROR]\n");
		}
		return 0;
	}

	return 0;
}
