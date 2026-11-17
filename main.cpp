#include "graph.hpp"
#include "dijkstra.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <ruta_archivo_grafo>\n";
    return 1;
  }

  std::string filepath = argv[1];

  try {
    // 1. Medición de carga del grafo
    auto t0_load = std::chrono::steady_clock::now();
    Graph g = load_graph(filepath);
    auto t1_load = std::chrono::steady_clock::now();

    double load_ms = std::chrono::duration<double, std::milli>(t1_load - t0_load).count();

    std::cout << "========================================\n";
    std::cout << "Archivo: " << filepath << "\n";
    std::cout << "Vertices (V): " << g.V << " | Aristas (E): " << g.E << "\n";
    std::cout << "Consulta: " << g.start_node << " -> " << g.end_node << "\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Tiempo de carga del grafo: " << load_ms << " ms\n";
    std::cout << "----------------------------------------\n";

    // 2. Medición de ejecución de Dijkstra
    auto t0_algo = std::chrono::steady_clock::now();
    DijkstraResult res = run_dijkstra(g);
    auto t1_algo = std::chrono::steady_clock::now();

    double algo_ms = std::chrono::duration<double, std::milli>(t1_algo - t0_algo).count();

    if (!res.valid_vertices) {
      std::cout << res.error_message << "\n";
      std::cout << "Calculo abortado.\n";
    } else {
      std::cout << "Tiempo de ejecucion Dijkstra: " << algo_ms << " ms\n";
      if (!res.path_found) {
        std::cout << "Resultado: No existe camino entre " << g.start_node << " y " << g.end_node << ".\n";
      } else {
        std::cout << "Costo minimo: " << res.total_cost << "\n";
        std::cout << "Nodos en ruta (" << res.path.size() << "): ";
        if (res.path.size() <= 20) {
          for (size_t i = 0; i < res.path.size(); ++i) {
            std::cout << res.path[i] << (i + 1 < res.path.size() ? " -> " : "");
          }
        } else {
          std::cout << res.path.front() << " -> ... -> " << res.path.back();
        }
        std::cout << "\n";
      }
    }
    std::cout << "========================================\n";

  } catch (const std::exception& ex) {
    std::cerr << "Excepcion: " << ex.what() << "\n";
    return 1;
  }

  return 0;
}

