#ifndef MODEL_H
#define MODEL_H

#define BUF_SIZE 4096
#define SHARED_FILE_PATH "mymsgbuf"

#include <semaphore.h>

typedef struct shmbuf {
    sem_t sem;
    size_t cnt;
    int fd;
    char msgbuf[];
} ShmBuf;

ShmBuf* buf_init();
void model_send_message(ShmBuf* shmp, const char* msg);
char* model_read_messages(ShmBuf* shmp);
char* model_execute_command(const char* cmd);

#endif
