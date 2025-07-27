#include<iostream>
#include<stdexcept>
#include<arpa/inet.h>
#include<sys/socket.h>
#include"TcpServer.h"
#include"wrap.h"

void listen_cb(int fd, short event, void *arg)
{
    TcpServer *mythis = static_cast<TcpServer*>(arg);
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    
    try
    {
        int cli_fd = Accept(fd, (struct sockaddr*)&cli_addr, &len);
        std::cout << "New client connected: " << inet_ntoa(cli_addr.sin_addr)
            << ":" << ntohs(cli_addr.sin_port) << std::endl;
        
            //查找当前监听数最少的子线程给其分配文件描述符
            int min = 65536; //用于暂存最小监听数
            int tmpfd = 0; //暂存最小监听数对应的文件描述符
            for(auto &kv : mythis->_pth_work_num)
            {
                if(kv.second <= min)
                {
                    min = kv.second;
                    tmpfd = kv.first;
                }
            }

            //将客户端fd从主线程发送到子线程
            char abuff[16]={0};
            //memset(abuff, 0x00, sizeof(abuff));
            sprintf(abuff, "%d", cli_fd);
            if(write(tmpfd, abuff, sizeof(abuff))<0)
            {
                std::cerr << "ERROR: failed to write to sockerpair" << std::endl;
            }
    }catch(const std::runtime_error &e){
        std::cerr << "[TcpServer::run]" << e.what() << std::endl;
    }
}


//主线程的sockpair回调
void sock_pair_cb(int fd, short event, void *arg)
{
    TcpServer *mythis = static_cast<TcpServer*>(arg);
    char buff[16]={0};
    //memset(buff, 0x00, sizeof(buff));
    ssize_t n = read(fd, buff, sizeof(buff)-1);
    if(n > 0)
    {
        try
        {
            int num = std::atoi(buff);
            mythis -> _pth_work_num[fd] = num;
        }
        catch(const std::exception &e)
        {
            std::cerr << "Invalid number from child thread" << buff << std::endl;
        }  
    }
    else
    {
        std::cerr << "Error; failed to read from socketpair (fd=" << fd << ")" << std::endl;
    }
}

TcpServer::TcpServer(const char *ip, short port, int pth_num, Control* control)
    : _pth_num(pth_num), _control(control)
{
    try
    {
        //创建服务器
        _listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = inet_addr(ip);

        int opt = 1;
        setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        Bind(_listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        Listen(_listen_fd, 128);

        //为libevent申请空间
        _base = event_base_new();
        if(!_base)
        {
            throw std::runtime_error("Failed to create event_base");
        }

        //注册监听事件
        struct event* ev = event_new(_base, _listen_fd, EV_READ | EV_PERSIST, listen_cb, this);
        if(!ev)
        {
            throw std::runtime_error("Failed to create event for listen socket");
        }
        event_add(ev, nullptr);

        std::cout << "Server initialized, waiting for connections..." << std::endl;
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "[TcpServer ctor]" << e.what() << std::endl;
        if(_listen_fd > 0) Close(_listen_fd);
        throw;
    }
}

void TcpServer::run()
{
    try
    {
        //申请sockpair
        get_sock_pair();

        //创建线程
        get_pthread();

        //为主线程的sockpair创建事件，绑定回调函数
        for(int i = 0; i < _pth_num; i++)
        {
            struct event *ev = event_new(_base, _socket_pair[i].getReadFd(), EV_READ | EV_PERSIST, sock_pair_cb, this);
            if (!ev) 
            {
                throw std::runtime_error("Failed to create event for socketpair");
            }
            event_add(ev, nullptr);

            event_base_dispatch(_base);
        }
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << "[TcpServer::run]" << e.what() << std::endl;
    }
}

void TcpServer::get_sock_pair()
{
    for(int i = 0; i < _pth_num; i++)
    {
        int pair[2];
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
        {
            throw std::runtime_error("socketpair creation failed: " + std::string(strerror(errno)));
        }

        PipePair pi(pair);
        _socket_pair.push_back(pi); //存到vector中

        _pth_work_num.insert(std::make_pair(pi.getReadFd(),0));
    }
}

void TcpServer::get_pthread()
{
    for(int i = 0; i < _pth_num; i++)
    {
        try
        {
            _pthread.push_back(new Pthread(_socket_pair[i].getWriteFd(), _control));
        }
        catch(const std::exception& e)
        {
            throw std::runtime_error("Failed to create thread object: " + std::string(strerror(errno)));
        }
    }
}