#include <iostream>
#include <event.h>
#include <pthread.h>
#include<stdexcept>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include"Control.h"
#include"MyPthread.h"
#include"wrap.h"

extern Control* ccontrol_sever_ptr;

// 客户端回调函数：处理客户端消息
void client_cb(int fd, short event, void *arg)
{
    try {
        Pthread *mythis = static_cast<Pthread *>(arg);
        char buff[128] = {0};

        ssize_t n = recv(fd, buff, sizeof(buff) - 1, 0);
        if (n < 0) 
        {
            throw std::runtime_error("recv failed: " + std::string(strerror(errno)));
        }
        if (n == 0) 
        {
            // 客户端断开
            std::cout << "[Info] Client disconnected: fd=" << fd << std::endl;

            // 删除事件
            auto it = mythis->_event_map.find(fd);
            if (it != mythis->_event_map.end()) 
            {
                event_del(it->second);
                event_free(it->second);
                mythis->_event_map.erase(it);
            }

            Close(fd);
            return;
        }

        std::cout << "[Recv] from client(" << fd << "): " << buff << std::endl;

        //返发送给客户端
        ssize_t sent = send(fd, buff, n, 0);
        if (sent < 0) {
            std::cerr << "send error on fd " << fd << std::endl;
        }
        // 调用业务逻辑
        //control_sever.process(fd, std::string(buff));
        std::string msg(buff, n);
        mythis->_control->process(fd, msg);
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "[Exception in client_cb] " << e.what() << std::endl;
    }
}

// socketpair 回调：接收主线程发来的客户端 fd
void sock_pair_1_cb(int fd, short event, void *arg)
{
    try {
        Pthread *mythis = static_cast<Pthread *>(arg);
        char buff[20] = {0};

        if (read(mythis->_sock_fd, buff, sizeof(buff) - 1) <= 0) 
        {
            throw std::runtime_error("read from socketpair failed");
        }

        int client_fd = atoi(buff);
        if (client_fd <= 0) 
        {
            throw std::runtime_error("invalid client fd");
        }

        // 创建 client 事件
        struct event *client_event = event_new(mythis->_base, client_fd, EV_READ | EV_PERSIST, client_cb, arg);
        if (!client_event) 
        {
            throw std::runtime_error("event_new for client failed");
        }

        if (event_add(client_event, nullptr) != 0) 
        {
            event_free(client_event);
            Close(client_fd);
            throw std::runtime_error("event_add for client failed");
        }

        mythis->_event_map[client_fd] = client_event;

        // 回复当前负载
        int num = static_cast<int>(mythis->_event_map.size());
        char abuff[16] = {0};
        sprintf(abuff, "%d", num);
        if (write(mythis->_sock_fd, abuff, sizeof(abuff)) < 0) 
        {
            throw std::runtime_error("write to socketpair failed");
        }

        std::cout << "[Info] Added client fd=" << client_fd << " to thread, total=" << num << std::endl;
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "[Exception in sock_pair_1_cb] " << e.what() << std::endl;
    }
}

void *pthread_run(void *arg)
{
    try {
        Pthread *mythis = static_cast<Pthread *>(arg);

        // 注册 socketpair 事件
        struct event *sock_event = event_new(mythis->_base, mythis->_sock_fd, EV_READ | EV_PERSIST, sock_pair_1_cb, arg);
        if (!sock_event) 
        {
            throw std::runtime_error("Failed to create event for socketpair");
        }

        if (event_add(sock_event, NULL) != 0) 
        {
            event_free(sock_event);
            throw std::runtime_error("Failed to add socketpair event");
        }

        std::cout << "[Thread] Worker thread started, sockpair fd=" << mythis->_sock_fd << std::endl;

        // 进入事件循环
        event_base_dispatch(mythis->_base);

        event_free(sock_event);
    } catch (const std::exception &e) 
    {
        std::cerr << "[Exception in pthread_run] " << e.what() << std::endl;
    }
    return nullptr;
}

Pthread::Pthread(int sock_fd, Control* control)
    : _sock_fd(sock_fd), _control(control)
{
    _base = event_base_new();
    if(!_base)
    {
        throw std::runtime_error("Failed to create event base");
    }

    if (pthread_create(&_pthread, nullptr, pthread_run, this) != 0) {
        throw std::runtime_error("pthread_create failed: " + std::string(strerror(errno)));
    }

    pthread_detach(_pthread);
}

Pthread::~Pthread() 
{
    for (auto &it : _event_map) 
    {
        event_del(it.second);
        event_free(it.second);
    }
    _event_map.clear();

    if (_base)
    {
        event_base_free(_base);
        _base = nullptr;
    }

    Close(_sock_fd);
}