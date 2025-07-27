#ifndef PUBLIC_H
#define PUBLIC_H

enum MSG_TYPE 
{
    MSG_TYPE_REGISTER    = 1,  // 注册
    MSG_TYPE_LOGIN       = 2,  // 登录
    MSG_TYPE_TALK_TO_ONE = 3,  // 单聊
    MSG_TYPE_TALK_TO_GROUP = 4,// 群聊
    MSG_TYPE_GET_LIST    = 5,  // 获取在线列表
    MSG_TYPE_EXIT        = 6   // 退出
};

#endif
