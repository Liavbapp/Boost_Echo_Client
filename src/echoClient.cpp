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


	//From here we will see the rest of the ehco client implementation:
    while (connected) {


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
                    int typeInt=type[0]-48;
                    std::string postingUser = "";


                    connectionHandler.getFrameAscii(postingUser, '\0');
                    postingUser=postingUser.substr(0,postingUser.length()-1);
                    std::string content = "";
                    connectionHandler.getFrameAscii(content, '\0');
                    content=content.substr(0,content.length()-2);
                    std::string type_s;
                    if(typeInt==0) type_s="PM";
                    else type_s="Public";
                    std::cout<<"NOTIFICATION "<<type_s <<" "<<postingUser<<""<<content<<std::endl;
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
                            keyboardThread.join();
                            break;
                        }
                        case 4: {
                            char numOfUsers[2];
                            connectionHandler.getBytes(numOfUsers, 2);
                            std::string numOfUsersString ="";
                            numOfUsersString += numOfUsers[0];
                            numOfUsersString+=numOfUsers[1];
                            int numOfUsersInt=stoi(numOfUsersString);
                            std::string usernameList = "";

                            for (int i = 0; i < numOfUsersInt; i = i + 1) {
                                std::string name = "";
                                connectionHandler.getFrameAscii(name, '\0');



                                name=name.substr(0,name.length()-1);
                                usernameList += name + " ";
                            }


                            usernameList=usernameList.substr(0,usernameList.length()-1); //cuts last " "
                            optional += std::to_string(numOfUsersInt) + " ";
                            optional+=usernameList;

                            break;
                        }
                        case 7: {
                            char numOfUsers[2];
                            connectionHandler.getBytes(numOfUsers, 2);
                            std::string numOfUsersString ="";
                            numOfUsersString += numOfUsers[0];
                            numOfUsersString+=numOfUsers[1];
                            int numOfUsersInt=stoi(numOfUsersString);
                            std::string userNameList="";
                            userNameList = connectionHandler.getUserNameList(userNameList, numOfUsersInt);
                            optional+=std::to_string(numOfUsersInt)+" ";
                            optional+=userNameList;

                            break;
                        }
                        case 8: {



                            char numPosts[2];
                            connectionHandler.getBytes(numPosts, 2);
                            std::string numOfPosts="";
                            numOfPosts+=numPosts[0];
                            numOfPosts+=numPosts[1];
                            int numPosts_s = stoi(numOfPosts);

                            char numFollowers[2];
                            connectionHandler.getBytes(numFollowers, 2);
                            std::string numOfFollowers="";
                            numOfFollowers+=numFollowers[0];
                            numOfFollowers+=numFollowers[1];
                            int numFollowers_s = stoi(numOfFollowers);

                            char numFollowing[2];
                            connectionHandler.getBytes(numFollowing, 2);
                            std::string numOfFollowing="";
                            numOfFollowing+=numFollowing[0];
                            numOfFollowing+=numFollowing[1];
                            int numFollowing_s = stoi(numOfFollowing);

                            optional+=std::to_string(numPosts_s)+" ";
                            optional+=std::to_string(numFollowers_s)+" ";
                            optional+=std::to_string(numFollowing_s);

                            std::string optioanl="";

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
    //}
    return 0;
}

