import subprocess
from subprocess import PIPE
from subprocess import Popen

import networkx as nx

# define paths etc.
exe = '../builds/linux/graph_viewer' # current graph_viewer

# make dataset
l = 50
k = 30
p = 0.1
G = nx.relaxed_caveman_graph(l,k,p)
edges = G.edges()
par = None # No partition data for some reason...