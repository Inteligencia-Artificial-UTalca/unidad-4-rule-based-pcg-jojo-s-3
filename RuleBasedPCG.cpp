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
    Map newMap(H, std::vector<int>(W, 0)); // Nueva grilla
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            // Contar vecinos con '1' en una ventana de radio R
            int count = 0;
            for (int di = -R; di <= R; ++di) {
                for (int dj = -R; dj <= R; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    // Tratar bordes como 0
                    if (ni >= 0 && ni < H && nj >= 0 && nj < W) {
                        count += currentMap[ni][nj];
                    }
                }
            }
            // Aplicar la regla
            newMap[i][j] = (count > U) ? 1 : 0; // Umbral absoluto
        }
    }
    return newMap;
}

Map drunkAgent(Map currentMap, int W, int H, int J, int I, int roomSizeX, int roomSizeY,
               double probGenerateRoom, double probIncreaseRoom,
               double probChangeDirection, double probIncreaseChange,
               int& agentX, int& agentY) {
    Map newMap = currentMap;
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<> dirDist(0, 3); // 0:right, 1:up, 2:left, 3:down

    double currentProbRoom = probGenerateRoom;
    double currentProbChange = probChangeDirection;
    int dir = dirDist(rng); // Dirección inicial

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
            // Marcar la posición actual como pasillo
            if (agentX >= 0 && agentX < H && agentY >= 0 && agentY < W) {
                newMap[agentX][agentY] = 1;
            }

            // Mover el agente
            int dx[] = {0, -1, 0, 1}; // right, up, left, down
            int dy[] = {1, 0, -1, 0};
            int nextX = agentX + dx[dir];
            int nextY = agentY + dy[dir];

            // Verificar límites
            if (nextX < 0 || nextX >= H || nextY < 0 || nextY >= W) {
                dir = dirDist(rng); // Cambiar dirección
                continue;
            }

            agentX = nextX;
            agentY = nextY;
        }

        // Intentar generar habitación
        if (dist(rng) <= currentProbRoom) {
            placeRoom(agentX, agentY);
            currentProbRoom = probGenerateRoom; // Reset probabilidad
        } else {
            currentProbRoom += probIncreaseRoom;
        }

        // Intentar cambio de dirección
        if (dist(rng) <= currentProbChange) {
            dir = dirDist(rng);
            currentProbChange = probChangeDirection; // Reset probabilidad
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

    // Configuración inicial
    int mapRows = 20;
    int mapCols = 40;

    // --- Simulación Cellular Automata ---
    std::cout << "\nCellular Automata Simulation:\n";
    Map cellularMap = initializeRandomMap(mapCols, mapRows);
    std::cout << "Initial random map state:\n";
    printMap(cellularMap);

    // Parámetros del Cellular Automata
    int ca_W = mapCols;
    int ca_H = mapRows;
    int ca_R = 1;      // Radio
    double ca_U = 3;   // Umbral (más denso)
    int ca_iterations = 3;

    // Bucle de Cellular Automata
    for (int i = 0; i < ca_iterations; ++i) {
        std::cout << "\nCellular Automata Iteration " << i + 1 << ":\n";
        cellularMap = cellularAutomata(cellularMap, ca_W, ca_H, ca_R, ca_U);
        printMap(cellularMap);
    }

    // --- Simulación Drunk Agent ---
    std::cout << "\nDrunk Agent Simulation:\n";
    Map drunkMap(mapRows, std::vector<int>(mapCols, 0));
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> posDistX(0, mapRows - 1);
    std::uniform_int_distribution<> posDistY(0, mapCols - 1);
    int drunkAgentX = posDistX(rng); // Posición inicial aleatoria
    int drunkAgentY = posDistY(rng);

    std::cout << "Initial empty map with agent at (" << drunkAgentX << ", " << drunkAgentY << "):\n";
    drunkMap[drunkAgentX][drunkAgentY] = 1; // Marcar posición inicial
    printMap(drunkMap);

    // Parámetros del Drunk Agent
    int da_W = mapCols;
    int da_H = mapRows;
    int da_J = 20;      // Más movimientos
    int da_I = 5;       // Pasos por movimiento
    int da_roomSizeX = 5;
    int da_roomSizeY = 3;
    double da_probGenerateRoom = 0.4;    // Más probabilidad de habitaciones
    double da_probIncreaseRoom = 0.15;   // Incremento más rápido
    double da_probChangeDirection = 0.6; // Más cambios de dirección
    double da_probIncreaseChange = 0.1;  // Incremento más rápido

    // Ejecutar Drunk Agent
    drunkMap = drunkAgent(drunkMap, da_W, da_H, da_J, da_I, da_roomSizeX, da_roomSizeY,
                          da_probGenerateRoom, da_probIncreaseRoom,
                          da_probChangeDirection, da_probIncreaseChange,
                          drunkAgentX, drunkAgentY);
    std::cout << "\nFinal Drunk Agent map:\n";
    printMap(drunkMap);

    std::cout << "\n--- Simulation Completed ---\n";
    return 0;
}