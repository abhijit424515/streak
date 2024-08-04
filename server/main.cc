#include "../headers/common.hh"

mqd_t mq;
struct sigevent sev;

void signal_handler(int signo) {
	if (signo != SIGUSR1) return;
	
	Entry e;
	ssize_t bytes_read = mq_receive(mq, (char*)(void*)&e, MSG_MAX_SIZE, NULL);
	if (bytes_read == -1) {
		perror("mq_receive");
		exit(EXIT_FAILURE);
	}
	printf("Message received: UTS = %ld, Cmd = '%s'\n", e.uts, e.msg);

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;
    sev.sigev_value.sival_ptr = NULL;
    if (mq_notify(mq, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }
}

int main() {
	mq_unlink(QUEUE_NAME);

	struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_MAX_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;
    sev.sigev_value.sival_ptr = NULL;
    if (mq_notify(mq, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }

    printf("Receiver is waiting for messages...\n");
    while (1)
        pause();

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
}