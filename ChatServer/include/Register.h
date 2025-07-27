#ifndef REGISTER_H
#define REGISTER_H
#include"View.h"

class Register : public View
{
    public:
        void process(int fd, std::string &json) override;
        void response() override;
    private:
        int _fd;
        std::string _str;
};


#endif