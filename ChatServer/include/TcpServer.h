#ifndef TCPSERVER_H
#define TCPSERVER_H
#include<vector>
#include<memory>
#include<event.h>
#include<map>
#include"Control.h"
#include"MyPthread.h"

class TcpServer
{
    public:
        TcpServer(const char *ip, short port, int pth_num, Control* control);
        ~TcpServer(){};
        void run();
        
        class PipePair
        {
            public:
                PipePair(int fds[2])
                {
                    fd[0] = fds[0];
                    fd[1] = fds[1];    
                }
                int getReadFd() const { return fd[0]; }
                int getWriteFd() const { return fd[1]; }
            private:
                int fd[2];
        };
    
    private:
        int _listen_fd = -1; //监听套接字
        int _pth_num; //启动的线程的个数
        Control* _control;//保存control指针

        struct event_base *_base;
        // std::unique_ptr<event_base,void(*)(event_base *)> _base;
        // libevent事件主循环，使用unique_ptr自动释放
        // std::unique_ptr<event_base, decltype(&event_base_free)> _base;

        
        
        std::vector<PipePair> _socket_pair;
        std::vector<Pthread*> _pthread; //子线程对象
        
        // 子线程监听客户端数量：key = pipe写端fd，value = 当前监听连接数
        std::map<int,int> _pth_work_num;

        void get_sock_pair();
        void get_pthread();

        friend void listen_cb(int fd, short event, void *arg);
        friend void sock_pair_cb(int fd, short event, void *arg);
};

#endif