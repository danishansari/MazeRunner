/**
 * Description: Implementation of ServerSocket class
 *
 * Author: Md Danish
 *
 * Date: 2016-06-22
 */

#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include "Socket.h"

/**
 * @brief: Socket Server class
 */
class ServerSocket : private Socket
{
    // socket for communication
    Socket m_socket;

    public:
        // constructor
        ServerSocket(int port=8001);

        // default destructor
        ~ServerSocket();

        // function to accept connection
        bool accept();

        // overloaded fucntion to send data
        int operator << (const std::string msg) const;

        // overloaded fucntion to recv data
        int operator >> (std::string &msg);
};

#endif // SERVER_SOCKET_H
