#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using Map = std::vector<std::vector<int>>;

void printMap(const Map& map) {
    std::cout << "--- Current Map ---\n";
    for (const auto& row : map) {
        for (int cell : row) {
            std::cout << (cell == 0 ? '.' : '#') << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------\n";
}

Map cellularAutomata(const Map& currentMap, int W, int H, int R, double U) {
    Map newMap(H, std::vector<int>(W, 0));
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            int count = 0;
            for (int di = -R; di <= R; ++di) {
                for (int dj = -R; dj <= R; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < H && nj >= 0 && nj < W) {
                        count += currentMap[ni][nj];
                    }
                }
            }
            newMap[i][j] = (count > U) ? 1 : 0;
        }
    }
    return newMap;
}

Map cellularAutomataInPlace(Map currentMap, int W, int H, int R, double U) {
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            int count = 0;
            for (int di = -R; di <= R; ++di) {
                for (int dj = -R; dj <= R; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < H && nj >= 0 && nj < W) {
                        count += currentMap[ni][nj];
                    }
                }
            }
            currentMap[i][j] = (count > U) ? 1 : 0;
        }
    }
    return currentMap;
}

Map drunkAgent(Map currentMap, int W, int H, int J, int I, int roomSizeX, int roomSizeY,
               double probGenerateRoom, double probIncreaseRoom,
               double probChangeDirection, double probIncreaseChange,
               int& agentX, int& agentY) {
    Map newMap = currentMap;
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<> dirDist(0, 3);

    double currentProbRoom = probGenerateRoom;
    double currentProbChange = probChangeDirection;
    int dir = dirDist(rng);

    auto placeRoom = [&](int x, int y) {
        int startX = std::max(0, x - roomSizeX / 2);
        int endX = std::min(W - 1, x + roomSizeX / 2);
        int startY = std::max(0, y - roomSizeY / 2);
        int endY = std::min(H - 1, y + roomSizeY / 2);
        for (int i = startY; i <= endY; ++i) {
            for (int j = startX; j <= endX; ++j) {
                newMap[i][j] = 1;
            }
        }
    };

    for (int j = 0; j < J; ++j) {
        for (int i = 0; i < I; ++i) {
            if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W) {
                newMap[agentX][agentY] = 1;
            }

            int dx[] = {0, -1, 0, 1};
            int dy[] = {1, 0, -1, 0};
            int nextX = agentX + dx[dir];
            int nextY = agentY + dy[dir];

            if (nextX < 0 || nextX >= H || nextY < 0 || nextY >= W) {
                dir = dirDist(rng);
                continue;
            }

            agentX = nextX;
            agentY = nextY;
        }

        if (dist(rng) <= currentProbRoom) {
            placeRoom(agentX, agentY);
            currentProbRoom = probGenerateRoom;
        } else {
            currentProbRoom += probIncreaseRoom;
        }

        if (dist(rng) <= currentProbChange) {
            dir = dirDist(rng);
            currentProbChange = probChangeDirection;
        } else {
            currentProbChange += probIncreaseChange;
        }
    }

    return newMap;
}

Map initializeRandomMap(int W, int H) {
    Map map(H, std::vector<int>(W, 0));
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dist(0, 1);
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            map[i][j] = dist(rng);
        }
    }
    return map;
}

int main() {
    std::cout << "--- CELLULAR AUTOMATA AND DRUNK AGENT SIMULATION ---\n";

    // CONF. INICIAL
    int mapRows = 20;
    int mapCols = 40;

    // NUMEROS ALEATORIOS
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> rDist(1, 2); // ca_R
    std::uniform_int_distribution<> iterDist(2, 5); // ca_iterations
    std::uniform_int_distribution<> jDist(10, 30); // da_J
    std::uniform_int_distribution<> iDist(3, 7); // da_I
    std::uniform_int_distribution<> roomXDist(3, 7); // da_roomSizeX
    std::uniform_int_distribution<> roomYDist(2, 5); // da_roomSizeY
    std::uniform_real_distribution<> probDist(0.2, 0.6); // Probabilidades iniciales
    std::uniform_real_distribution<> incrDist(0.05, 0.2); // Incrementos

    // --- SIMULACION CELLULAR AUTOMATA (2da Grilla)---
    std::cout << "\nCellular Automata Simulation (With Second Grid):\n";
    Map cellularMap = initializeRandomMap(mapCols, mapRows);
    std::cout << "Initial random map state:\n";
    printMap(cellularMap);

    // PARAMETROS ALEATORIOS PARA CELLULAR AUTOMATA (2da Grilla)
    int ca_W = mapCols;
    int ca_H = mapRows;
    int ca_R = rDist(rng);
    double ca_U = (ca_R == 1) ? std::uniform_real_distribution<>(2.0, 5.0)(rng) : std::uniform_real_distribution<>(4.0, 8.0)(rng);
    int ca_iterations = iterDist(rng);
    std::cout << "Parameters: R=" << ca_R << ", U=" << ca_U << ", Iterations=" << ca_iterations << "\n";

    // BUCLE CELLULAR AUTOMATA (2da Grilla)
    Map cellularMapSecondGrid = cellularMap;
    for (int i = 0; i < ca_iterations; ++i) {
        std::cout << "\nCellular Automata (Second Grid) Iteration " << i + 1 << ":\n";
        cellularMapSecondGrid = cellularAutomata(cellularMapSecondGrid, ca_W, ca_H, ca_R, ca_U);
        printMap(cellularMapSecondGrid);
    }

    // --- SIMULACION CELLULAR AUTOMATA (sin 2da grilla) ---
    std::cout << "\nCellular Automata Simulation (In-Place, Optional Point 5):\n";
    std::cout << "Initial random map state (same as above):\n";
    printMap(cellularMap);
    std::cout << "Parameters: R=" << ca_R << ", U=" << ca_U << ", Iterations=" << ca_iterations << "\n";

    // BUCLE CEllUlAR AUTOMATA (sin 2da grilla)
    for (int i = 0; i < ca_iterations; ++i) {
        std::cout << "\nCellular Automata (In-Place) Iteration " << i + 1 << ":\n";
        cellularMap = cellularAutomataInPlace(cellularMap, ca_W, ca_H, ca_R, ca_U);
        printMap(cellularMap);
    }

    // --- SIMULACION DRUNK AGENT ---
    std::cout << "\nDrunk Agent Simulation:\n";
    Map drunkMap(mapRows, std::vector<int>(mapCols, 0));
    std::uniform_int_distribution<> posDistX(0, mapRows - 1);
    std::uniform_int_distribution<> posDistY(0, mapCols - 1);
    int drunkAgentX = posDistX(rng);
    int drunkAgentY = posDistY(rng);

    std::cout << "Initial empty map with agent at (" << drunkAgentX << ", " << drunkAgentY << "):\n";
    drunkMap[drunkAgentX][drunkAgentY] = 1;
    printMap(drunkMap);

    // PARAMETROS ALEATORIOS DRUNK AGENT
    int da_W = mapCols;
    int da_H = mapRows;
    int da_J = jDist(rng);
    int da_I = iDist(rng);
    int da_roomSizeX = roomXDist(rng);
    int da_roomSizeY = roomYDist(rng);
    double da_probGenerateRoom = probDist(rng);
    double da_probIncreaseRoom = incrDist(rng);
    double da_probChangeDirection = probDist(rng);
    double da_probIncreaseChange = incrDist(rng);
    std::cout << "Parameters: J=" << da_J << ", I=" << da_I << ", RoomSizeX=" << da_roomSizeX
              << ", RoomSizeY=" << da_roomSizeY << ", ProbGenerateRoom=" << da_probGenerateRoom
              << ", ProbIncreaseRoom=" << da_probIncreaseRoom << ", ProbChangeDirection=" << da_probChangeDirection
              << ", ProbIncreaseChange=" << da_probIncreaseChange << "\n";

    // EJECUTAR DRUNK AGENT
    drunkMap = drunkAgent(drunkMap, da_W, da_H, da_J, da_I, da_roomSizeX, da_roomSizeY,
                          da_probGenerateRoom, da_probIncreaseRoom,
                          da_probChangeDirection, da_probIncreaseChange,
                          drunkAgentX, drunkAgentY);
    std::cout << "\nFinal Drunk Agent map:\n";
    printMap(drunkMap);

    std::cout << "\n--- Simulation Completed ---\n";
    return 0;
}