import networkx as nx
import sys
import fire # Allows for automatic commandline arg parsing.  Very nice. https://stackoverflow.com/a/16377263/11385910


class EdgeListGenerator(object):
  # # deprecated, not using
  # def random(self, list_comm_sizes, p_in, p_out, partition=False, edges=False):
  #   G = nx.random_partition_graph(list_comm_sizes, p_in, p_out)
  #   header = f"random_graph: list comm sizes={list_comm_sizes}, p_in={p_in}, p_out={p_out}"
  #   if partition:
  #     print(f"# partition {header}")
  #     print_partition(G)
  #   if edges:
  #     print(f"# edges {header}")
  #     print_edges(G)

  def planted_partition(self, num_groups, size_of_each_group, p_in, p_out, partition=False, edges=False, seed=None, directed=False):
    G = nx.planted_partition_graph(num_groups, size_of_each_group, p_in, p_out, seed, directed)
    header = f"planted_N_{num_groups}_size_{size_of_each_group}_p_in{p_in}_p_out{p_out}"
    if partition:
      print(f"# partition {header}")
      print_partition(G)
    if edges:
      print(f"# edges {header}")
      print_edges(G)

  # shape_param of 1 is normal, >1 is flatter distribution, <1 is sharper distribution.
  def gaussian_random(self, N, mu, shape_param, p_in, p_out, partition=False, edges=False):
    # variance = mu/shape_param
    G = nx.gaussian_random_partition_graph(N, mu, shape_param, p_in, p_out)
    header = f"gaussian_random_N{N}_mu{mu}_p_in{p_in}_p_out{p_out}"
    if partition:
      print(f"# partition {header}")
      print_partition(G)
    if edges:
      print(f"# edges {header}")
      print_edges(G)

  def connected_caveman(self, cliques, size_of_each_clique, partition=False, edges=False):
    G = nx.connected_caveman_graph(cliques, size_of_each_clique)
    header = f"connected_caveman_cliques{cliques}_size_of_each_clique{size_of_each_clique}"
    if partition:
      print(f"# partition {header}")
      print_partition(G)
    if edges:
      print(f"# edges {header}")
      print_edges(G)

  def relaxed_caveman(self, cliques, size_of_each_clique, p, partition=False, edges=False):
    G = nx.relaxed_caveman_graph(cliques, size_of_each_clique, p)
    header = f"relaxed_caveman_cliques{cliques}_size_of_each_clique{size_of_each_clique}_p{p}"
    if partition:
      print(f"# partition {header}")
      print_partition(G)
    if edges:
      print(f"# edges {header}")
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

