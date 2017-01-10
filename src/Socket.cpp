/**
 * Description: Socket class implementation
 *
 * Author: Md Danish
 *
 * Date: 2016-06-22 
 */

#include "Socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CONN 10
#define MAX_RECV 1024

/**
 * @brief: default constructor
 */
Socket::Socket() : m_sockFD (-1)
{
    // reste socket address structure 
    memset(&m_addr, 0, sizeof(m_addr));
}

/**
 * @brief: defualt destructor
 */
Socket::~Socket()
{ 
    // close running connection
    ::close (m_sockFD);
}

/**
 * @brief: function to create socket
 *
 * @return: return true on success and false on failure
 */
bool Socket::create()
{
    // initialize socket
    m_sockFD = socket(AF_INET, SOCK_STREAM, 0);
   
    // check if socket initialize was success 
    if (m_sockFD < 0)
    {
        fprintf(stderr, "\x1b[32m" "Socket:: could not create socekt!!\n" "\x1b[0m");
        return false; // return failure
    }

    struct timeval timeout;      
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    //if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
    //            sizeof(timeout)) < 0)
    //    error("setsockopt failed\n");

    // set socket option to reuse socket for reconnection
    int on = 1;
    if (setsockopt(m_sockFD, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) == -1)
    {
        fprintf(stderr, "\x1b[32m" "Socket:: could not set socket options\n" "\x1b[0m");
    }

    return true; // return success
}

/**
 * @brief: function to bind socket to the port
 *
 * @return: return true on success and false on failure
 */
bool Socket::bind(const int port)
{
    /* setup the host_addr structure for use in bind call */
    // server byte order
    m_addr.sin_family = AF_INET;

    // automatically be filled with current host's IP address
    m_addr.sin_addr.s_addr = INADDR_ANY;

    // convert integer port into network bytes
    m_addr.sin_port = htons(port);

    // bind port to the socket
    if (::bind(m_sockFD, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0)
    {
        fprintf(stderr, "\x1b[32m" "Socket:: could not bind socekt!!\n" "\x1b[0m");
        return false; // return failure
    }

    return true; // return success
}

/**
 * @brief: function to listen to the incoming connection
 *
 * @return: return true on success and false on failure
 */
bool Socket::listen() const
{
    // listen for incoming connection
    if (::listen(m_sockFD, MAX_CONN) < 0)
    {
        fprintf(stderr, "\x1b[32m" "Socket:: could not listen socekt!!\n" "\x1b[0m");
        return false; // return failure
    }

    return true; // return success
}

/**
 * @brief: function to accept incoming connection
 *
 * @return: return true on success and false on failure
 */
bool Socket::accept(Socket *newSock) const
{
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(m_sockFD, &fd);

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    if (select(m_sockFD+1, &fd, NULL, NULL, &tv) > 0)
    {
        // length of client address
        socklen_t len = sizeof(*newSock);

        // accept incoming request
        newSock->m_sockFD = ::accept(m_sockFD, (sockaddr *) &m_addr, &len);

        // set host address
        newSock->m_addr = m_addr;

        // check if connection was accepted
        if (newSock->m_sockFD < 0)
        {
            fprintf(stderr, "\x1b[32m" "Socket:: could not accept socket: %d\n" "\x1b[0m",
                    newSock->m_sockFD);
            return false; // return failure
        }
        else
            return true; // return success
    }
}

/**
 * @brief: function to connect to the running host
 *
 * @return: return true on success and false on failure
 */
bool Socket::connect(const std::string host, int port)
{
    // server byte order
    m_addr.sin_family = AF_INET;

    // convert integer port into network bytes
    m_addr.sin_port = htons(port);

    // initialize options
    int status = inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr);
    if (status <= 0)
    {
        return false; // return failure
    }

    // connect to the running server
    status = ::connect(m_sockFD, (sockaddr *) &m_addr, sizeof (m_addr) );
    if (status == 0)
        return true; // return success
    else
        return false; // return failure
}

/**
 * @brief: function to disconnect socket
 *
 * @return: return true on success and false on failure
 */
bool Socket::disConnect(Socket &newSock)
{
    if (EBADF != ::shutdown(newSock.m_sockFD, SHUT_RDWR))
    {
        ::close (newSock.m_sockFD);
    
        return true; // return sucess
    }

    return false; // return failure
}

/**
 * @brief: function to send data to network
 *
 * @return: return no of bytes sent successfuly
 */
int Socket::send(std::string msg) const
{
    // send data to the network
    int status = ::send(m_sockFD, msg.c_str(), msg.size(), MSG_NOSIGNAL);
    return status;
}

/**
 * @brief: function to recieve data from network
 *
 * @return: return no of bytes sent successfuly
 */
int Socket::recv(std::string &msg) const
{
    // temp buffer to recieve data
    char tmpMsg[MAX_RECV];
    memset(tmpMsg, '\0', MAX_RECV);

    // recieve data from network
    int status = ::recv(m_sockFD, tmpMsg, MAX_RECV, 0);
   
    // set recieved message 
    msg = tmpMsg;

    return status;
}
