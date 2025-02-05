#include <cwutils/cwcmd.h>
#include <cwutils/cwarena.h>

#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

CwCmd cwcmd_create(CwArena a, const char* cmd) {
    CwCmd output;
    output.ptr = cwnew(&a, char*);
    output.ptr[0] = cmd;
    output.size = 1;
    output.memory = a;

    return output;
}

void cwcmd_push_arg(CwCmd* self, const char* arg) {
    *(const char**)(cwnew(&self -> memory, char*)) = (char*)(arg);
}

int cwcmd_run(CwCmd* self) {
    // terminate the array with a NULL pointer
	(void)(cwnew(&self -> memory, char*));

	const char* name = self -> ptr[0];
	const char **args = (self -> ptr + 1);
	char* environment[] = { NULL };

	int pid = fork();
	if (pid == 0) {
    	int err = execve(name, (char* const*)args, environment);
    	return err;
	} 

	else {
    	int status;
    	do {
        	waitpid(pid, &status, 0);
    	} while (WIFEXITED(status) == false);

    	return WEXITSTATUS(status);
	}
}
