#include <stdlib.h>
#include <connectionHandler.h>
#include <mutex>
#include <queue>
#include <thread>
#include "readInputTask.h"
#include "Util.h"


/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }

    Util util;
    std::mutex _mutex;
    std::queue <std::string> tasksQueue;
    std::string host = argv[1];
    short port = atoi(argv[2]);
    bool connected=false;

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }


    connected=true;
    readInputTask readInputTask1(_mutex,connectionHandler,tasksQueue);
    std::thread keyboardThread(&readInputTask::run, &readInputTask1);
    int len=-1;

    //---------DEBUGGING------------//

    //std::string test="FOLLOW 0 3 YOSII RONEN DIMA";
    //std::string prepared=connectionHandler.prepareMessage(test);

    //^^^^^^^^^DEBUGGING^^^^^^^^^^^^//

	//From here we will see the rest of the ehco client implementation:
    while (connected) {

        if(!tasksQueue.empty()) {
            //std::lock_guard<std::mutex> lock(_mutex); // constructor locks the mutex while
            std::string line=tasksQueue.front();
            tasksQueue.pop();
            //std::lock_guard<std::mutex> unlock(_mutex);

            if (!connectionHandler.sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }

            // We can use one of three options to read data from the server:
            // 1. Read a fixed number of characters
            // 2. Read a line (up to the newline character using the getline() buffered reader
            // 3. Read up to the null character
            std::string answer;
            // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
            // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
            if (!connectionHandler.getLine(answer)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }

           len=answer.length();
            // A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
            // we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
            answer.resize(len-1);
            std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;

            std::string opcode=answer.substr(0,2);

            //processing

            std::cout<<answer<<std::endl;

            /*switch(opcode)
            {
                case "09":

                    char* splitted=answer.c_str();
                    splitted=std::strtok(answer.c_str(),"\0");
                    break;
                case "10":
                    break;
                case "11":
                    break;
            }*/
           std::cout<<answer<<std::endl;
            if (answer == "bye") {
                std::cout << "Exiting...\n" << std::endl;
                break;

            }

        }
    }
    return 0;
}

