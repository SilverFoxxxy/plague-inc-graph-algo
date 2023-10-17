#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <random>
#include <queue>

using namespace std;

const int INF = 1e9;

void my_random_shuffle(vector <int>& a) {
  random_device rd;
  mt19937 g(rd());

  shuffle(a.begin(), a.end(), g);
}

class World {
public:
  World() {}

  World(int n) {
    SetSize(n); 
  }

  // Задаём размер графа
  void SetSize(int n) {
    n_ = n;
    g_.resize(n_);
    infected_.assign(n_, false);
    infected_friend_count_.assign(n_, 0);
    subgraph_mark_.assign(n_, -1);
  }

  // Задаём рёбра
  void AddEdges(const vector <pair <int, int> >& edges) {
    for (auto e: edges) {
      int u = e.first;
      int v = e.second;
      g_[u].push_back(v);
      g_[v].push_back(u);
    }
  }

  // Заражаем вершину
  void InfectVertex(int u) {
    if (infected_[u]) return;

    infected_[u] = true;
    infected_event_queue_.push(u);
  }

  // Заражаем все вершины, которые могут
  // быть заражены
  void UpdateInfection() {
    // Пока есть новое событие заражения
    while (!infected_event_queue_.empty()) {
      int u = infected_event_queue_.front();
      infected_event_queue_.pop();

      // cout << "u: " << u + 1 << endl;

      // Каждый сосед - потенциально новый заражённый
      for (int v: g_[u]) {
        if (infected_[v]) continue;

        infected_friend_count_[v]++;

        // cout << "-> " << v + 1 << ' ' << infected_friend_count_[v] << endl;
        if (infected_friend_count_[v] >= 2) {
          InfectVertex(v);
        }
      }
    }
  }

  // Находим и заражаем изолированные вершины и листья
  // От соседей они заразиться не смогут
  void InfectIsolated() {
    for (int i = 0; i < n_; i++) {
      if (g_[i].size() <= 1) {
        InfectVertex(i);
        initiators_.push_back(i);
      }
    }
    UpdateInfection();
  }

  // dfs для поиска компонент связности не заражённых вершин
  void dfs(int u, vector <bool>& used, vector <int>& component) {
    if (used[u]) return;

    component.push_back(u);
    used[u] = true;

    for (int v: g_[u]) {
      dfs(v, used, component);
    }
  }

  // Поиск компонент не заражённых вершин
  void FindComponents(vector <vector <int> >& components) {
    vector <bool> used(n_, false);

    for (int u = 0; u < n_; u++) {
      used[u] = infected_[u];
    }

    for (int u = 0; u < n_; u++) {
      if (!used[u]) {
        vector <int> component;
        components.push_back(component);
        dfs(u, used, components.back());
      }
    }
  }

  void SaveState(vector <bool>& infected_backup, vector <int>& infected_friend_count_backup) {
    infected_backup.resize(n_);
    infected_friend_count_backup.resize(n_);

    for (int i = 0; i < n_; i++) {
      infected_backup[i] = infected_[i];
      infected_friend_count_backup[i] = infected_friend_count_[i];
    }
  }

  void RestoreState(vector <bool>& infected_backup, vector <int>& infected_friend_count_backup) {
    for (int i = 0; i < n_; i++) {
      infected_[i] = infected_backup[i];
      infected_friend_count_[i] = infected_friend_count_backup[i];
    }

    while (!infected_event_queue_.empty()) {
      infected_event_queue_.pop();
    }
    initiators_.clear();
  }

  // Проверяем, что все вершины заражены
  bool CheckSuccess() {
    int cnt = 0;
    for (int i = 0; i < n_; i++) {
      cnt += infected_[i];
    }

    return (cnt == n_);
  }

  void RandomSearch(int iter_number, vector <int>& res_initiators) {
    vector <bool> infected_backup;
    vector <int> infected_friend_count_backup;
    SaveState(infected_backup, infected_friend_count_backup);

    // Самый небольшой найденный набор вершин,
    // который сможет заразить все
    int ans = INF;
    vector <int> ans_initiators;

    // Текущий набор вершин
    int cur_ans = 0;
    vector <int> cur_initiators;

    // Список индексов вершин для random_shuffle
    vector <int> vertex;
    for (int i = 0; i < n_; i++) {
      vertex.push_back(i);
    }

    bool too_much = false;
    // Повторяем поиск ограниченное число раз
    for (int j = 0; j < iter_number; j++) {
      my_random_shuffle(vertex);
      cur_initiators.clear();
      cur_ans = 0;

      // cout << "RandomSearch" << endl;
      for (int i = 0; i < n_; i++) {
        int u = vertex[i];
        if (infected_[u]) continue;

        InfectVertex(u);
        UpdateInfection();

        cur_ans++;
        cur_initiators.push_back(u);

        if (cur_ans >= ans) {
          too_much = true;
          break;
        }
      }

      if (CheckSuccess()) {
        if (cur_ans < ans) {
          ans = cur_ans;
          swap(cur_initiators, ans_initiators);
        }
      }

      RestoreState(infected_backup, infected_friend_count_backup);

      if (ans == 1) break;
    }

    for (int u: ans_initiators) {
      res_initiators.push_back(u);
    }
  }

  void CreateSubgraph(World& world, const vector <int>& component) {
    int n = component.size();
    world.SetSize(n);
    for (int i = 0; i < n; i++) {
      subgraph_mark_[component[i]] = i;
    }

    vector <pair <int, int> > edges;
    for (int u: component) {
      int u0 = subgraph_mark_[u];
      for (int v: g_[u]) {
        int v0 = subgraph_mark_[v];
        if (v0 == -1) continue;
        if (u0 < v0) {
          edges.push_back({u0, v0});
        }
      }
    }

    for (int i = 0; i < n; i++) {
      subgraph_mark_[component[i]] = -1;
    }

    world.AddEdges(edges);

    for (int u: component) {
      int u0 = subgraph_mark_[u];
      world.infected_friend_count_[u0] = infected_friend_count_[u];
    }
  }

  int GetEdgeCount() {
    int cnt = 0;
    for (int i = 0; i < n_; i++) {
      cnt += g_[i].size();
    }

    return cnt;
  }

  // Поиск ответа для одной компоненты
  void FindComponentAnswer(const vector <int>& component) {
    World world;
    CreateSubgraph(world, component);

    vector <int> cur_initiators;
    int edge_count = world.GetEdgeCount();

    int one_iter = (component.size() + edge_count);
    int iter_number = 1e8 / (one_iter) / (1 + 1e4 / component.size());
    world.RandomSearch(iter_number, cur_initiators);

    for (int i: cur_initiators) {
      InfectVertex(component[i]);
      initiators_.push_back(component[i]);
    }
    UpdateInfection();
  }

  // Поиск ответа
  void FindAnswer() {
    // Заражаем тех, кто не сможет заразиться от соседей
    InfectIsolated();

    // Находим компоненты связности заражённых вершин
    vector <vector <int> > components;
    FindComponents(components);

    // cout << "comp_n: " << components.size() << "\n";
    // for (int i = 0; i < components.size(); i++) {
    //   cout << "# " << i << endl;
    //   for (int u: components[i]) {
    //     cout << u + 1 << ' ';
    //   } cout << "\n";
    // }

    for (vector <int>& component: components) {
      FindComponentAnswer(component);
    }

    if (!CheckSuccess()) {
      cout << "Error!" << endl;
    }
  }

  vector <int>& GetInitiators() {
    return initiators_;
  }

  void PrintGraph() {
    for (int i = 0; i < n_; i++) {
      if (g_[i].size() != 0) {
        cout << i + 1 << " -> ";
      }
      for (int v: g_[i]) {
        cout << v + 1 << ' ';
      } cout << "\n";
    }
    cout << "\n";
  }

  void PrintInfection() {
    cout << "infection:\n";
    for (int i = 0; i < n_; i++) {
      cout << infected_[i] << ' ';
    } cout << "\n";
  }

private:
  // Количество городов
  int n_;

  // Граф - список смежности
  vector <vector <int> > g_;

  // Список изначально заражённых
  vector <int> initiators_;

  // Пометка "заражённый"
  vector <bool> infected_;

  // Количество заражённых соседей
  vector <int> infected_friend_count_;

  // Очередь из не обработанных новых заражённых
  queue <int> infected_event_queue_;

  // Вспомогательные пометки для "CreateSubgraph"
  vector <int> subgraph_mark_;
};

void solve() {
  vector <pair <int, int> > edges;

  int m;
  cin >> m;

  int u, v;
  int n = 0;
  for (int i = 0; i < m; i++) {
    cin >> u >> v;
    n = max(n, max(u, v));
    u--; v--;
    edges.push_back({u, v});
  }

  World world;
  world.SetSize(n);
  world.AddEdges(edges);

  world.FindAnswer();

  // world.PrintGraph();

  vector <int>& ans = world.GetInitiators();

  cout << ans.size() << "\n";

  for (int u: ans) {
    cout << u + 1 << ' ';
  }

  // vector <int> g[n];
  // for (auto e: edges) {
  //   int u = e.first;
  //   int v = e.second;
  //   g[u].push_back(v);
  //   g[v].push_back(u);
  // }
}

int main() {
  solve();
  return 0;
}

/*
3
1 2
1 3
2 3
*/
