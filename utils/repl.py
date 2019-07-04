import networkx as nx

G=nx.gaussian_random_partition_graph(100,3,1,0.25,0.01)

def pp(graph_G):
  graph_G.graph['partition'].sort(key=len)
  for p in graph_G.graph['partition']:
    print(p, len(p))
