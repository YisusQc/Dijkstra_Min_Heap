#pragma once
#include <vector>
#include <cstdio>
#include <cstdint>
#include <string>
#include <stdexcept>

struct Graph {
  int V = 0;
  int E = 0;
  int start_node = -1;
  int end_node = -1;
  int max_id = 0;

  // Representación Forward Star (CSR) para máxima localidad de caché
  std::vector<int> head;
  std::vector<int> to;
  std::vector<int> weight;
  std::vector<int> next_edge;
  std::vector<uint8_t> vertex_exists;

  void add_edge(int u, int v, int w) {
    to.push_back(v);
    weight.push_back(w);
    next_edge.push_back(head[u]);
    head[u] = static_cast<int>(to.size()) - 1;
  }
};

class FastParser {
  std::vector<char> buffer;
  size_t pos = 0;
  size_t size = 0;

public:
  explicit FastParser(const std::string& filepath) {
    FILE* f = std::fopen(filepath.c_str(), "rb");
    if (!f) throw std::runtime_error("No se pudo abrir el archivo: " + filepath);

    std::fseek(f, 0, SEEK_END);
    size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);

    buffer.resize(size + 1);
    if (std::fread(buffer.data(), 1, size, f) != size) {
      std::fclose(f);
      throw std::runtime_error("Error leyendo el archivo.");
    }
    buffer[size] = '\0';
    std::fclose(f);
  }

  inline bool next_int(int& out) {
    while (pos < size && (buffer[pos] < '0' || buffer[pos] > '9')) {
      if (buffer[pos] == '-') break;
      ++pos;
    }
    if (pos >= size) return false;

    bool neg = false;
    if (buffer[pos] == '-') {
      neg = true;
      ++pos;
    }

    int val = 0;
    while (pos < size && buffer[pos] >= '0' && buffer[pos] <= '9') {
      val = val * 10 + (buffer[pos] - '0');
      ++pos;
    }
    out = neg ? -val : val;
    return true;
  }
};

inline Graph load_graph(const std::string& filepath) {
  FastParser parser(filepath);
  Graph g;

  if (!parser.next_int(g.V) || !parser.next_int(g.E) ||
    !parser.next_int(g.start_node) || !parser.next_int(g.end_node)) {
    throw std::runtime_error("Formato de cabecera invalido.");
  }

  // Lectura temporal de vértices para determinar el ID máximo dinámico
  std::vector<int> declared_vertices;
  declared_vertices.reserve(g.V);
  int max_v_id = std::max(g.V, std::max(g.start_node, g.end_node));

  for (int i = 0; i < g.V; ++i) {
    int id;
    if (!parser.next_int(id)) break;
    declared_vertices.push_back(id);
    if (id > max_v_id) max_v_id = id;
  }

  // Guardamos aristas temporalmente si algún ID en aristas supera max_v_id
  struct RawEdge { int u, v, w; };
  std::vector<RawEdge> raw_edges;
  raw_edges.reserve(g.E);

  for (int i = 0; i < g.E; ++i) {
    int u, v, w;
    if (!parser.next_int(u) || !parser.next_int(v) || !parser.next_int(w)) break;
    raw_edges.push_back({u, v, w});
    if (u > max_v_id) max_v_id = u;
    if (v > max_v_id) max_v_id = v;
  }

  g.max_id = max_v_id;
  g.vertex_exists.assign(max_v_id + 1, 0);
  for (int id : declared_vertices) {
    if (id >= 0) g.vertex_exists[id] = 1;
  }

  g.head.assign(max_v_id + 1, -1);
  g.to.reserve(raw_edges.size());
  g.weight.reserve(raw_edges.size());
  g.next_edge.reserve(raw_edges.size());

  // Grafo dirigido según formato estándar (u -> v con peso w).
  // Si fuera no dirigido, descomentar la segunda llamada.
  for (const auto& e : raw_edges) {
    if (e.u >= 0 && e.v >= 0) {
      g.add_edge(e.u, e.v, e.w);
      // g.add_edge(e.v, e.u, e.w); 
    }
  }

  return g;
}

