//
// Created by liavba@wincs.cs.bgu.ac.il on 12/31/18.
//

#include "readInputTask.h"

readInputTask::readInputTask(std::mutex &_mutex,ConnectionHandler& connHandler,std::queue<std::string>& taskQueue):connectionHandler(connectionHandler), _mutex(_mutex),connected(true),tasksQueue(taskQueue) {}

void readInputTask::run() {

    while (connected) {

        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        int len=line.length();

        std::lock_guard<std::mutex> lock(_mutex); // constructor locks the mutex while
        tasksQueue.push(line);
        std::lock_guard<std::mutex> unlock(_mutex);

        // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
        std::cout << "Sent " << len+1 << " bytes to server" << std::endl;
    }
}

void readInputTask::setConnected(bool connected) {
    readInputTask::connected = connected;
}
