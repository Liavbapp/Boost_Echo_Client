#include <connectionHandler.h>
#include <include/Util.h>
#include <sys/ioctl.h>
using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;


ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){}
    
ConnectionHandler::~ConnectionHandler() {
    close();
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}


bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
		do{
			getBytes(&ch, 1);
            frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

std::string ConnectionHandler::getUserNameList(std::string userList,short numOfUsers) {
    std::string toReturn="";
    for(int i=0;i<numOfUsers;i++) {
        std::string currentUserInList="";
        getFrameAscii(currentUserInList,'\0');
        currentUserInList.resize(currentUserInList.length()-1);
        currentUserInList=currentUserInList+" ";
        toReturn+=currentUserInList;
    }
    toReturn.resize(toReturn.length()-1);
    return toReturn;
}
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
	bool result=sendBytes(frame.c_str(),frame.length());
	if(!result) return false;
	return sendBytes(&delimiter,1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}

std::string ConnectionHandler::prepareMessage(std::string userInput) {

    Util util;
    std::string currentMessage=userInput;
    std::vector<std::string> splitted;
    std::string type;
    std::string preparedMessage;
    char bytesArr [2];
    if(currentMessage.find(" ")!=string::npos) {
        type = currentMessage.substr(0, currentMessage.find(" "));
        currentMessage = currentMessage.substr(type.length() + 1, currentMessage.length() - type.length());

        splitted = util.split(currentMessage);
    }
    else
        type=currentMessage;

    int hashedType=util.hashit(type);

    switch (hashedType)
    {
        case 1: { //register
            util.shortToBytes(1,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            preparedMessage += splitted[0];
            preparedMessage += '\0';
            preparedMessage += splitted[1];
            preparedMessage += '\0';
            break;
        }
        case 2: { //login
            util.shortToBytes(2,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            preparedMessage+=splitted[0] + '\0';
            preparedMessage+=splitted[1]+'\0';
            break;
        }
        case 3: { //logout
            util.shortToBytes(3,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            break;
        }
        case 4: { //follow
            util.shortToBytes(4,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            if(splitted[0]=="0")
            preparedMessage+='\0';
            else
                preparedMessage+='\1';
            char* bytesArr=new char[2];
            bytesArr[0] = ((stoi(splitted[1]) >> 8) & 0xFF);
            bytesArr[1] = ((stoi(splitted[1]) & 0xFF));
            preparedMessage+=bytesArr[0];
            preparedMessage+=bytesArr[1];
            preparedMessage+=prepareUserNameList(splitted)+'\0';
            break;
        }

        case 5: { //post
            util.shortToBytes(5,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            std::string content="";
            for(int i=0;i<splitted.size();i++)
                content+=splitted[i]+" ";
            preparedMessage+=content + '\0';
            break;
        }
        case 6: { //pm
            util.shortToBytes(6,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            preparedMessage+=splitted[0]+'\0';
            std::string content="";
            for(int i=1;i<splitted.size();i++)
                content+=splitted[i]+" ";

            preparedMessage+=content + '\0';
            break;
        }
        case 7: { //userlist
            util.shortToBytes(7,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            break;
        }
        case 8: { //stat
            util.shortToBytes(8,bytesArr);
            preparedMessage += bytesArr[0];
            preparedMessage += bytesArr[1];
            preparedMessage+=splitted[0] + '\0';
            break;
        }
        default:
            break;

    }
    return preparedMessage;
}


std::string ConnectionHandler:: prepareUserNameList(std::vector<std::string> userNameList){

    std::string list="";
    for(int i=2;i<userNameList.size();i=i+1){
        list+=userNameList.at(i)+'\0';
    }
    return list;
}

std::string ConnectionHandler::concatenateNames(std::vector<std::string> strings) {
    std::string toReturn="";
    for(int i=0;i<strings.size();i++)
        toReturn+=strings[i]+" ";
    return toReturn.substr(0,toReturn.size()-1);
}





