import sys
from os import listdir
from os.path import isfile, join
from collections import deque

class Graph:
 
    # init function to declare class variables
    def __init__(self, V):
        self.V = V
        self.adj = [[] for i in range(V)]

    def iterativeDFS(self, temp, v, visited):

        # create a stack used to do iterative DFS
        stack = deque()

        # push the source node into stack
        stack.append(v)

        # loop till stack is empty
        while stack:
            # Pop a vertex from stack
            v = stack.pop()

            # if the vertex is already discovered yet, ignore it
            if visited[v]:
                continue

            # we will reach here if the popped vertex v
            # is not discovered yet. We print it and process
            # its undiscovered adjacent nodes into stack
            visited[v] = True
            temp.append(v)

            # do for every edge (v -> u)
            adj = self.adj[v]
            for i in reversed(range(len(adj))):
                u = adj[i]
                if not visited[u]:
                    stack.append(u)
        return temp
 
    # method to add an undirected edge
    def addEdge(self, v, w):
        self.adj[v].append(w)
        self.adj[w].append(v)
 
    # Method to retrieve connected components
    # in an undirected graph
    def connectedComponents(self):
        visited = []
        cc = []
        for i in range(self.V):
            visited.append(False)
        for v in range(self.V):
            if visited[v] == False:
            #if visited[v] == False and len(self.adj[v]) > 0:
                temp = []
                cc.append(self.iterativeDFS(temp, v, visited))
        return cc
 
# Driver Code
if __name__ == "__main__":
    sys.setrecursionlimit(10**8)
    #limit = sys.getrecursionlimit() 
    #print(limit)
    #exit(0)

    cluster_ir_dir = sys.argv[1]
    cluster_irs = [f for f in listdir(cluster_ir_dir) if ".ir." in f]

    for cluster_ir in cluster_irs:
        max_cid = 0
        print (cluster_ir)
        with open(join(cluster_ir_dir,cluster_ir)) as fp:
            line = fp.readline()
            while line:
               tokens = line.strip().split()
               for token in tokens[0:-1]:
                   if int(token) > max_cid:
                       max_cid = int(token)
               line = fp.readline()

        g = Graph(max_cid + 1)
        with open(join(cluster_ir_dir,cluster_ir)) as fp:
            line = fp.readline()
            while line:
               tokens = line.strip().split()
               source_cid = int(tokens[0])
               dest_cid = int(tokens[1])
               g.addEdge(source_cid, dest_cid)
               line = fp.readline()

        cc = g.connectedComponents()
        #print("Max. cluster id: ", max_cid)
        #print("Number of graphs: ", len(cc))
        if len(cc) == 1:
            print("PASSED")
        else:
            print("FAILED")
            print("Number of graphs: ", len(cc))
        #print(cc)
        #exit(0)
        print()
