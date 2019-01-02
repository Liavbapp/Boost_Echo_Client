#include <stdlib.h>
#include <connectionHandler.h>
#include <mutex>
#include <queue>
#include <thread>
#include <zconf.h>
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

            char opcode[2];
            connectionHandler.getBytes(opcode,2);
            short opcodeShort=util.bytesToShort(opcode);

            //processing

            switch(opcodeShort)
            {
                case 9: //notification
                {
                    char type[1];
                    type[1] = connectionHandler.getBytes(type, 1);
                    std::string postingUser = "";
                    postingUser = connectionHandler.getFrameAscii(postingUser, '\0');
                    std::string content = "";
                    content = connectionHandler.getFrameAscii(content, '\0');
                    std::string type_s;
                    if(type==0) type_s="PM";
                    else type_s="PUBLIC";
                    std::cout<<"NOTIFICATION "<<type_s <<" "<<postingUser<<" "<<content<<std::endl;
                    break;
                }
                case 10: //ack
                {
                    char msgOpcode[2];
                    connectionHandler.getBytes(msgOpcode, 2);
                    short msgOpcodeShort = util.bytesToShort(msgOpcode);
                    std::string optional = "";

                    switch (msgOpcodeShort) { //resolving the ack's that has optional
                        case 3: //logout-will terminate
                        {
                            connected=false;
                        }
                        case 4: {
                            char numOfUsers[2];
                            connectionHandler.getBytes(numOfUsers, 2);
                            short numOfUsersShort = util.bytesToShort(numOfUsers);
                            std::string usernameList = "";
                            for (int i = 0; i < numOfUsersShort; i = i + 1) {
                                std::string name = "";
                                name = connectionHandler.getFrameAscii(name, '\0');
                                usernameList += name + " ";
                            }
                            optional += std::to_string(msgOpcodeShort) + " " + std::to_string(numOfUsersShort) + " " +
                                        usernameList;
                            break;
                        }
                        case 7: {
                            char numOfUsers[2];
                            connectionHandler.getBytes(numOfUsers, 2);
                            short numOfUsers_s = util.bytesToShort(numOfUsers);
                            std::string userNameList;
                            userNameList = connectionHandler.getUserNameList(userNameList, numOfUsers_s);
                            optional+=numOfUsers_s+" ";
                            optional+=userNameList;
                            break;
                        }
                        case 8: {
                            char numPosts[2];
                            connectionHandler.getBytes(numPosts, 2);
                            short numPosts_s = util.bytesToShort(numPosts);

                            char numFollowers[2];
                            connectionHandler.getBytes(numFollowers, 2);
                            short numFollowers_s = util.bytesToShort(numFollowers);

                            char numFollowing[2];
                            connectionHandler.getBytes(numFollowing, 2);
                            short numFollowing_s = util.bytesToShort(numFollowing);

                            optional+=numPosts_s+" ";
                            optional+=numFollowers_s+" ";
                            optional+=numFollowing_s;

                            break;
                        }
                    }
                    std::cout<<"ACK "<<msgOpcodeShort <<" "<<optional<<std::endl;
                    break;
                }
                case 11: //error
                {
                    char msgOpcode[2];
                    connectionHandler.getBytes(msgOpcode, 2);
                    short msgOpcode_s = util.bytesToShort(msgOpcode);

                    std::cout<<"ERROR "<<msgOpcode_s<< " " <<std::endl;

                    break;
                }
            }

            if (answer == "bye") {
                std::cout << "Exiting...\n" << std::endl;
                break;

            }


            /*  if (!connectionHandler.getBytes(answer,4)) {
                  std::cout << "Disconnected. Exiting...\n" << std::endl;
                  break;
              }

             len=answer.length();
              // A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
              // we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
              answer.resize(len-1);
              std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
            */



        }
    }
    return 0;
}

