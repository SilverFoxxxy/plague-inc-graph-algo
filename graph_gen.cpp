#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
  freopen("input.txt", "w", stdout);
  int n = 10000;

  // Средняя степень одной вершины
  double deg = 1.3;

  // Вероятность одного ребра
  double p = deg * 0.5 / n;

  vector <pair <int, int> > edges;

  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      if (rand() % n <= p * n) {
        edges.push_back({i + 1, j + 1});
      }
    }
  }

  int m = min(1000000, (int)edges.size());

  cout << m << endl;
  for (auto e: edges) {
    cout << e.first << ' ' << e.second << "\n";
  }
}
