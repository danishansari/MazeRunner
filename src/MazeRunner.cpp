#include "MazeRunner.h"

#include <sstream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
using namespace std;

MazeRunner::MazeRunner(Socket **socket, int numClients)
{
    m_socket = socket;

    m_numClients = numClients;    
    
    m_mazeRow = -1;
    m_mazeCol = -1;
}

MazeRunner::~MazeRunner()
{ }

bool MazeRunner::initGame(int num)
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
                m_maze[i][j] = 0;
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

int MazeRunner::findMinVertex(int *row, std::vector<int> &visited)
{
    int min = 9999;
    int minVer = -1;
    
    for (int i = 0; i < m_mazeRow*m_mazeCol; i++)
    {
        if (row[i])
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
            //cout << "Edge added = " << currVextex+1 << " -> " << minVertex+1 << endl;
            currVextex = minVertex;
            visitedVertices.push_back(currVextex);
            parent.push_back(currVextex);
            
            backTrace = -1;
        }
        //cout << "visited = ";
        //for (int i = 0; i < visitedVertices.size(); i++)
        //    cout << visitedVertices[i] <<"  " ;
        //cout << endl;
        //usleep(1000000);
    }

    //cout << "Visited Vertices size = " << visitedVertices.size() << endl;
}

void MazeRunner::showMaze()
{
    //system("clear");

    const char heart[] = "\xe2\x99\xa5";

    for (int i = 0; i < 2*m_mazeRow+1; i++)
    {
        for (int j = 0; j < 2*m_mazeCol+1; j++)
        {
            if (m_maze[i][j])
                printf("  ");
            else
                printf("[]", i*m_mazeRow+j);
        }
        printf("\n");
    }


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
                    updateMaze(pos);
                }
            }
        }

        break;
    }
}

int MazeRunner::updateMaze(int pos)
{
  return 0;
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
            //cout << "* (" << i << " " << m_mazeCol << ") = " << m_maze[i][i+m_mazeCol] << endl;
            m_maze[i+m_mazeCol][i] = m_maze[i][i+m_mazeCol];
        }
    }

    primsMinSpanningTree();
    
    for (int i = 0; i < 2*m_mazeRow+1; i++)
    {
        for (int j = 0; j < 2*m_mazeCol+1; j++)
        {
            m_maze[i][j] = 0;
        }
    }
    //cout << "Map size = " << m_spanningTree.size() << endl;
    std::vector< pair<int, int> >::iterator it = m_spanningTree.begin();
    //for (it=m_spanningTree.begin(); it!=m_spanningTree.end(); ++it)
    //    cout << "(" << it->first << " -> " << it->second <<") ";
    //cout << endl;
    //cout << "=================================" << endl;
     
    for (it=m_spanningTree.begin(); it!=m_spanningTree.end(); ++it)
    {
        //std::map<int, int>::iterator it1 = it;
        //for (it1=m_spanningTree.begin(); it1!=m_spanningTree.end(); ++it1)
        //    cout << "(" << it1->first << " -> " << it1->second <<") ";
        //cout << endl;
        //cout << "=================================" << endl;

        int x1 = -1; 
        int y1 = -1;
        int x2 = -1;
        int y2 = -1;        

        int dir = 0;

        //cout << "*path = " << it->first << " " <<  it->second << " row = " << m_mazeRow << endl; 

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
            //cout << "if " << x1 << " " << y1 << " :: " << x2 << " " << y2 << endl;
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
            //cout << "elif " << x1 << " " << y1 << " :: " << x2 << " " << y2 << endl;
        }

        for (int j = y1; j <= y2; j++)
        {
            //cout << "######### " << x1 << " " << j << endl;
            if (dir == 2)
                m_maze[j][x1] = 1;
            else
                m_maze[x1][j] = 1;
        }

        //showMaze();
        //int p;
        //cin >> p;
    }
}

void MazeRunner::playGame()
{
    generateMaze();

    pthread_create(&m_displayThread, NULL, displayThread, this);

    int dir=0;

    while (isAlive() && isInsideMaze())
    {
        //scanf("%d", &dir);
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
                    // m_socket[i];
                    //m_socket[i] = NULL;
                }
            }
        }

        usleep(10000);
        break;
    }

    pthread_join(m_displayThread, NULL);
}
