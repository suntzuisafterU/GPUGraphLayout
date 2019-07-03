import networkx as nx

def pp(graph_G):
  graph_G.graph['partition'].sort(key=len)
  for p in graph_G.graph['partition']:
    print(p, len(p))
