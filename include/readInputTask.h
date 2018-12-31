//
// Created by liavba@wincs.cs.bgu.ac.il on 12/31/18.
//

#ifndef BOOST_ECHO_CLIENT_READINPUT_H
#define BOOST_ECHO_CLIENT_READINPUT_H


#include <mutex>
#include <connectionHandler.h>
#include <queue>

class readInputTask {
    private:

            std::mutex & _mutex;
            bool connected;
             ConnectionHandler& connectionHandler;
             std::queue<std::string>& tasksQueue;

    public:

   readInputTask(std::mutex &_mutex,ConnectionHandler& connectionHandler1,std::queue<std::string>& taskQueue);

    void run();

    void setConnected(bool connected);

};


#endif //BOOST_ECHO_CLIENT_READINPUT_H
