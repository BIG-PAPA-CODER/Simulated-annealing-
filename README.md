# Multi-Constraint Knapsack Solver (Simulated Annealing)

Projekt realizuje rozszerzony problem plecakowy (ang. **knapsack problem**) z dwoma ograniczeniami: wagowym i objętościowym. Do rozwiązania wykorzystano **algorytm symulowanego wyżarzania** (simulated annealing), wzbogacony o system kar i opcję benchmarkowania.

## Problem

Dany jest zbiór przedmiotów, z których każdy ma:
- wartość (`value`),
- wagę (`weight`),
- objętość (`volume`).

Celem jest wybranie podzbioru przedmiotów tak, aby:
- maksymalizować sumę wartości,
- nie przekroczyć maksymalnej wagi i objętości.

## Zastosowany algorytm

Zaimplementowano zmodyfikowany algorytm **Simulated Annealing** z:
- dynamiczną liczbą zmian w sąsiedztwie (zależną od temperatury),
- funkcją celu z karą za przekroczenia ograniczeń,
- mechanizmem chłodzenia,
- historią wyników zapisywaną do CSV,
- trybem benchmarku z automatycznymi eksperymentami.

## Kompilacja

Wymagany kompilator C++11 lub nowszy. Kompilacja przykładowo:

```bash
g++ -std=c++11 -o knapsack_solver main.cpp
```

## Uruchomienie

Domyślnie program uruchamia pojedyncze wykonanie z domyślnymi parametrami:

```bash
./knapsack_solver
```

Plik `wyniki.csv` zostanie utworzony z zapisem:
- wartości funkcji celu,
- temperatury w danej iteracji.

## Tryb benchmarku

Aby uruchomić tryb eksperymentów (zapis do `benchmark_wyniki.csv`), zmień wartość zmiennej `RUN_BENCHMARK` w kodzie:

```cpp
bool RUN_BENCHMARK = true;
```

Wówczas program przeprowadzi:
1. Test różnych liczby iteracji.
2. Test różnych temperatur początkowych.
3. Test różnych współczynników chłodzenia.

Dane wynikowe będą zawierać:
- `run`: numer uruchomienia,
- `iterations`: liczba iteracji,
- `bestScore`: najlepszy wynik,
- `bestIteration`: iteracja, w której osiągnięto wynik,
- `tempStart`: temperatura początkowa,
- `coolingRate`: współczynnik chłodzenia,
- `timeMicros`: czas działania w mikrosekundach.

## Struktura danych

Każdy przedmiot to struktura:

```cpp
struct Item {
    int value;
    int weight;
    int volume;
};
```

Cały problem to:

```cpp
struct Problem {
    std::vector<Item> items;
    int maxWeight;
    int maxVolume;
};
```

## 🧪 Dane przykładowe

```cpp
Problem problem = {
    {
        {10, 5, 3}, {8, 4, 2}, {15, 8, 6}, {4, 2, 1},
        {12, 6, 4}, {6, 3, 2}, {11, 7, 5}, {7, 4, 3}
    },
    20, // maksymalna waga
    15  // maksymalna objętość
};
```

## 📈 Pliki wyjściowe

- `wyniki.csv`: historia wyników i temperatur dla pojedynczego uruchomienia.
- `benchmark_wyniki.csv`: dane z wielu uruchomień dla różnych parametrów.

Można je analizować w arkuszach kalkulacyjnych lub np. za pomocą Pythona (pandas/matplotlib).

## 📃 Licencja

Projekt edukacyjny. Można dowolnie modyfikować i wykorzystywać.
