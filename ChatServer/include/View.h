#ifndef VIEW_H
#define VIEW_H

#include<string>

class View
{
    public:
        virtual ~View() = default;

        virtual void process(int fd, std::string &json) = 0;
        virtual void response() = 0;
};

#endif