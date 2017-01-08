import sys
from random import randint

def minVertex(mat, visited):
  min = float('inf')
  minV = -1;
  for i in range(len(mat)):
    if mat[i] < min and i not in visited:
      min = mat[i]
      minV = i

  return minV

def spanningTreeUsingPrims(adjMat, row, col):
  
  nVertices = row*col;

  parent = []
  visitedVertices = []
    
  currVertex = 0

  minV =  currVertex 
  visitedVertices.append(currVertex)

  backT = -1
 
  spanningTree = []

  while len(visitedVertices) < nVertices:

    minV = minVertex(adjMat[currVertex], visitedVertices)

    if minV == -1:
      debug = 1;

    if minV == -1:
      currVertex = parent[backT]
      backT -= 1
    else: 
      spanningTree.append((currVertex, minV))
      currVertex = minV
      visitedVertices.append(currVertex)
      parent.append(currVertex);
      backT = -1

  print "Visited = ", visitedVertices, "\n"
  print "Spaning Tree = ", spanningTree, "\n"
  return spanningTree

def generateGraph(row, col):
  matx = [[float('inf') for x in range(row*col)] for y in range(row*col)] 
  
  for i in range(row*col):
    if (i+1 < (row*col) and (i+1)%row != 0):
      matx[i][i+1] = randint(1, 9)
      matx[i+1][i] = matx[i][i+1]

    if (i+col < (row*col)):
      matx[i][i+col] = randint(1, 9)
      matx[i+col][i] = matx[i][i+col]

  #i = 0;
  #for r in matx:
  #  print i, " ", r
  #  i+=1
  #print "=========================================="
  return matx

def displayTree1(spanTree, adjMat, row, col):
  mat = [[(x+y*row) for x in range(row)] for y in range(col)] 

  for i in range(len(mat)): 
    for j in range(len(mat[0])): 
      print "[%d]" % mat[i][j],
      if j+(i*row)+1 < row*col:
        v = adjMat[j+(i*row)][j+(i*row)+1]
        if (adjMat[j+(i*row)][j+(i*row)+1] == float('inf')):
              v = 0
        if  j < row-1:
            print "----(%d)----" % v,
   
    for j in range(5): 
      if  i < row-1:
        print ""
        if j == 2:
          for k in range(len(mat[0])): 
            #print"(%d, %d)" % (k+(i+1)*row, i*row+k),
            v = adjMat[k+(i+1)*row][i*row+k];
            if (adjMat[k+(i+1)*row][i*row+k] == float('inf')):
              v = 0
            print "(%d)\t\t" % v,
        else:
          for k in range(len(mat[0])): 
            print " |\t\t",
    
    print ""


def displayTree(spanTree, adjMat, row, col):
  mat = [[(x+y*row) for x in range(row)] for y in range(col)] 

  for i in range(len(mat)): 
    for j in range(len(mat[0])): 
      print "[%d]" % mat[i][j],
      if j+(i*row)+1 < row*col:
        v = adjMat[j+(i*row)][j+(i*row)+1]
        if (adjMat[j+(i*row)][j+(i*row)+1] == float('inf')):
              v = 0
        if  j < row-1:
            print "----(%d)----" % v,
   
    for j in range(5): 
      if  i < row-1:
        print ""
        if j == 2:
          for k in range(len(mat[0])): 
            #print"(%d, %d)" % (k+(i+1)*row, i*row+k),
            v = adjMat[k+(i+1)*row][i*row+k];
            if (adjMat[k+(i+1)*row][i*row+k] == float('inf')):
              v = 0
            print "(%d)\t\t" % v,
        else:
          for k in range(len(mat[0])): 
            print " |\t\t",
    
    print ""


def main():
  row = 5

  if len(sys.argv) > 1:
    row = int(str(sys.argv[1]))
   
  col = row

  adjMat = generateGraph(row, col)

  for a in adjMat:
      print a

  spanningTree = spanningTreeUsingPrims(adjMat, row, col);

  #displayTree(spanningTree, adjMat, row, col)

if __name__=="__main__":
  main()
