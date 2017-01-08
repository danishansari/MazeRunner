/**
 * Description: Socket header file
 *
 * Author: Md Danish
 *
 * Date: 2016-06-22
 */


#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

/**
 * @brief: Socket wrapper class for all socket feature.
 */
class Socket
{
    // socket file descriptor
    int m_sockFD;

    // socket addres structure
    sockaddr_in m_addr;

    public:
        // default construtor
        Socket();

        // default destrutor
        ~Socket();

        // function to create socket
        bool create();

        // function to bind socket to the port
        bool bind(const int port);

        // function to listen on the port
        bool listen() const;

        // function to accept incoming connection
        bool accept(Socket &socket) const;

        // fucntion to connect to the running socket(server)
        bool connect(const std::string host, int port);

        // function to disconnect from the connected socket
        bool disConnect(Socket &socket);

        // function to send data
        int send(const std::string msg) const;

        // function to recieve data
        int recv(std::string &msg) const;

};

#endif // SOCKET_H
