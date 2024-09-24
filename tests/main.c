#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char* const args[] = { NULL };

int hide_output() {
    int res;
    int null_id = open("/dev/null", O_WRONLY);
    res = dup2(null_id, STDOUT_FILENO);
    res = dup2(null_id, STDERR_FILENO);
    (void)(res);

    return 0;
}

int run_test_file(char* file) {
    int pid = fork();
    if (pid == 0) {
        hide_output();
        return execv(file, args);
    }
    else return pid;
}

int main(int argc, char** argv) {
    int failure_count = 0;
    int test_status;
    for (int i=1;i<argc;i++) {
        printf("running: %s: ", argv[i]);
        waitpid(run_test_file(argv[i]), &test_status, 0);

        test_status == 0 ? printf("PASS") : (printf("FAIL"), failure_count++);
        printf("\n");
    }

    printf("\n%d tests failed\n", failure_count);

    return 0;
}
