#ifndef COMMON_HH
#define COMMON_HH

#include <bits/stdc++.h>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
using namespace std;

#define SHM_ID 73
#define SHM_PATH "/streak_shm"
#define SHM_SIZE 1<<30
#define QUEUE_NAME "/streak"
#define MAX_CHAR 4096

// --------------------------------

enum struct ReqType {
	GET,
	SET,
};

// --------------------------------

struct Request {
	ReqType t;
};

struct Get_R: Request {
	char key[MAX_CHAR];
	
	Get_R(const std::string& msg="") {
        strncpy(this->key, msg.c_str(), MAX_CHAR - 1);
        this->key[MAX_CHAR-1] = '\0';
		t = ReqType::GET;
    }
};

// --------------------------------

struct Entry {
    time_t uts;
	ssize_t offset;
	
	Entry(ssize_t offset=0, time_t uts=0): offset(offset), uts(uts) {}
};

#define MSG_MAX_SIZE sizeof(Entry)

// --------------------------------

#endif