#include "MazeRunner.h"
#include "terminos.h"

#include <sstream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <stdio.h>

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

    if (num <= MAX_ROW)
    {
        m_mazeRow = num;
        m_mazeCol = num;
       
        printf("initializing game with row = %d\n", m_mazeRow);
        int nVertices = m_mazeRow*m_mazeCol;

        for (int i = 0; i <nVertices; i++)
            for (int j = 0; j < nVertices; j++)
                m_maze[i][j] = -1;

        m_playerInfoVec = playInfo;

        for (int i = 0; i < m_playerInfoVec.size(); i++)
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

bool MazeRunner::isAlive()
{
  return true;
}

bool MazeRunner::isInsideMaze()
{
  return true;
}

void MazeRunner::showMaze()
{
    //system("clear");

    const char heart[] = "\xe2\x99\xa5";

    for (int i = 0; i < m_playerInfoVec.size(); i++)
    {
        cout << "Player # " << i << " pos = " << m_playerInfoVec[i].currPos << endl;
    }

    for (int i = 0; i < 2*m_mazeRow+1; i++)
    {
        for (int j = 0; j < 2*m_mazeCol+1; j++)
        {
            cout << m_maze[i][j] << "(" << (i*(2*m_mazeRow+1))+(j) << ") " ;
        }
        cout << endl;
    }
    cout << "==================================" << endl;

    for (int i = 0; i < 2*m_mazeRow+1; i++)
    {
        for (int j = 0; j < 2*m_mazeCol+1; j++)
        {
            int playerFound = 0;

            //cout << "i = " << i << " j = " << j << " : " << (i*(2*m_mazeRow+1))+(j) << endl;;
            if (m_maze[i][j] == 0)
            {
                for (int m = m_playerInfoVec.size()-1; m > 0; m--)
                {
                    if (((i*(2*m_mazeRow+1))+(j)) == abs(m_playerInfoVec[m].currPos))
                    {
                        if (m_playerInfoVec[m].currPos > 0)
                            printf("%s *%s", COL_g[m+1].c_str(), COL_g[0].c_str());
                        else
                            printf("%s*%s", COL_g[m+1].c_str(), COL_g[0].c_str());

                        playerFound = 1;
                        break;
                    }
                }

                if (!playerFound)
                    printf("  ");
            }
            else
                printf("[]", i*m_mazeRow+j);
        }
        printf("\n");
    }

    //for (int i = 0; i < 2*m_mazeRow+1; i++)
    //{
    //    for (int j = 0; j < 2*m_mazeCol+1; j++)
    //    {
    //        cout << i*(2*m_mazeRow+1)+j <<" "; 
    //    }
    //    cout << endl;
    //}


#if 0
    std::map<int, int>::iterator it = m_spanningTree.begin();
    for (it=m_spanningTree.begin(); it!=m_spanningTree.end(); ++it)
        printf("%d --> %d\n", it->first, it->second);
    
    for (int i = 0; i < m_mazeRow; i++)
    {
        printf ("\n\t(%d) ", (i*m_mazeRow));
        for (int j = 0; j < m_mazeCol-1; j++)
        {
            if ((i*m_mazeRow)+j+1 == m_spanningTree.find(i*m_mazeRow+j)->second)
                printf("----");
            else
                printf("\t");

            printf(" (%d) ", (i*m_mazeRow)+j+1);
        }

        if (i < m_mazeRow-1)
        {
            printf("\n");
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < m_mazeCol-1; j++)
                    printf("\t |");
                printf("\n");
            }
        }
    }
    printf("\n");
    

    //int nVertices = m_mazeRow*m_mazeCol;

    //for (int i = 0; i < nVertices; i++)
    //{
    //    for (int j = 0; j < nVertices; j++)
    //        printf("%d ", m_maze[i][j]);

    //    printf("\n");
    //}
   
    //printf ("spanningTree size = %d\n", m_spanningTree.size());

    //std::map<int, int>::iterator it = m_spanningTree.begin();
    //for (it=m_spanningTree.begin(); it!=m_spanningTree.end(); ++it)
    //    printf("%d --> %d\n", it->first, it->second);
#endif
}

void MazeRunner::displayMaze()
{
    while (isAlive() && isInsideMaze())
    {
        showMaze();

        for (int i = 0; i < m_numClients; i++)
        {
            if (m_socket[i])
            {
                std::string posStr;
                int ret = m_socket[i]->recv(posStr);

                if (ret > 0)
                {
                    int pos = atoi(posStr.c_str());
                    printf("Maze:: Pos = %d\n", pos); 
                    updateMaze(pos, i);
                }
            }
        }
        
        usleep(10000);

        //break;
    }
}

int MazeRunner::updateMaze(int dir, int playerId)
{
  int ret = 0;
  cout << "Update Maze with " << dir << " for " << playerId << " vec size = "<< m_playerInfoVec.size() << endl;

  static int prevDir = dir;

  if (playerId < m_playerInfoVec.size())
  {
    int x = abs(m_playerInfoVec[playerId].currPos)/(m_mazeRow*2);
    int y = abs(m_playerInfoVec[playerId].currPos)%(m_mazeCol*2);
  
    cout << m_playerInfoVec[playerId].currPos << " decompsed = (" << x << ", "<< y << ")" << endl;

    if (abs(dir) % 2)
        x += dir;
    else
        y += (dir/2);

    cout << "compose = (" << x << ", "<< y << ") = " << x*(2*m_mazeRow+1)+y << " d = " << dir * prevDir << endl;

    int newPos = x*(2*m_mazeRow)+y;

    if (y > -1 && newPos > 0 && newPos < (4*m_mazeRow*(m_mazeCol-1)))
    {
        if ((m_playerInfoVec[playerId].currPos < 0 && dir < 0)|| (m_playerInfoVec[playerId].currPos > 0 && 
            (dir*prevDir < 0 || dir > 0)))
        {
            m_playerInfoVec[playerId].currPos = x*(2*m_mazeRow)+y;
            cout << "m_playerInfoVec["<< playerId << "].currPos = " << m_playerInfoVec[playerId].currPos << endl;
            if (dir > 0)
                m_playerInfoVec[playerId].currPos *= -1;

            ret = 1;
        }
        else
            m_playerInfoVec[playerId].currPos *= -1;
    }
    else
        m_playerInfoVec[playerId].currPos *= -1;

    prevDir = dir;
  }
  else
      cout << "Runner:: invalid player request!!" << endl;

  showMaze();

  return ret;
}

void* MazeRunner::displayThread(void *ptr)
{
    ((MazeRunner *) ptr)->displayMaze();
    return NULL;
}

void MazeRunner::generateMaze(int def)
{
    int nVertices = m_mazeRow*m_mazeCol;

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
        int x2 = -1;
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

void MazeRunner::playGame()
{
    generateMaze();

    pthread_create(&m_displayThread, NULL, displayThread, this);

    int dir=0;

    while (isAlive() && isInsideMaze())
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
        cout << "Dir = " << dir << endl;
        updateMaze(dir);

        for (int i = 0; i < m_numClients; i++)
        {
            if (m_socket[i])
            {
                std::stringstream ss;
                ss << dir;
                int ret = m_socket[i]->send(ss.str());
                if (ret <= 0)
                {
                     m_socket[i];
                    m_socket[i] = NULL;
                }
            }
        }

        usleep(10000);
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

    //cout << "minVer = " << minVer << endl;
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

    while (visitedVertices.size() < nVertices)
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
