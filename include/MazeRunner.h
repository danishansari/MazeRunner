#ifndef MAZE_RUNNER_H
#define MAZE_RUNNER_H

#include "Socket.h"

#include <vector>
#include <map>

#include <pthread.h>

#define MAX_ROW 1000
#define MAX_COL 1000

#define TIMEDIFF_MS(t1, t2) (t1.tv_sec - t2.tv_sec)*1000.0 +\
                            (t1.tv_usec - t2.tv_usec)/1000.0

struct PlayerInfo
{
    std::string playerName;
    std::string ip;
    int currPos;
    int rank;
    int id;
    double currTime;

    PlayerInfo()
    {
        playerName = "unknown";
        ip = "unknown";
        currPos = -1;
        rank = -1;
        id = -1;
    }
};

class MazeRunner
{
    int m_maze[MAX_ROW][MAX_COL];
    int m_showMaze[MAX_ROW][MAX_COL];

    std::vector<PlayerInfo> m_playerInfoVec;

    std::vector< std::pair<int, int> > m_spanningTree;
    
    int m_numClients;
    int m_mazeRow;
    int m_mazeCol;

    Socket **m_socket;

    pthread_t m_displayThread;

    void displayMaze();
    int updateMaze(int p, int dir, int id=0);
    void showMaze();
    void generateMaze(struct timeval tv);

    void primsMinSpanningTree();
    int findMinVertex(int*, std::vector<int>&);

    bool isRunnerAlive(int id=-1);
    bool isRunnerInsideMaze(int id=-1);

    static void* displayThread(void *ptr);

    int getMyPosition(int d, int p);
    public:
        MazeRunner(Socket **socket, int numClients=1);
        ~MazeRunner();

        bool initGame(int num, std::vector<PlayerInfo> &playInfo);
        void playGame(struct timeval tv);
};

#endif // MAZE_RUNNER_H
