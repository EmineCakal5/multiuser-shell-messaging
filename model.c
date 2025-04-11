#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "include/model.h"

ShmBuf* buf_init() {
    int fd = shm_open(SHARED_FILE_PATH, O_CREAT | O_RDWR, 0600);
    ftruncate(fd, BUF_SIZE);
    ShmBuf* shmp = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    sem_init(&shmp->sem, 1, 1);
    shmp->cnt = 0;
    memset(shmp->msgbuf, 0, BUF_SIZE - sizeof(ShmBuf));
    return shmp;
}

void model_send_message(ShmBuf* shmp, const char* msg) {
    if (!shmp || !msg) return;

    const char* tag = getenv("USER_TAG");
    if (!tag) tag = "Anon";

    size_t len = strlen(tag) + strlen(msg) + 10;
    if (len >= BUF_SIZE) len = BUF_SIZE - 1;

    sem_wait(&shmp->sem);
    snprintf(shmp->msgbuf, len, "[%s]: %s", tag, msg);
    shmp->msgbuf[len] = '\0';
    shmp->cnt = strlen(shmp->msgbuf);
    sem_post(&shmp->sem);
}

char* model_read_messages(ShmBuf* shmp) {
    if (!shmp || shmp->cnt == 0 || shmp->cnt >= BUF_SIZE) return NULL;
    sem_wait(&shmp->sem);
    char* buffer = malloc(shmp->cnt + 1);
    strncpy(buffer, shmp->msgbuf, shmp->cnt);
    buffer[shmp->cnt] = '\0';
    sem_post(&shmp->sem);
    return buffer;
}

char* model_execute_command(const char* cmd) {
    char* command = strdup(cmd);
    int is_pipe = 0;

    char* pipe_ptr = strstr(command, "|");
    if (pipe_ptr) {
        is_pipe = 1;
        *pipe_ptr = '\0';
    }

    if (is_pipe) {
        char* first_cmd = command;
        char* second_cmd = pipe_ptr + 1;

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            return strdup("[HATA] Pipe oluşturulamadı\n");
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);

            char* args1[64];
            int i = 0;
            char* token = strtok(first_cmd, " ");
            while (token && i < 63) {
                args1[i++] = token;
                token = strtok(NULL, " ");
            }
            args1[i] = NULL;
            execvp(args1[0], args1);
            perror("execvp (ilk komut)");
            exit(1);
        }

        int pipefd2[2];
        if (pipe(pipefd2) == -1) {
            perror("pipe2");
            return strdup("[HATA] 2. Pipe oluşturulamadı\n");
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            close(pipefd[1]);
            close(pipefd2[0]);
            dup2(pipefd[0], STDIN_FILENO);
            dup2(pipefd2[1], STDOUT_FILENO);
            dup2(pipefd2[1], STDERR_FILENO);
            close(pipefd[0]);
            close(pipefd2[1]);

            char* args2[64];
            int i = 0;
            char* token = strtok(second_cmd, " ");
            while (token && i < 63) {
                args2[i++] = token;
                token = strtok(NULL, " ");
            }
            args2[i] = NULL;
            execvp(args2[0], args2);
            perror("execvp (ikinci komut)");
            exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[1]);

        char buffer[1024];
        ssize_t count;
        char* result = malloc(1);
        result[0] = '\0';
        size_t total = 0;

        while ((count = read(pipefd2[0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[count] = '\0';
            total += count;
            result = realloc(result, total + 1);
            strcat(result, buffer);
        }
        close(pipefd2[0]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        free(command);
        return result;
    }

    // normal komut
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return strdup("[HATA] Pipe oluşturulamadı\n");
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return strdup("[HATA] Fork başarısız\n");
    }

    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        char* args[64];
        int i = 0;
        char* token = strtok(command, " ");
        while (token && i < 63) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        execvp(args[0], args);
        perror("execvp");
        exit(1);
    } else {
        close(pipefd[1]);
        char buffer[1024];
        ssize_t count;
        char* result = malloc(1);
        result[0] = '\0';
        size_t total = 0;

        while ((count = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[count] = '\0';
            total += count;
            result = realloc(result, total + 1);
            strcat(result, buffer);
        }
        close(pipefd[0]);
        wait(NULL);
        free(command);
        return result;
    }
}
