#ifndef MAZE_RUNNER_H
#define MAZE_RUNNER_H

#include "Socket.h"

#include <vector>
#include <map>

#include <pthread.h>

#define MAX_ROW 1000
#define MAX_COL 1000

class MazeRunner
{
    int m_maze[MAX_ROW][MAX_COL];
    int m_showMaze[MAX_ROW][MAX_COL];

    std::vector< std::pair<int, int> > m_spanningTree;
    
    int m_numClients;
    int m_mazeRow;
    int m_mazeCol;

    Socket **m_socket;

    pthread_t m_displayThread;

    void displayMaze();
    int updateMaze(int p);
    void showMaze();
    void generateMaze(int def=0);

    void primsMinSpanningTree();
    int findMinVertex(int*, std::vector<int>&);

    bool isAlive();
    bool isInsideMaze();

    static void* displayThread(void *ptr);

    public:
        MazeRunner(Socket **socket, int numClients=1);
        ~MazeRunner();

        bool initGame(int num);
        void playGame();
};

#endif // MAZE_RUNNER_H
