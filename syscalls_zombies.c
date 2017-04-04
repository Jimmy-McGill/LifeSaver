#include <linux/sched.h>

int set_max_zombies(int max_z, pid_t pid) {
	if (pid < 0) {
		errno = ESRCH;
		return -1;
	}
	if (max_z < 0) {
		errno = EINVAL;
		return -1;
	}

	task_t* target = find_task_by_pid(pid);
	target->max_zombies = max_z;

	return 0;
}

int get_max_zombies() {
	int my_max_zombies = current->max_zombies;

	if (my_max_zombies == NO_Z_LIMIT) {
		errno = EINVAL;
		return -1;
	}

	return my_max_zombies;
}
