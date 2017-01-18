#include <iostream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>

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

int joinedPlayersCount_g = 0;
int maxRow_g = 5;

double waitForConn_g = 10.0;

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

    for (int i = 1; i < argc; i+=2)
    {
        if (string(argv[i]) == "-i")
        {
            ip = argv[i+1];
            connType = 0;
        }
        else if (string(argv[i]) == "-p")
            port = atoi(argv[i+1]);
        else if (string(argv[i]) == "-u")
            username = argv[i+1];
        else if (string(argv[i]) == "-pw")
            gamePassWord_g = argv[i+1];
        else if (string(argv[i]) == "-n")
            maxClient = atoi(argv[i+1]);
        else if (string(argv[i]) == "-r" && username == "superuser")
            maxRow_g = atoi(argv[i+1]);
        else if (string(argv[i]) == "-c")
        {
            connType = 0;
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

    if (!connType)
        maxClient = 1;

    int connected = 1;

    struct timeval gameStartTime;
    gettimeofday(&gameStartTime, NULL);

    //while (connected)
    {
        if (!sockConnect_g)
        {
            sockConnect_g = new Socket*[maxClient];

            for (int i = 0; i < maxClient; i++)
                sockConnect_g[i] = NULL;
        }

        if (connType == 1)
            socket_g = (Socket *) new ServerSocket(port);
        else
            socket_g = (Socket *) new ClientSocket();

        pthread_t accThreads[maxClient];

        struct timeval tv_start, tv_end;

        for (int i = 0; i < maxClient; i++)
        {
            if (connType == 1) // server
            {
                gamePassWord_g = password;

                if (i == 0)
                    cout << "Runner:: Waiting for players.." << endl;
               
                gettimeofday(&tv_start, NULL);

                pthread_create(&accThreads[i], NULL, accThreadsFunc, (void *)&i);
                pthread_join(accThreads[i], NULL);

                gettimeofday(&tv_end, NULL);
                waitForConn_g -= (tv_end.tv_sec - tv_start.tv_sec)+1;
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
                    string playerInfoStr = "$$"+password+"$"+username+"$$"; // add user ip 
                    sockConnect_g[0] = socket_g;

                    int ret = sockConnect_g[0]->send(playerInfoStr);
                    if (ret > 0)
                    {
                        int ret = sockConnect_g[0]->recv(playerInfoStr);
                        cout << "recv = " << playerInfoStr << endl;
                        if (ret > 0 || playerInfoStr.find("connection accepted") != string::npos)
                        {
                            size_t found = playerInfoStr.find(":");
                            if (found != string::npos)
                              maxRow_g = atoi(playerInfoStr.substr(found+1).c_str());
                            
                            found = playerInfoStr.find_last_of(":");
                            //if (found != string::npos)
                            //  joinedPlayersCount_g = atoi(playerInfoStr.substr(found+1).c_str());
      
                            while (playerInfoStr != "done")
                            {
                                sockConnect_g[0]->recv(playerInfoStr);
                                cout << "String recv = " << playerInfoStr << endl; 
            
                                long sec = 0;
                                long usec = 0;

                                size_t found1 = playerInfoStr.find("[");
                                if (found1 != string::npos)
                                {
                                    size_t found2 = playerInfoStr.find(".");
                                    sec = atol(playerInfoStr.substr(found1+1, found2).c_str());

                                    found1 = playerInfoStr.find_last_of("]");
                                    usec = atol(playerInfoStr.substr(found2+1, found1).c_str());
                                    gameStartTime.tv_sec = sec;
                                    gameStartTime.tv_usec = usec;

                                    cout << "# TimeStamp = " << sec << "." << usec << endl;
                                }

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
          string playerInfoStr = "$$"+password+"$"+username+"$$"; // add user ip 
          parserPlayerInfo(playerInfoStr, 1);

          cout << "Final PlayerInfo Size = " << playerInfo_g.size() << endl;
          for (int j = 0; j < (int)playerInfoStrVec_g.size(); j++)
            cout << j << " = " << playerInfoStrVec_g[j] << endl;

          if (connType)
          {    
            for (int i = 0; i < maxClient; i++)
            {
                for (int j = 0; j < (int)playerInfoStrVec_g.size(); j++)
                {
                    if (j != i+1)
                    {
                        if (sockConnect_g[i])
                        {
                            cout << "server: sending[" << i << "] : j = " << j << " : "  << playerInfoStrVec_g[j] << endl;
                            sockConnect_g[i]->send(playerInfoStrVec_g[j]);

                            char timeStr[1024];
                            sprintf(timeStr, "[%ld.%06ld]", gameStartTime.tv_sec, gameStartTime.tv_usec);
                            sockConnect_g[i]->send(timeStr);
                            //usleep(100000);
                        }
                    }
                }
                    
                if (sockConnect_g[i])
                    sockConnect_g[i]->send("done");
            }
          }
            cout << "start game with clients = " << maxClient << endl;
          //MazeRunner runner(sockConnect_g, joinedPlayersCount_g);
          MazeRunner runner(sockConnect_g, maxClient);
          //runner.initGame(5);
          if (playerInfo_g.size() > 0)
          {
              for (int i = 0; i < (int)playerInfo_g.size(); i++)
                  cout << i << " # " << playerInfo_g[i].playerName << endl;
              runner.initGame(maxRow_g, playerInfo_g);
              runner.playGame(gameStartTime);
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

    if (socket_g->accept(sockConnect_g[threadNo], waitForConn_g))
    {
        joinedPlayersCount_g++;

        string playerInfoStr;

        int ret = sockConnect_g[threadNo]->recv(playerInfoStr);

        if (ret > 0 && !playerInfoStr.empty())
        {
            size_t passLen = gamePassWord_g.length();
            if (playerInfoStr.substr(2, passLen) == gamePassWord_g)
            {
                char cmsg[1024];
                sprintf(cmsg, "connection accepted:%d:%d", maxRow_g, joinedPlayersCount_g);
                cout << threadNo << " Player Accepted = " << playerInfoStr << endl;
                int ret = sockConnect_g[threadNo]->send(cmsg);
                if (ret > 0)
                {
                    //parserAndFillUserInfo(playerInfoStr.substr(passLen));
                    cout << "PlyerInfo: " << playerInfoStr << " " << cmsg <<  endl;

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
                int ret = sockConnect_g[threadNo]->send("connection refused: "+gamePassWord_g);
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
  cout << "Init Parsing with : " << playerInfoStr << endl;
    string tmpStr = playerInfoStr;

    size_t foundPos = 0;

    PlayerInfo pInfo;

    while (tmpStr.length() > 0)
    {
        vector<string> parsedStringVec;

        foundPos = tmpStr.find("$$");
        cout << "Start Parse: " << tmpStr << endl;
        if (foundPos != string::npos)
        {
            tmpStr = tmpStr.substr(foundPos+2);
            
            size_t found = tmpStr.find("$$");
            
            if (found != string::npos)
            {
                string compStr = tmpStr.substr(0, found);

                //while (compStr.length() > 0)
                {
                    size_t found1 = compStr.find("$"); 
                    if (found1 != string::npos)
                    {
                        parsedStringVec.push_back(compStr.substr(0, found1));
                        parsedStringVec.push_back(compStr.substr(found1+1));
                        cout << "######## Pushing: " << compStr << endl;
                    }
                }
            }

            tmpStr = tmpStr.substr(found+2);
            cout << "Stop Parse: " << tmpStr << endl;
        }

        if (parsedStringVec.size() > 1)
        {
            pInfo.playerName = parsedStringVec[1];

            playerInfo_g.push_back(pInfo);
            cout << pos << " # pushing: " << playerInfoStr << endl;
            if (pos)
              playerInfoStrVec_g.insert(playerInfoStrVec_g.begin(), playerInfoStr);
            else
              playerInfoStrVec_g.push_back(playerInfoStr);
        }
        else
        {
            cout << "empty string: " << tmpStr << endl;
            tmpStr = "";
        }
    }

    if (pos)
    {
      cout << "$$$$$$$$$$$$$$$$$$$$$$ POS = 1" << endl;
      for (int i = 0; i < pInfo.size(); i++)
      {
        cout << "Player # " << i << " : " << pInfo[i].playerName << endl;
      }
    }
}
