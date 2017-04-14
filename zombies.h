#include <errno.h>
#include <termios.h>

#define NO_Z_LIMIT -1 /* For the max_zombies feature */

int set_max_zombies(int max_z, pid_t pid) {
	int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (243), "b" (max_z), "c"(pid)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res);
		return -1;
	}
	return __res;
}

int get_max_zombies() {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (244)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res);
		return -1;
	}
	return __res;
}

int get_zombies_count(pid_t pid) {
	int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (245), "b" (pid)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res);
		return -1;
	}
	return __res;
}

pid_t get_zombie_pid(int n) {
	int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (246), "b" (n)
		: "memory"
	);
	if ((__res) < 0) {
		errno = (-__res);
		return -1;
	}
	return __res;
}

int give_up_zombie(int n, pid_t adopter_pid) {
	int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (247), "b" (n), "c"(adopter_pid)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res);
		return -1;
	}
	return __res;
}
