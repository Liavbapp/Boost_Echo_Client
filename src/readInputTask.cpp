//
// Created by liavba@wincs.cs.bgu.ac.il on 12/31/18.
//

#include "readInputTask.h"

readInputTask::readInputTask(std::mutex &_mutex,ConnectionHandler& connHandler,std::queue<std::string>& taskQueue):connectionHandler(connHandler), _mutex(_mutex),connected(true),tasksQueue(taskQueue) {}

void readInputTask::run() {

    while (connected) {

        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        if(line=="LOGOUT")
            connected=false;
        line=connectionHandler.prepareMessage(line)+'\n';
        int len=line.length();

        if (!connectionHandler.sendLine(line)) {
            setConnected(false);
            break;
        }

        //std::lock_guard<std::mutex> lock(_mutex); // constructor locks the mutex while
        //tasksQueue.push(line);
        //std::lock_guard<std::mutex> unlock(_mutex);


        // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
    }
}

void readInputTask::setConnected(bool connected) {
    readInputTask::connected = connected;
}
