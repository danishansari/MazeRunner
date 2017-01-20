#include "MazeRunner.h"
#include "terminos.h"

#include <sstream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <iostream>
using namespace std;

string COL_g[] = { "\x1B[0m", "\x1B[31m", "\x1B[32m", "\x1B[33m", "\x1B[34m", "\x1B[35m", "\x1B[36m", "\x1B[37m" };

MazeRunner::MazeRunner(Socket **socket, int numClients)
{
    m_socket = socket;

    m_numClients = numClients;    
    
    m_mazeRow = -1;
    m_mazeCol = -1;
}

MazeRunner::~MazeRunner()
{ }

bool MazeRunner::initGame(int num, vector<PlayerInfo> &playInfo)
{
    if (!m_socket)
        return false;

    //m_numClients = playInfo.size()-1;

    if (num <= MAX_ROW)
    {
        m_mazeRow = num;
        m_mazeCol = num;
       
        //printf("initializing game with row = %d\n", m_mazeRow);
        int nVertices = m_mazeRow*m_mazeCol;

        for (int i = 0; i <nVertices; i++)
            for (int j = 0; j < nVertices; j++)
                m_maze[i][j] = -1;

        m_playerInfoVec = playInfo;

        for (int i = 0; i < (int)m_playerInfoVec.size(); i++)
        {
            m_playerInfoVec[i].id = i+1;
            m_playerInfoVec[i].currPos = (2*m_mazeRow+1)*(2*m_mazeRow-1)+(2*m_mazeCol);
            cout << "Init:: Player # " << i << " " << m_playerInfoVec[i].playerName << " pos = " << m_playerInfoVec[i].currPos << endl;
        }
    }
    else
        return false;

    return true;
}

bool MazeRunner::isRunnerAlive(int id)
{
  return true;
}

bool MazeRunner::isRunnerInsideMaze(int id)
{
  if (id == -1)
  {
    for (int i = 0; i < (int)m_playerInfoVec.size(); i++)
        if (m_playerInfoVec[i].currPos == 1)
            return false;

    return true;
  }

  return (m_playerInfoVec[id].currPos != 1);
}

void MazeRunner::showMaze()
{
    system("clear");

    //const char heart[] = "\xe2\x99\xa5";

    for (int i = 0; i < (int)m_playerInfoVec.size(); i++)
        printf("> Player # %d %s%s%s\n", i+1, COL_g[i+1].c_str(), 
                        m_playerInfoVec[i].playerName.c_str(), COL_g[0].c_str());

    //for (int i = 0; i < 2*m_mazeRow+1; i++)
    //{
    //    for (int j = 0; j < 2*m_mazeCol+1; j++)
    //    {
    //        cout << m_maze[i][j] << "(" << (i*(2*m_mazeRow+1))+(j) << ") " ;
    //    }
    //    cout << endl;
    //}
    printf("==================================\n");
    int block = 0;
    for (int i = 0; i < 2*m_mazeRow+1; i++)
    {
        for (int j = 0; j < 2*m_mazeCol+1; j++)
        {
            int playerFound = 0;

            //cout << "$$$$ i = " << i << " j = " << j << " : " << (i*(2*m_mazeRow+1))+(j) << endl;;
            if (m_maze[i][j] == 0)
            {
                for (int m = 0; m < (int)m_playerInfoVec.size(); m++)
                {
                    if (playerFound != abs(m_playerInfoVec[m].currPos) && 
                            ((i*(2*m_mazeRow+1))+(j)) == abs(m_playerInfoVec[m].currPos))
                    {
                        //if (abs(m_playerInfoVec[m].currPos) != (((2*m_mazeRow)+1)*((2*m_mazeCol)-1)+2*m_mazeCol) &&
                        //        ((m > 0 && abs(m_playerInfoVec[m].currPos) == abs(m_playerInfoVec[m-1].currPos)) ||
                        //         (m < (int)m_playerInfoVec.size()-1 && abs(m_playerInfoVec[m].currPos)) == abs(m_playerInfoVec[m+1].currPos)))
                        //{
                        //    printf("%s* %s", COL_g[m+1].c_str(), COL_g[0].c_str());
                        //    block = 1;
                        //}
                        //else 
                        if (m_playerInfoVec[m].currPos > 0)
                        {
                            printf("%s *%s", COL_g[m+1].c_str(), COL_g[0].c_str());
                            block = 2;
                        }
                        else
                        {
                            printf("%s* %s", COL_g[m+1].c_str(), COL_g[0].c_str());
                            block = 3;
                        }

                        playerFound = abs(m_playerInfoVec[m].currPos);
                        //break;
                    }
                }

                if (!playerFound)
                    printf("  ");
            }
            else
                printf("[]");
        }
        printf("\n");
    }
}

void MazeRunner::displayMaze()
{
    int countDisconnected = 0;

    showMaze();

    while (isRunnerAlive() && isRunnerInsideMaze())
    {
        for (int i = 0; i < m_numClients; i++)
        {
            if (m_socket[i])
            {
                std::string posStr;
                int ret = m_socket[i]->recv(posStr, 1);
                //cout << ret << " socket[" << i << "] of " << m_numClients << " recv = " << posStr << endl;
                if (ret >= 0)
                {
                    if (ret > 0)
                    {
                        cout << "####### Recieved message = " << posStr << " cli = " << m_numClients<< endl;
                        cout << ret << " socket[" << i << "] of " << m_numClients << " recv = " << posStr << endl;
                        size_t f = posStr.find(":");
                        int pos = atoi(posStr.substr(0, f).c_str());
                        int id = i, dir = 0;
                        posStr = posStr.substr(f+1);
                        
                        f = posStr.find(":");

                        if (f != string::npos)
                            dir =  atoi(posStr.substr(0, f).c_str());
                        
                        posStr = posStr.substr(f+1);

                        if (f != string::npos)
                            id =  atoi(posStr.c_str());

                        if (m_numClients == 1)
                            updateMaze(pos, dir, id);
                        else
                            updateMaze(pos, dir, i+1);

                        for (int n = 0; n < m_numClients; n++)
                        {
                            if (n != i && m_socket[n])
                            {
                                string msg = "";

                                std::stringstream ss1;
                                ss1 << pos << ":" << dir;

                                msg += ss1.str() + ":2";

                                int ret = m_socket[n]->send(msg);

                                //if (ret <= 0)
                                //{
                                //    delete m_socket[i];
                                //    m_socket[i] = NULL;
                                //}

                                usleep(50000/m_numClients);
                            }
                        }

                    }
                }
                else
                {
                    cout << "Socket Disconnected--------------------" << endl;
                    delete m_socket[i];
                    m_socket[i] = NULL;
                }

                countDisconnected = 0;
            }
            else
              countDisconnected ++;
        }
        
        //usleep(20000);

        for (int i = 0; i < (int)m_playerInfoVec.size(); i++)
        {
            if (m_playerInfoVec[i].currPos == 1)
            {
                if (i != 0)
                    printf("Runner:: %s won, better luck next time\n", 
                        m_playerInfoVec[i].playerName.c_str());
                else
                    printf("Runner:: You won!!\n");

                countDisconnected = m_numClients;
               
                // FIXME:: this should be handled properly
                usleep(100000);
                exit(0);
            }
        }

        if (countDisconnected == (int)m_numClients)
        {
          m_numClients = 0;
          break;
        }
    }

    cout << "Recv stopped!!" << endl;
}

int MazeRunner::updateMaze(int pos, int dir, int playerId)
{
  int ret = 0;
  cout << "Update Maze with pos = " << pos << " for " << playerId << " vec size = "<< m_playerInfoVec.size() << endl;

  //static int prevDir = dir;

  if (playerId < (int)m_playerInfoVec.size() || m_numClients == 0)
  {
    //int x = abs(m_playerInfoVec[playerId].currPos)/(m_mazeRow*2+1);
    //int y = abs(m_playerInfoVec[playerId].currPos)%(m_mazeCol*2+1);
  
    int x = abs(pos)/(m_mazeRow*2+1);
    int y = abs(pos)%(m_mazeRow*2+1);

    cout << playerId << ". " << m_playerInfoVec[playerId].currPos << " decompsed = (" << x << ", "<< y << ")" << endl;

    //if (abs(dir) % 2)
    //    x += dir;
    //else
    //    y += (dir/2);
    
    //int newPos = x*(2*m_mazeRow+1)+y;
    int newPos = pos;

    cout << "compose =  " << newPos << " dir = " << dir << endl;;

    if (y > 0 && !m_maze[x][y] && newPos < (((2*m_mazeRow)+1)*((2*m_mazeCol))) && 
            (newPos > 0 || (newPos < 0 && dir == 2)))
    {
        cout << "1st if passed------------------------- " << endl;

        m_playerInfoVec[playerId].currPos = newPos;

        //if ((m_playerInfoVec[playerId].currPos < 0 && dir < 0)|| (m_playerInfoVec[playerId].currPos > 0 && 
        //            (dir*prevDir < 0 || dir > 0 || dir%2)))
        //{
        //    m_playerInfoVec[playerId].currPos = newPos;
        //    cout << "m_playerInfoVec["<< playerId << "].currPos = " << m_playerInfoVec[playerId].currPos << endl;
        //    if (dir == 2)
        //        m_playerInfoVec[playerId].currPos *= -1;

        //    ret = 1;
        //}
        //else
        //    m_playerInfoVec[playerId].currPos *= -1;
    }
    else
    {
        cout << "1st if failed: " << y << " " << newPos << " newpos = " << newPos  << " : " << ((2*m_mazeRow)+1)*((2*m_mazeCol))<< endl;
        m_playerInfoVec[playerId].currPos *= -1;
    }

    //prevDir = dir;
  }
  else
      printf("Runner:: invalid player request!!");

  for (int i = 0; i < m_playerInfoVec.size(); i++)
      cout << "Player # " << i << " currPos = " << m_playerInfoVec[i].currPos << endl;

  showMaze();

  return ret;
}

void* MazeRunner::displayThread(void *ptr)
{
    ((MazeRunner *) ptr)->displayMaze();
    return NULL;
}

void MazeRunner::generateMaze(struct timeval tv)
{
    int nVertices = m_mazeRow*m_mazeCol;

    srand((tv.tv_sec*1000) + (tv.tv_usec/1000));
    for (int i = 0; i < nVertices; i++)
    {
        if (i+1 < (nVertices) && (i+1)%m_mazeRow != 0)
        {
            m_maze[i][i+1] = (rand()%10)+1;
            m_maze[i+1][i] = m_maze[i][i+1];
        }

        if (i+m_mazeCol < (nVertices))
        {
            m_maze[i][i+m_mazeCol] = (rand()%10)+1;
            m_maze[i+m_mazeCol][i] = m_maze[i][i+m_mazeCol];
        }
    }

    primsMinSpanningTree();
    
    for (int i = 0; i < 2*m_mazeRow+1; i++)
    {
        for (int j = 0; j < 2*m_mazeCol+1; j++)
        {
            m_maze[i][j] = -1;
        }
    }
    
    m_maze[0][1] = m_maze[m_mazeRow*2-1][m_mazeCol*2] = 0;

    std::vector< pair<int, int> >::iterator it = m_spanningTree.begin();
    
    for (it=m_spanningTree.begin(); it!=m_spanningTree.end(); ++it)
    {
        int x1 = -1; 
        int y1 = -1;
        //int x2 = -1;
        int y2 = -1;        

        int dir = 0;

        if (it->first < it->second)
        {
            if (abs(it->first - it->second) != 1 && it->first/m_mazeRow < (it->second)/m_mazeRow)
            {
                int x= it->first%m_mazeRow;
                if (x == 0)
                    x = m_mazeRow;

                x1 = 2*x-1;
                y1 = 2*((it->first-1)/m_mazeRow+1)-1;

                y2 = y1+2;

                dir = 2;
            }
            else
            {
                x1 = 2*((it->first-1)/m_mazeRow+1)-1;

                int x= it->first%m_mazeRow;
                if (x == 0)
                    x = m_mazeRow;
                y1 = 2*x-1;

                y2 = y1+2;

                dir = 1;
            }
        }
        else
        {
            if (abs(it->first - it->second) != 1 && it->first/m_mazeRow > it->second/m_mazeRow)
            {
                int x = it->first%m_mazeRow;
                if (x == 0)
                    x = m_mazeRow;

                x1 = 2*(x)-1;
                y1 = 2*((it->first-1)/m_mazeRow)-1;

                y2 = y1+2;
                dir = 2;
            }
            else
            {
                x1 = 2*((it->first-1)/m_mazeRow+1)-1;

                int x = it->second%m_mazeRow;
                if (x == 0)
                    x = m_mazeRow;

                y1 = 2*x-1;

                y2 = y1+2;

                dir = 1;
            }
        }

        for (int j = y1; j <= y2; j++)
        {
            if (dir == 2)
                m_maze[j][x1] = 0;
            else
                m_maze[x1][j] = 0;
        }
    }
}

void MazeRunner::playGame(struct timeval startTime)
{
    generateMaze(startTime);

    pthread_create(&m_displayThread, NULL, displayThread, this);

    int dir=0;

    while (isRunnerAlive() && isRunnerInsideMaze())
    {
      if (getch() == '\033') { // if the first value is esc
        getch(); // skip the [
        switch(getch()) { // the real value
          case 'A':
            // code for arrow up
            dir = -1;
            break;
          case 'B':
            // code for arrow down
            dir = 1;
            break;
          case 'C':
            // code for arrow right
            dir = 2;
            break;
          case 'D':
            // code for arrow left
            dir = -2;
            break;
        }
      }
      
      int newPos = getMyPosition(dir, 0);
      updateMaze(newPos, dir, 0);

      for (int i = 0; i < m_numClients; i++)
      {
        if (m_socket[i])
        {
          string msg = "";
          std::stringstream ss1;
          ss1 << newPos << ":" << dir;

          msg += ss1.str() + ":1";

          int ret = m_socket[i]->send(msg);
          if (ret <= 0)
          {
            delete m_socket[i];
            m_socket[i] = NULL;
          }
        
          usleep(50000/m_numClients);
        }
      }

      // sleep to delay player input - dont remove
      usleep(30000);
      //break;
    }

    pthread_join(m_displayThread, NULL);
}

int MazeRunner::findMinVertex(int *row, std::vector<int> &visited)
{
    int min = 9999;
    int minVer = -1;
    
    for (int i = 0; i < m_mazeRow*m_mazeCol; i++)
    {
        if (row[i] != -1)
        {
            if (row[i] < min && std::find(visited.begin(), visited.end(), i) == visited.end())
            {
                min = row[i];
                minVer = i;
            }
        }
    }

    return minVer;
}

void MazeRunner::primsMinSpanningTree()
{
    int nVertices = m_mazeRow*m_mazeCol;

    std::vector<int> parent;
    std::vector<int> visitedVertices;

    int currVextex = 0;
    int minVertex = currVextex;
    visitedVertices.push_back(currVextex);

    int backTrace = -1; 

    while ((int)visitedVertices.size() < nVertices)
    {
        minVertex = findMinVertex(m_maze[currVextex], visitedVertices);
        
        if (minVertex == -1)
        {
            if (backTrace == -1)
                backTrace = parent.size();

            backTrace --;
            currVextex = visitedVertices[backTrace];
        }
        else
        {
            m_spanningTree.push_back(std::pair<int,int>(currVextex+1, minVertex+1));

            currVextex = minVertex;

            visitedVertices.push_back(currVextex);
            parent.push_back(currVextex);
            
            backTrace = -1;
        }
    }
}

int MazeRunner::getMyPosition(int dir, int playerId)
{
    int x = abs(m_playerInfoVec[playerId].currPos)/(m_mazeRow*2+1);
    int y = abs(m_playerInfoVec[playerId].currPos)%(m_mazeCol*2+1);
  
    cout << "Get Position for: " << playerId << " prevpos = " << m_playerInfoVec[playerId].currPos << " dir = " << dir << endl;

    if (abs(dir) % 2)
        x += dir;
    else
        y += (dir/2);
   
    int newPos = x*(2*m_mazeRow+1)+y;

    if (abs(dir) % 2 == 0 && 
        (m_playerInfoVec[playerId].currPos > 0 && dir < 0) ||
        (m_playerInfoVec[playerId].currPos < 0 && dir > 0))
    {
        newPos =  (m_playerInfoVec[playerId].currPos * -1);
    }
    else if (m_playerInfoVec[playerId].currPos > 0 && dir == 2)
    {
        //m_playerInfoVec[playerId].currPos = newPos;
        newPos *= -1;
    }

    return newPos;
}
