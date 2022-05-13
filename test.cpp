#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using std::cout; using std::endl;

int main() {
    pid_t c_pid = fork();

    if (c_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (c_pid > 0) {
        cout << "printed from parent process " << getpid() << endl;
        wait(nullptr);
    } else {
        cout << "printed from child process " << getpid() << endl;
        exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
}
