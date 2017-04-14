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

	printk("pid: %d, max_zombies: %d, num_of_zombie_sons: %d\n~\n", target->pid, target->max_zombies, target->num_of_zombie_sons);

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
		return -ESRCH;
	}
	if (current->max_zombies == NO_Z_LIMIT) {
		return -EINVAL;
	}
	list_t *itr = NULL;
	list_for_each(itr, &(current->zombies_list)) {
		if (!n) {
			task_t *task_ptr = list_entry(itr, task_t, zombies_list);
			return task_ptr->pid;
		}
		n--;
	}
}

/* n is the NUMBER of zombies to adopt */
int sys_give_up_zombie(int n, pid_t adopter_pid) {
	// --- Errors --- //
	if (n < 0 || n > current->num_of_zombie_sons) {
		printk("pid: %d, max_zombies: %d, num_of_zombie_sons: %d\n~\n", current->pid, current->max_zombies, current->num_of_zombie_sons);
		printk("$$ EINVAL 1 $$\n");
		return -EINVAL;
	}
	if (adopter_pid < 0) {
		printk("pid: %d, max_zombies: %d, num_of_zombie_sons: %d\n~\n", current->pid, current->max_zombies, current->num_of_zombie_sons);
		printk("$$ ESRCH $$\n");
		return -ESRCH;
	}
	task_t *adopter = find_task_by_pid(adopter_pid);
	int new_num_of_zombies = adopter->num_of_zombie_sons + n;
	if (adopter->max_zombies == NO_Z_LIMIT ||
			new_num_of_zombies > adopter->max_zombies) {
		printk("adopter: pid: %d, max_zombies: %d, num_of_zombie_sons: %d\n~\n", adopter->pid, adopter->max_zombies, adopter->num_of_zombie_sons);
		printk("$$ EINVAL 2 $$\n");
		return -EINVAL;
	}

	/*
	 * 14/4/17 By Dor - I added this final part to the Errors section
	 * because of Yehonatan's comment on piazza (between errors and self
	 * adoption).
	 * It basically checks if the calling process' limit wasn't set, and if
	 * so, returns because there is nothing to do (no zombies to give up).
	 * */

	if (current->num_of_zombie_sons == NO_Z_LIMIT) {
		return 0;
	}


	// -------------- //

	list_t *itr;
	adopter->num_of_zombie_sons += n;
	current->num_of_zombie_sons -= n;

	list_for_each(itr, &(current->zombies_list)) {
		if (!n) break;

		task_t *current_task = list_entry(itr, task_t, zombies_list);
		current_task->p_pptr = adopter; 				// updates the new father of the current zombie to be the adopter

		list_t *tmp_prev = itr->prev;

		list_del(itr); 									// deletes the zombie from the calling process' zombies list
		list_add_tail(itr, &(adopter->zombies_list)); 	// adds the zombie to the adopter's zombies list

		task_t *next = current_task->p_osptr;			// marks the brothers of the current zombie in the family tree
		task_t *prev = current_task->p_ysptr;

		/* The if statement handles the case where the current task is the
		 * youngest son of the calling process, where the else statement handles
		 * the more common case, in it the current task has both an older brother
		 * and a younger one.
		 */
		if (current->p_cptr == current_task) {
			if (next) next->p_ysptr = prev; 			// needed in case that there is only 1 zombie
			current->p_cptr = next;						// updates the youngest son
		} else {
			if (next) next->p_ysptr = prev;
			if (prev) prev->p_osptr = next; 
		}

		/* Adds the zombie to the adopter's processes list, as the new
		 * youngest son of the adopter.
		 *
		 * For comfort uses -
		 * 		p_ysptr: younger brother ("right arrow")
		 * 		p_osptr: older brother ("left arrow")
		 * 		p_cptr: from father to youngest son
		 */
		task_t *youngest = adopter->p_cptr;
		if (youngest) youngest->p_ysptr = current_task;
		current_task->p_osptr = youngest;
		adopter->p_cptr = current_task;

		itr = tmp_prev;
		n--;
	}
	return 0;
}
