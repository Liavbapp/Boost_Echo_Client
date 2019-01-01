//
// Created by liavba@wincs.cs.bgu.ac.il on 1/1/19.
//

#include <cstring>
#include "Util.h"

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
    char *cstr = new char[toSplit.length() + 1];
    strcpy(cstr, toSplit.c_str());

    char * pch;
    pch = strtok (cstr," ");
    while (pch != NULL)
    {
        printf ("%s\n",pch);
        pch = strtok (NULL, " ,.-");
    }
    return 0;
}