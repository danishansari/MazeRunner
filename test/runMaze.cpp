#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>

#include "Socket.h"
#include "ServerSocket.h"
#include "ClientSocket.h"

#include "MazeRunner.h"

using namespace std;

Socket *socket_g = NULL;
Socket *sockConnect_g = NULL;

void* accThreadsFunc(void *ptr);

int* joinedPlayersStatus_g = NULL;
int joinedPlayersCount_g = 0;

int main(int argc, char**argv)
{
    string ip = "127.0.0.1";
    int port = 8000;
    string username = "unknown";
    string password = "admin";
    int maxClient = 3;
    int connType = 1;
    int maxRow = 5;

    for (int i = 1; i < argc; i+=2)
    {
        if (string(argv[i]) == "-i")
        {
            ip = argv[i+1];
            connType = 0;
            maxClient = 1;
        }
        else if (string(argv[i]) == "-p")
            port = atoi(argv[i+1]);
        else if (string(argv[i]) == "-u")
            username = argv[i+1];
        else if (string(argv[i]) == "-pw")
            password = argv[i+1];
        else if (string(argv[i]) == "-n")
            maxClient = atoi(argv[i+1]);
        else if (string(argv[i]) == "-r")
            maxRow = atoi(argv[i+1]);
        else if (string(argv[i]) == "-c")
        {
            connType = 0;
            maxClient = 1;
        }
        else
            cout << "Runner:: Invalid param: " << argv[i] << endl;
    }

    joinedPlayersStatus_g = new int[maxClient];

    int connected = 1;

    while (connected)
    {
        if (connType == 1)
            socket_g = (Socket *) new ServerSocket(port);
        else
            socket_g = (Socket *) new ClientSocket();

        pthread_t accThreads[maxClient];

        for (int i = 0; i < maxClient; i++)
        {
            if (connType == 1) // server
            {
                if (i == 0)
                    cout << "Runner:: Waiting for players.." << endl;

                if (!sockConnect_g)
                    sockConnect_g = new Socket[maxClient];
                    
                pthread_create(&accThreads[i], NULL, accThreadsFunc, (void *)&i);
                //accThreadsFunc((void*)&i);
                //usleep(2000000);
            }
            else // client
            {
                cout << "Runner:: trying to connect.." << endl;

                if (!socket_g->connect(ip, port))
                {
                    cout << "Runner:: could not connect to: " << ip << endl;
                    connected = 0;
                }
            }
        }

        if (connType == 1) // server
        {
            for (int i = 0; i < maxClient; i++)
                pthread_join(accThreads[i], NULL);
        
            cout << "Runner:: Total Players Joined = " << joinedPlayersCount_g << endl;
            if (joinedPlayersCount_g == 0)
                connected = 0;
        }
        else
        {
            joinedPlayersCount_g = maxClient;
            sockConnect_g = socket_g;
        }

        if (connected)
        {
          MazeRunner runner(sockConnect_g, maxClient);
          //runner.initGame(5);
          runner.initGame(maxRow);
          runner.playGame();
        }

        break;
    }

    cout << "Runner:: You played nice: " << username << endl;

    return 0;
}

void* accThreadsFunc(void *ptr)
{
    int threadNo = *(int *)ptr;

    if (socket_g->accept(*sockConnect_g))
    {
        joinedPlayersStatus_g[threadNo] = 1;
        joinedPlayersCount_g++;
    }
    
    return NULL;
}
