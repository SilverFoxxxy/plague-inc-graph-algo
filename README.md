### Белков Иван Иванович

# Задача 2 / Алгоритмы

По мотивам Plague Inc.

---

# Запуск решения

Решение хранится в одном файле main.cpp

Скомпилировать и запустить (на Linux подобных системах):

Одной командой:
~~~
g++ -std=c++17 main.cpp -o a.out; ./a.out
~~~

Только скомпилировать:
~~~
g++ -std=c++17 main.cpp -o a.out
~~~

Только запустить:
~~~
./a.out
~~~

# Описание решения

Подготовка и разбиение на компоненты:
1. Построим граф городов
2. Вершины с одним соседом и изолированные вершины не смогут заразиться от соседей - заражаем их сразу
3. От них по максимуму заражаем вершины через соседей.

4. В полученном графе выделяем компоненты не заражённых вершин, для них решаем задачу независимо

Решение для одной компоненты:
Запустим случайный алгоритм несколько раз (столько, сколько точно успеем)

Шаги одного запуска случайного алгоритма:
1. random_shuffle для списка вершин
2. Идём по вершинам, если ещё не заражена - заражаем
3. По возможности заражаем всех через соседей
4. Повторяем пункт (2), пока не дошли до конца списка

Под каждую компоненту создаём объект класса World
Запускаем RandomSearch для поиска ответа
