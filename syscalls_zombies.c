#include <linux/sched.h>

int sys_set_max_zombies(int max_z, pid_t pid) {
	if (max_z < 0) {
		return -EINVAL;
	}
	if (pid < 0) {
		return -ESRCH;
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
	if (n < 0 || n >= current->num_of_zombie_sons) {
		return -EINVAL;
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
	return 0;
}

/* n is the NUMBER of zombies to adopt */
int sys_give_up_zombie(int n, pid_t adopter_pid) {
	// --- Errors --- //
	if (n < 0 || n > current->num_of_zombie_sons) {
		return -EINVAL;
	}
	if (adopter_pid < 0) {
		return -ESRCH;
	}
	task_t *adopter = find_task_by_pid(adopter_pid);

	if (adopter->max_zombies == NO_Z_LIMIT) {
		return -EINVAL;
	}

	if (adopter != current) {
		int new_num_of_zombies = adopter->num_of_zombie_sons + n;
		if (new_num_of_zombies > adopter->max_zombies) {
			return -EINVAL;
		}

	} else { // SELF ADOPTION
		 if (current->num_of_zombie_sons > current->max_zombies) {
			 return -EINVAL;
		 }
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

	adopter->num_of_zombie_sons += n;
	current->num_of_zombie_sons -= n;

	list_t *itr;
	list_for_each(itr, &(current->zombies_list)) {
		if (!n) break;
		task_t *current_task = list_entry(itr, task_t, zombies_list);
		REMOVE_LINKS(current_task); //remove itr's task from the process's tasks list

		current_task->p_pptr = adopter; 				// updates the new father of the current zombie to be the adopter

		list_t *tmp_prev = itr->prev;

		list_del(itr); 									// deletes the zombie from the calling process' zombies list
		list_add_tail(itr, &(adopter->zombies_list)); 	// adds the zombie to the adopter's zombies list
		SET_LINKS(current_task); // Adds the zombie to the adopter's processes list

		itr = tmp_prev;
		n--;
	}
	return 0;
}
