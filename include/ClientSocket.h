/**
 * Description: Implmentation of ClientServer class
 *
 * Author: Md Danish
 *
 * Date: 2016-06-22
 */

#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include "Socket.h"

/**
 * @brief: Wrapper class for ClientSocket
 */
class ClientSocket : private Socket
{
    public:
        // default constructor
        ClientSocket();

        // constructor
        ClientSocket(const std::string host, int port);

        // function to connect to the host
        bool connect(const std::string host="127.0.0.1", int port=8001);

        // overloaded function to send data
        int operator << (const std::string msg) const;
    
        // overloaded function to recv data
        int operator >> (std::string &msg) const;
};

#endif // CLIENT_SOCKET_H
