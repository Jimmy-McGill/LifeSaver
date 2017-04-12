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

	return target->num_of_zombie_sons;
}

pid_t sys_get_zombie_pid(int n) {
	if (n >= current->num_of_zombie_sons) {
		printk("in the kernel2\n");
		return -ESRCH;
	}
	if (current->max_zombies == NO_Z_LIMIT) {
		printk("in the kernel3\n");
		return -EINVAL;
	}
	list_t *itr = NULL;
	list_for_each(itr, &(current->zombies_list)) {
		printk("in loop\n");
		if (!n) {
			printk("in loop in if\n");
			task_t *task_ptr = list_entry(itr, task_t, zombies_list);
			printk("my pid from kernel is: %d\n", task_ptr->pid);
			return task_ptr->pid;
		}
		n--;
	}
}
