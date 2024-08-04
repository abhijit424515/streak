#ifndef COMMON_HH
#define COMMON_HH

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <mqueue.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
using namespace std;

#define MAX_CHAR 4096

struct Entry {
    time_t uts;
	char msg[MAX_CHAR];

    Entry(time_t uts=0, const std::string& msg="") : uts(uts) {
        strncpy(this->msg, msg.c_str(), MAX_CHAR - 1);
        this->msg[MAX_CHAR-1] = '\0';
    }
};

#define QUEUE_NAME "/streak"
#define MSG_MAX_SIZE sizeof(Entry)

#endif