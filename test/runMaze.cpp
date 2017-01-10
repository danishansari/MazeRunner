#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <pthread.h>

#include "Socket.h"
#include "ServerSocket.h"
#include "ClientSocket.h"

#include "MazeRunner.h"

pthread_mutex_t acceptLock_g = PTHREAD_MUTEX_INITIALIZER;

using namespace std;

Socket *socket_g = NULL;
Socket **sockConnect_g = NULL;

void* accThreadsFunc(void *ptr);
void parserPlayerInfo(string playerInfoStr, int pos=0);

int* joinedPlayersStatus_g = NULL;
int joinedPlayersCount_g = 0;

vector<PlayerInfo> playerInfo_g;
vector<string> playerInfoStrVec_g;
string gamePassWord_g = "admin";


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
            gamePassWord_g = argv[i+1];
        else if (string(argv[i]) == "-n")
            maxClient = atoi(argv[i+1]);
        else if (string(argv[i]) == "-r")
            maxRow = atoi(argv[i+1]);
        else if (string(argv[i]) == "-c")
        {
            connType = 0;
            maxClient = 1;
            i--;
        }
        else
            cout << "Runner:: Invalid param: " << argv[i] << endl;
    }
    
    if (username == "unknown")
    {
        cout << "Provide User Name" << endl;
        return -1;
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
                gamePassWord_g = password;
                if (i == 0)
                    cout << "Runner:: Waiting for players.." << endl;
                
                pthread_create(&accThreads[i], NULL, accThreadsFunc, (void *)&i);
                pthread_join(accThreads[i], NULL);
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
                    string playerInfoStr = "$"+password+"$"+username+"$"; // add user ip 
                    sockConnect_g[0] = socket_g;

                    int ret = sockConnect_g[0]->send(playerInfoStr);
                    if (ret > 0)
                    {
                        int ret = sockConnect_g[0]->recv(playerInfoStr);
                        cout << "recv = " << playerInfoStr << endl;
                        if (ret > 0 || playerInfoStr == "connection accepted")
                        {
                            while (playerInfoStr != "done")
                            {
                                sockConnect_g[0]->recv(playerInfoStr);
                                cout << "String recv = " << playerInfoStr << endl; 
                                if (playerInfoStr.length() > 0)
                                    parserPlayerInfo(playerInfoStr);

                                size_t f = playerInfoStr.find("done");
                                if (f != string::npos)
                                    playerInfoStr = "done";
                            }
                        }
                        else
                        {
                           cout << "Runner:: Wrong Password!!" << endl;
                           connected = 0;
                        }
                    }
                    else
                    {
                        cout << "Runner:: Could not send data over socket!!" << endl;
                    }
                }
            }
        }

        if (connected)
        {
          cout << "PlayerInfo Size = " << playerInfo_g.size() << endl;
          string playerInfoStr = "$"+password+"$"+username+"$"; // add user ip 
          parserPlayerInfo(playerInfoStr, 0);

          if (connType)
          {    
            for (int i = 0; i < maxClient; i++)
            {
                for (int j = 0; j < playerInfoStrVec_g.size(); j++)
                {

                    if (i != j)
                    {
                        if (sockConnect_g[i])
                        {
                            cout << "server: sending : " << playerInfoStrVec_g[i] << endl;
                            sockConnect_g[i]->send(playerInfoStrVec_g[i]);
                            usleep(100000);
                        }
                    }
                }
                    
                if (sockConnect_g[i])
                    sockConnect_g[i]->send("done");
            }
          }

          MazeRunner runner(sockConnect_g, maxClient);
          //runner.initGame(5);
          if (playerInfo_g.size() > 0)
          {
              runner.initGame(maxRow, playerInfo_g);
              runner.playGame();
          }
          else cout << "Unexpected error!!" << endl;
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
            size_t passLen = gamePassWord_g.length();
            if (playerInfoStr.substr(1, passLen) == gamePassWord_g)
            {
                int ret = sockConnect_g[threadNo]->send("connection accepted");
                if (ret > 0)
                {
                    //parserAndFillUserInfo(playerInfoStr.substr(passLen));
                    cout << "PlyerInfo: " << playerInfoStr <<  endl;

                    parserPlayerInfo(playerInfoStr);
                }
                else
                {
                    delete sockConnect_g[threadNo];
                    sockConnect_g[threadNo] = NULL;
                }
            }
            else
            {
                int ret = sockConnect_g[threadNo]->send("connection refused");
                if (ret < 0)
                    cout << "Runner:: connection refused!!" <<  endl;

                delete sockConnect_g[threadNo];
                sockConnect_g[threadNo] = NULL;
            }
        }
    }
    
    return NULL;
}

void parserPlayerInfo(string playerInfoStr, int pos)
{
    string tmpStr = playerInfoStr;

    size_t foundPos = 0;

    PlayerInfo pInfo;

    vector<string> parsedStringVec;

    while(tmpStr.length() > 0 && 
        (foundPos = tmpStr.find_last_of("$")) != string::npos)
    {
        tmpStr.erase(foundPos);

        foundPos = tmpStr.find_last_of("$");

        if (foundPos != string::npos)
        {
            parsedStringVec.push_back(tmpStr.substr(foundPos+1));
            tmpStr = tmpStr.substr(0, foundPos+1);
        }
    }

    if (parsedStringVec.size() > 1)
    {
        pInfo.playerName = parsedStringVec[1];

        playerInfo_g.push_back(pInfo);
        cout << "pushing: " << playerInfoStr << endl;
        if (pos)
            playerInfoStrVec_g.insert(playerInfoStrVec_g.begin(), playerInfoStr);
        else
            playerInfoStrVec_g.push_back(playerInfoStr);
    }
    else
        cout << "empty string!!" << endl;
}
