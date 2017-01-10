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
Socket **sockConnect_g = NULL;

void* accThreadsFunc(void *ptr);

vector<string> playInfoStrVec_g;
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

    //while (connected)
    {
        if (!sockConnect_g)
            sockConnect_g = new Socket*[maxClient];

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
                else
                {
                    string playerInfoStr = "$"+username+"$"+password+"$"; // add user ip 
                    sockConnect_g[0] = socket_g;

                    cout << "Sending Player info = " << playerInfoStr << endl;
                    int ret = sockConnect_g[0]->send(playerInfoStr);
                    if (ret <= 0)
                    {
                        cout << "Could not send data over socket!!" << endl;
                    }
                }
            }
        }

        if (connected)
        {
          MazeRunner runner(sockConnect_g, maxClient);
          //runner.initGame(5);
          runner.initGame(maxRow);
          runner.playGame();
        }

        if (connType == 1) // server
        {
            for (int i = 0; i < maxClient; i++)
                pthread_join(accThreads[i], NULL);
        
            cout << "Runner:: Total Players Joined = " << joinedPlayersCount_g << endl;
            if (joinedPlayersCount_g == 0)
                connected = 0;
        }
    }

    cout << "Runner:: You played nice: " << username << endl;

    return 0;
}

void* accThreadsFunc(void *ptr)
{
    int threadNo = *(int *)ptr;
    cout << "Thread no = " << threadNo << endl;

    sockConnect_g[threadNo] = new Socket;

    if (socket_g->accept(sockConnect_g[threadNo]))
    {
        joinedPlayersStatus_g[threadNo] = 1;
        joinedPlayersCount_g++;
        string playerInfoStr;
        int ret = sockConnect_g[threadNo]->recv(playerInfoStr);
        if (!playerInfoStr.empty())
        {
            playInfoStrVec_g.push_back(playerInfoStr);
            cout << "PlyerInfo: " << playerInfoStr <<  endl;
        }
    }
    
    return NULL;
}
