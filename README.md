# Dijkstra - Priority Queue (Min-Heap)

Implementación de la variante óptima de Dijkstra con Cola de Prioridad (Min-Heap) sin eliminación perezosa (lazy deletion)
y representación mediante Forward Star (Compressed Sparse Row - CSR) o listas de adyacencia contiguas.
Límites: `1 <= V <= 1 000 000` , `1<= E <= 500 000`

## Fundamento de elección

- **Min-Heap (O((V + E) log V)) vs. Matriz de Adyacencia (O(V²)):** Con 10⁶ vértices, una matriz requeriría ~4 TB de RAM y 10¹²
  operaciones. Además, dado que E ≤ 0.5V, el grafo es extremadamente disperso (*sparse*).
- **Min-Heap Estándar (std::priority_queue) vs. Fibonacci Heap:** Aunque Fibonacci Heap tiene mejor complejidad teórica
  (O((V + E) log V)), su constante oculta por punteros y fallos de caché lo hace mucho más lento en la práctica que un heap binario
  contiguo en memoria.
- **CSR (Compressed Sparse Row) vs. vector<vector<pair>>:** Al tener hasta 10⁶ vértices, instanciar un millón de `std::vector`
  individuales fragmenta la memoria y penaliza la localidad de caché. Usar arreglos planos contiguos (head, to, weight, next)
  elimina la sobrecarga de asignación dinámica por nodo y acelera tanto la lectura como el recorrido.
- **Lectura rápida con mmap / buffer estático (fread):** Leer 500,000 líneas con `std::ifstream` y `>>` toma varios cientos de
  milisegundos. Un parser manual sobre un bloque leído con `fread` reduce la carga a pocos milisegundos.

## Compilación y ejecución

```bash
g++ -std=c++20 -O3 -march=native main.cpp -o main
./main grafo.txt
```
