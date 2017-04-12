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

int sys_get_max_zombies(void) {
	int my_max_zombies = current->max_zombies;

	if (my_max_zombies == NO_Z_LIMIT) {
		return -EINVAL;
	}

	return my_max_zombies;
}

int sys_get_zombies_count(pid_t pid) {
	if (pid < 0) {
		return -ESRCH;
	}
	task_t* target = find_task_by_pid(pid);
	if (target->max_zombies == NO_Z_LIMIT) {
		return 0;
	}
//	list_t iterator = target->zombies_list;
//	int counter = 0;
//	printk("line 37\n");
//	list_for_each(&iterator, &(target->zombies_list)) {
//		printk("line 39\n");
//		counter++;
//	}
	return target->num_of_zombie_sons;
}


