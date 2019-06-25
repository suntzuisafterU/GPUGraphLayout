import networkx as nx
import sys
import fire # Allows for automatic commandline arg parsing.  Very nice. https://stackoverflow.com/a/16377263/11385910


class EdgeListGenerator(object):
    def random(self, list_comm_sizes, p_in, p_out, partition=False, edges=False):
      G = nx.random_partition_graph(list_comm_sizes, p_in, p_out)
      if partition:
        print(f"# partition of random graph: list_comm_sizes={list_comm_sizes}, p_in={p_in}, p_out={p_out}")
        print_partition(G)
      if edges:
        print(f"# edges of random graph: list_comm_sizes={list_comm_sizes}, p_in={p_in}, p_out={p_out}")
        print_edges(G)
    
    def gaussian_random(self, N, mu, shape_param, p_in, p_out, partition=False, edges=False):
      # variance = mu/shape_param
      G = nx.gaussian_random_partition_graph(N, mu, shape_param, p_in, p_out)
      if partition:
        print(f"# partition of gaussian random graph: N={N}, mu={mu}, shape_param={shape_param}, p_in={p_in}, p_out={p_out}")
        print_partition(G)
      if edges:
        print(f"# edges of gaussian random graph: N={N}, mu={mu}, shape_param={shape_param}, p_in={p_in}, p_out={p_out}")
        print_edges(G)
    
    def karate_club(self):
      G = nx.karate_club_graph()
      print(f"# edges of karate club graph")
      print_edges(G)
    
    def davis_southern_women(self):
      G = nx.davis_southern_women_graph()
      print(f"# davis southern women graph")
      print_edges(G)
    
    def florentine_families(self):
      G = nx.florentine_families_graph()
      print(f"# florentine families graph")
      print_edges(G)
    
    def les_miserables(self):
      G = nx.les_miserables_graph()
      print(f"# les miserables graph")
      print_edges(G)

def print_partition(G):
  for par in G.graph['partition']:
    print(par)

def print_edges(G):
  for u,v in G.edges():
    print(u,v)

if __name__ == '__main__':
    fire.Fire(EdgeListGenerator) # Parse args and run our functions