#include <linux/sched.h>

int sys_set_max_zombies(int max_z, pid_t pid) {
	if (pid < 0) {
		return -ESRCH;
	}
	if (max_z < 0) {
		return -EINVAL;
	}

	task_t* target = find_task_by_pid(pid);
	target->max_zombies = max_z;

	return 0;
}

int sys_get_max_zombies() {
	int my_max_zombies = current->max_zombies;

	if (my_max_zombies == NO_Z_LIMIT) {
		return -EINVAL;
	}

	return my_max_zombies;
}
