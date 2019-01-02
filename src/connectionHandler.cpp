#include <connectionHandler.h>
#include <include/Util.h>

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
        case 1: {
            preparedMessage += "01";
            preparedMessage += splitted[0];
            preparedMessage += '\0';
            preparedMessage += splitted[1];
            preparedMessage += '\0';
            break;
        }
        case 2: {
            preparedMessage += "02" + splitted[0] + '\0' + splitted[1];
            break;
        }
        case 3: {
            preparedMessage += "03";
            break;
        }
        case 4: {
            std::string preparedMessage = "04" + splitted[0] + splitted[1] + prepareUserNameList(splitted);
            break;
        }

        case 5: {
            preparedMessage += "05" + splitted[0] + "\0";
            break;
        }
        case 6: {
            preparedMessage += "06" +splitted[0]+'\0'+ splitted[1] + '\0';
            break;
        }
        case 7: {
            preparedMessage += "07";
            break;
        }
        case 8: {
            preparedMessage += "08" + splitted[0] + '\0';
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
        list+=userNameList.at(i)+"\0";
    }
    return list;
}


