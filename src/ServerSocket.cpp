
/**
 * Description:
 *
 * Author: Md Danish
 *
 * Date: 
**/

#include "ServerSocket.h"

#include <stdio.h>

/**
 * @brief: server socket constructor
 */
ServerSocket::ServerSocket(int port)
{
    Socket::create();
    Socket::bind(port);
    Socket::listen();
}

/**
 * @brief: server socket destructor
 */
ServerSocket::~ServerSocket()
{
    // close local socket connection
    m_socket.disConnect(m_socket);
}

/**
 * @brief: function to accept connect
 *
 * @return: return true on succes and false on failure
 */
bool ServerSocket::accept()
{
    // accept connection
    return Socket::accept(m_socket);
}

/**
 * @brief: function to send data
 *
 * @return: return no of bytes sent
 */
int ServerSocket::operator << (const std::string msg) const
{
    // send data
    return m_socket.send(msg);
}

/**
 * @brief: function to recv data
 *
 * @return: return no of bytes recv
 */
int ServerSocket::operator >> (std::string &msg)
{
    // recv data
    return m_socket.recv(msg);
}
