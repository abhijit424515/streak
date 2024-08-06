#include "file.hh"

Shma *sm;

mqd_t mq;
struct sigevent sev;

void signal_handler(int signo) {
	if (signo == SIGUSR1) {
		Entry e;
		ssize_t bytes_read = mq_receive(mq, (char*)(void*)&e, MSG_MAX_SIZE, NULL);
		if (bytes_read == -1) {
			perror("mq_receive");
			exit(EXIT_FAILURE);
		}

		Get_R *z = sm->ptr<Get_R>(e.offset);
		printf("Message received: UTS = %ld, Type = '%d', Key = '%s'\n", e.uts, int(z->t), z->key);

		sev.sigev_notify = SIGEV_SIGNAL;
		sev.sigev_signo = SIGUSR1;
		sev.sigev_value.sival_ptr = NULL;
		if (mq_notify(mq, &sev) == -1) {
			perror("mq_notify");
			exit(EXIT_FAILURE);
		}
	} else if (signo == SIGINT) {
        std::cout << "\nReceived SIGINT, cleaning up and exiting..." << std::endl;
        
		delete sm;
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        exit(EXIT_SUCCESS);
    }
}

int main() {
	sm = new Shma();

	// clear MQ
	mq_unlink(QUEUE_NAME);

	// signal handler for new MQ entry
	struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

	// [x] TODO: add signal handler for Ctrl+C (avoid abrupt exit)
	// signal handler for Ctrl+C
    struct sigaction sa_int;
    sa_int.sa_handler = signal_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

	// create MQ
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

	// setup MQ notify
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

	// close and delink MQ
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
}