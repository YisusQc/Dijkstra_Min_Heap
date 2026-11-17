#pragma once
#include "graph.hpp"
#include <queue>
#include <limits>
#include <algorithm>

struct DijkstraResult {
  bool valid_vertices = true;
  bool path_found = false;
  int64_t total_cost = -1;
  std::vector<int> path;
  std::string error_message;
};

inline DijkstraResult run_dijkstra(const Graph& g) {
  DijkstraResult res;

  // Validación de existencia de vértices origen y destino
  bool start_ok = (g.start_node >= 0 && g.start_node <= g.max_id && g.vertex_exists[g.start_node]);
  bool end_ok   = (g.end_node >= 0 && g.end_node <= g.max_id && g.vertex_exists[g.end_node]);

  if (!start_ok || !end_ok) {
    res.valid_vertices = false;
    if (!start_ok && !end_ok) {
      res.error_message = "Error: Ni el vertice origen (" + std::to_string(g.start_node) +
                          ") ni el destino (" + std::to_string(g.end_node) + ") existen en el grafo.";
    } else if (!start_ok) {
        res.error_message = "Error: El vertice origen (" + std::to_string(g.start_node) + ") no existe en el grafo.";
    } else {
        res.error_message = "Error: El vertice destino (" + std::to_string(g.end_node) + ") no existe en el grafo.";
    }
    return res;
  }

  constexpr int64_t INF = std::numeric_limits<int64_t>::max();
  std::vector<int64_t> dist(g.max_id + 1, INF);
  std::vector<int> parent(g.max_id + 1, -1);

  using State = std::pair<int64_t, int>; // {distancia, nodo}
  std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

  dist[g.start_node] = 0;
  pq.emplace(0, g.start_node);

  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();

    // Lazy deletion: descartar entradas obsoletas sin usar set/decrease-key
    if (d > dist[u]) continue;

    // Parada temprana: al extraer el destino del min-heap, su distancia ya es óptima
    if (u == g.end_node) break;

    for (int e = g.head[u]; e != -1; e = g.next_edge[e]) {
      int v = g.to[e];
      int64_t new_dist = d + g.weight[e];

      if (new_dist < dist[v]) {
        dist[v] = new_dist;
        parent[v] = u;
        pq.emplace(new_dist, v);
      }
    }
  }

  if (dist[g.end_node] == INF) {
    res.path_found = false;
    return res;
  }

  res.path_found = true;
  res.total_cost = dist[g.end_node];

  // Reconstrucción de la ruta
  for (int curr = g.end_node; curr != -1; curr = parent[curr]) {
    res.path.push_back(curr);
  }
  std::reverse(res.path.begin(), res.path.end());

  return res;
}

