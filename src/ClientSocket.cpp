
/**
 * Description:
 *
 * Author: Md Danish
 *
 * Date: 
**/

#include "ClientSocket.h"

/**
 * @brief: default constructor
 */
ClientSocket::ClientSocket()
{
    Socket::create();
}

/**
 * @brief: parameterized constructor
 */
ClientSocket::ClientSocket(const std::string host, int port)
{
    Socket::create();
    Socket::connect(host, port);
}

/**
 * @brief: function to connection to running host
 *
 * @return: return true on success and false on faiure
 */
bool ClientSocket::connect(const std::string host, int port)
{
    // connect to host
    return Socket::connect(host, port);
}

/**
 * @brief: function to send data
 *
 * @return: return no of bytes sent on network
 */
int ClientSocket::operator << (const std::string msg) const
{
    return Socket::send(msg);
}

/**
 * @brief: function to recv data
 *
 * @return: return no of bytes recv on network
 */
int ClientSocket::operator >> (std::string &msg) const
{
    return Socket::recv(msg);
}
