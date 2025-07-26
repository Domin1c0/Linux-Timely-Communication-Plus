#ifndef MYPTHREAD_H
#define MYPTHREAD_H
#include<pthread.h>
#include<map>
#include<event.h>

class Pthread
{
    public:
        Pthread(int sock_fd);
        ~Pthread();

    private:
        int _sock_fd;
        struct event_base* _base;
        std::map<int,struct event*> _event_map;
        pthread_t _pthread;

        friend void sock_pair_1_cb(int fd, short event, void *arg);
        friend void client_cb(int fd, short event, void *arg);
        friend void *pthread_run(void *arg);
};

#endif