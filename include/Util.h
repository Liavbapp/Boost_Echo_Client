//
// Created by liavba@wincs.cs.bgu.ac.il on 1/1/19.
//

#ifndef BOOST_ECHO_CLIENT_UTIL_H
#define BOOST_ECHO_CLIENT_UTIL_H


#include <string>
#include <vector>

class Util {
public:
    short bytesToShort(const char *bytesArr);

    void shortToBytes(short num, char *bytesArr);

    std::vector<std::string> split(std::string);

    int hashit(std::string);
};

#endif //BOOST_ECHO_CLIENT_UTIL_H
