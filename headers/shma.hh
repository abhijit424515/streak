#include "common.hh"

// [ ] TODO: create a free list for efficient alloc
struct Shma {
	key_t key;
	int shmid;
    void *shmptr, *head, *current;
	bool is_new = 1;

	Shma(bool fresh=0) {
		if (fresh) {
			if (shmctl(shmid, IPC_RMID, NULL) == -1) {
				perror("shmctl");
			}
		}

		// Ensure the file exists
		int fd = open(SHM_PATH, O_CREAT | O_RDWR, 0666);
		if (fd == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}
		close(fd);
		
		// generate unique key for shared memory
		key = ftok(SHM_PATH, SHM_ID);
		if (key == -1) {
			perror("ftok");
			exit(EXIT_FAILURE);
		}

		// create/reuse shared memory
		shmid = shmget(key, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
        if (shmid == -1) {
            if (errno == EEXIST) {
                // shared memory segment already exists, access it
                shmid = shmget(key, SHM_SIZE, 0666);
                if (shmid == -1) {
                    perror("shmget");
                    exit(EXIT_FAILURE);
                }
                is_new = 0;
            } else {
                perror("shmget");
                exit(EXIT_FAILURE);
            }
        } else is_new = 1;

		// Attach the shared memory to the process's address space
		shmptr = shmat(shmid, NULL, 0);
		if (shmptr == (void *) -1) {
			perror("shmat");
			exit(EXIT_FAILURE);
		}

		head = (char*)shmptr + sizeof(int);

		// if new shm, set size to 0, else retrieve size
		int *s = (int*)shmptr;
		if (is_new) *s = 0;
		current = (char*)head + *s;
	}

	template <typename T, typename... Args>
	T* alloc(Args&&... args) {
		ssize_t size = sizeof(T);
		int *s = (int*)shmptr;
		*s = *s + size;

		void* allocated_mem = current;
		current = (char*)current + size;

		T* obj = new (allocated_mem) T(forward<Args>(args)...);
		return obj;
	}

	int offset(void* x) {
		return (char*)x - (char*)head;
	}

	template <typename T>
	T* ptr(ssize_t offset) {
		void *x = (char*)head + offset;
		return (T*)x;
	}

	~Shma() {
		if (shmdt(shmptr) == -1)
            perror("shmdt");

        if (shmctl(shmid, IPC_RMID, NULL) == -1)
            perror("shmctl");
	}
};