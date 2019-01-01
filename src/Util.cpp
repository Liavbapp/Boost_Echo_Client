//
// Created by liavba@wincs.cs.bgu.ac.il on 1/1/19.
//

#include <cstring>
#include "Util.h"

enum string_code {REGISTER=1,LOGIN=2,LOGOUT=3,FOLLOW=4,POST=5,PM=6,USERLIST=7,STAT=8};

short bytesToShort(char* bytesArr)
{
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

void shortToBytes(short num, char* bytesArr)
{
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

std::vector<std::string> Util::split(std::string toSplit) {
    std::vector<std::string> splitted;
    char *cstr = new char[toSplit.length() + 1];
    strcpy(cstr, toSplit.c_str());

    char * pch;
    pch = strtok (cstr," ");
    while (pch != NULL)
    {
        splitted.push_back(pch);
        pch = strtok (NULL, " ");
    }
    return splitted;
}

int Util::hashit(std::string toHash) {
    if(toHash=="REGISTER")
        return REGISTER;
    if(toHash=="LOGIN")
        return LOGIN;
    if(toHash=="LOGOUT")
        return LOGOUT;
    if(toHash=="FOLLOW")
        return FOLLOW;
    if(toHash=="POST")
        return POST;
    if(toHash=="PM")
        return PM;
    if(toHash=="USERLIST")
        return USERLIST;
    if(toHash=="STAT")
        return STAT;
    return -1;
}