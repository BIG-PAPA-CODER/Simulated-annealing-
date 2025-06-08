#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <chrono>
#include <tuple>

bool RUN_BENCHMARK = false;
std::string FILE_NAME = "wyniki.csv";
std::string BENCHMARK_FILE_NAME = "benchmark_" + FILE_NAME;

struct Item {
    int value; //wartość
    int weight; //waga
    int volume; //objętość
};

struct Problem {
    std::vector<Item> items;
    int maxWeight;
    int maxVolume;
};

bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void logResultToCSV(const std::string& filename, int runId, int iterationCount, double bestScore,
    int bestIteration, double tempStart, double coolingRate, long timeMicros) {

    bool addHeader = !fileExists(filename); // tylko jeśli plik nie istnieje

    std::ofstream file(filename, std::ios::app); // otwarcie w trybie dopisywania
    if (addHeader) {
        file << "run,iterations,bestScore,bestIteration,tempStart,coolingRate,timeMicros\n";
    }

    // zapis danych z kropką jako separator dziesiętny (Excel: opcjonalna zamiana potem)
    file << runId << "," << iterationCount << "," << bestScore << "," << bestIteration
        << "," << tempStart << "," << coolingRate << "," << timeMicros << "\n";

    file.close();
}

// Funkcja celu z karą za przekroczenie ograniczeń
double objectiveFunction(const std::vector<bool>& solution, const Problem& problem) {
    int totalValue = 0;
    int totalWeight = 0;
    int totalVolume = 0;

    for (size_t i = 0; i < solution.size(); ++i) {
        if (solution[i]) {
            totalValue += problem.items[i].value;
            totalWeight += problem.items[i].weight;
            totalVolume += problem.items[i].volume;
        }
    }

    // Kara za przekroczenia
    int penalty = 0;
    if (totalWeight > problem.maxWeight)
        penalty += 10 * (totalWeight - problem.maxWeight);
    if (totalVolume > problem.maxVolume)
        penalty += 10 * (totalVolume - problem.maxVolume);

    return totalValue - penalty;
}

// Losowe sąsiedztwo: odwraca stan losowego przedmiotu
std::vector<bool> getNeighbor(const std::vector<bool>& currentSolution, double temperature, double tempStart) {
    std::vector<bool> neighbor = currentSolution;

    // Liczba elementów do zmiany zależy od temperatury (np. proporcjonalnie do temp/tempStart)
    int maxFlips = std::max(1, (int)(currentSolution.size() * (temperature / tempStart)));

    for (int i = 0; i < maxFlips; ++i) {
        int index = rand() % currentSolution.size();
        neighbor[index] = !neighbor[index];
    }

    return neighbor;
}

std::tuple<double, int, long> simulatedAnnealing(const Problem& problem, int iterations, double tempStart, double coolingRate) {
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<double> scoresHistory;
    std::vector<double> temperatureHistory;
    int bestIteration = 0;

    std::vector<bool> currentSolution(problem.items.size());
    for (size_t i = 0; i < currentSolution.size(); ++i)
        currentSolution[i] = rand() % 2;

    double currentScore = objectiveFunction(currentSolution, problem);
    double temperature = tempStart;

    std::vector<bool> bestSolution = currentSolution;
    double bestScore = currentScore;

    for (int i = 0; i < iterations; ++i) {
        std::vector<bool> neighbor = getNeighbor(currentSolution, temperature, tempStart);
        double neighborScore = objectiveFunction(neighbor, problem);

        double delta = neighborScore - currentScore;
        if (delta > 0 || (exp(delta / temperature) > ((double)rand() / RAND_MAX))) {
            currentSolution = neighbor;
            currentScore = neighborScore;
        }

        if (currentScore > bestScore) {
            bestSolution = currentSolution;
            bestScore = currentScore;
            bestIteration = i;
        }
    
        temperature *= coolingRate;

        scoresHistory.push_back(currentScore);
        temperatureHistory.push_back(temperature);

    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationMicro = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();


    std::cout << "Best score: " << bestScore << "\n";
    std::cout << "Found at iteration: " << bestIteration << "\n";
    std::cout << "Items selected:\n";
    for (size_t i = 0; i < bestSolution.size(); ++i) {
        if (bestSolution[i]) {
            std::cout << "- Item " << i
                << " [value=" << problem.items[i].value
                << ", weight=" << problem.items[i].weight
                << ", volume=" << problem.items[i].volume << "]\n";
        }
    }

    std::ofstream file(FILE_NAME);
    file << "iteracja,wynik,temperatura\n";
    for (size_t i = 0; i < scoresHistory.size(); ++i) {
        file << i << "," << scoresHistory[i] << "," << temperatureHistory[i] << "\n";
    }
    file.close();

    return std::make_tuple(bestScore, bestIteration, durationMicro);
}

int main() {
    srand(time(0));

    Problem problem = {
        {
            {10, 5, 3},
            {8, 4, 2},
            {15, 8, 6},
            {4, 2, 1},
            {12, 6, 4},
            {6, 3, 2},
            {11, 7, 5},
            {7, 4, 3}
        },
        20,  // max weight
        15   // max volume
    };

    if (!RUN_BENCHMARK) {
        simulatedAnnealing(problem, 10000, 100.0, 0.99);
        return 0;
    }
    else {


        int runId = 0;

        // 1️⃣ Test różnych liczby iteracji (przy stałej temperaturze i chłodzeniu)
        std::vector<int> iterationsSet = { 100, 150, 200, 300, 500 };
        for (int iter : iterationsSet) {
            for (int i = 0; i < 100; ++i) {
                double tempStart = 100.0;
                double coolingRate = 0.99;

                std::tuple<double, int, long> result = simulatedAnnealing(problem, iter, tempStart, coolingRate);
                double bestScore = std::get<0>(result);
                int bestIteration = std::get<1>(result);
                long timeMicros = std::get<2>(result);
                logResultToCSV(BENCHMARK_FILE_NAME, runId++, iter, bestScore, bestIteration, tempStart, coolingRate, timeMicros);
            }
        }

        // 2️⃣ Test różnych temperatur początkowych (przy stałej liczbie iteracji i chłodzeniu)
        std::vector<double> tempStarts = { 10.0, 50.0, 100.0, 200.0, 500.0 };
        int fixedIterations = 300;
        for (double tempStart : tempStarts) {
            for (int i = 0; i < 100; ++i) {
                double coolingRate = 0.99;

                std::tuple<double, int, long> result = simulatedAnnealing(problem, fixedIterations, tempStart, coolingRate);
                double bestScore = std::get<0>(result);
                int bestIteration = std::get<1>(result);
                long timeMicros = std::get<2>(result);
                logResultToCSV(BENCHMARK_FILE_NAME, runId++, fixedIterations, bestScore, bestIteration, tempStart, coolingRate, timeMicros);
            }
        }

        // 3️⃣ Test różnych współczynników chłodzenia (przy stałej liczbie iteracji i temperaturze)
        std::vector<double> coolingRates = { 0.999, 0.995, 0.99, 0.98, 0.95 };
        double fixedTemp = 100.0;
        for (double cooling : coolingRates) {
            for (int i = 0; i < 100; ++i) {
                std::tuple<double, int, long> result = simulatedAnnealing(problem, fixedIterations, fixedTemp, cooling);
                double bestScore = std::get<0>(result);
                int bestIteration = std::get<1>(result);
                long timeMicros = std::get<2>(result);
                logResultToCSV(BENCHMARK_FILE_NAME, runId++, fixedIterations, bestScore, bestIteration, fixedTemp, cooling, timeMicros);
            }
        }

        std::cout << "\nEksperyment zakończony. Dane zapisane do pliku: " << BENCHMARK_FILE_NAME << std::endl;

        return 0;
    }
}
