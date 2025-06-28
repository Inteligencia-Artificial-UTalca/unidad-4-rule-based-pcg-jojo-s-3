#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>

using namespace std;

const int WIDTH = 15;
const int HEIGHT = 8;

enum Tile {
    WALL,
    SOFT_BLOCK,
    PATH,
    POWER_UP
};

vector<vector<Tile>> map;

void initializeMap() {
    map = vector<vector<Tile>>(HEIGHT, vector<Tile>(WIDTH, WALL));
}

void applyCellularAutomata(int iterations = 3) {
    // Inicialización aleatoria
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            map[y][x] = (rand() % 100 < 55) ? WALL : PATH;
        }
    }

    // Aplicar reglas por iteraciones
    for (int it = 0; it < iterations; ++it) {
        vector<vector<Tile>> newMap = map;

        for (int y = 1; y < HEIGHT - 1; y++) {
            for (int x = 1; x < WIDTH - 1; x++) {
                int wallCount = 0;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (map[y + dy][x + dx] == WALL) wallCount++;
                    }
                }
                newMap[y][x] = (wallCount >= 6) ? WALL : PATH;
            }
        }

        map = newMap;
    }
}

void applyRandomWalker(int walkers = 4, int steps = 35) {
    for (int w = 0; w < walkers; w++) {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;

        for (int s = 0; s < steps; s++) {
            map[y][x] = PATH;

            int dir = rand() % 4;
            switch (dir) {
                case 0: if (x > 1) x--; break;
                case 1: if (x < WIDTH - 2) x++; break;
                case 2: if (y > 1) y--; break;
                case 3: if (y < HEIGHT - 2) y++; break;
            }
        }
    }
}

void placeSoftBlocksAndPowerUps() {
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            if (map[y][x] == PATH && rand() % 100 < 60) {
                map[y][x] = (rand() % 100 < 20) ? POWER_UP : SOFT_BLOCK;
            }
        }
    }
}

void printMap() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            switch (map[y][x]) {
                case WALL: cout << "#"; break;
                case PATH: cout << " "; break;
                case SOFT_BLOCK: cout << "+"; break;
                case POWER_UP: cout << "*"; break;
            }
        }
        cout << endl;
    }
}

// Verifica si todas las celdas PATH son alcanzables desde el punto inicial
bool isMapFullyConnected() {
    vector<vector<bool>> visited(HEIGHT, vector<bool>(WIDTH, false));
    queue<pair<int, int>> q;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == PATH) {
                q.push({y, x});
                visited[y][x] = true;
                goto bfs_start;
            }
        }
    }

bfs_start:
    const int dx[] = {1, -1, 0, 0};
    const int dy[] = {0, 0, 1, -1};

    while (!q.empty()) {
        auto [y, x] = q.front(); q.pop();
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                if (!visited[ny][nx] && map[ny][nx] == PATH) {
                    visited[ny][nx] = true;
                    q.push({ny, nx});
                }
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            if (map[y][x] == PATH && !visited[y][x])
                return false;

    return true;
}

void scatterHardBlocksInOpenAreas(int chance = 15) {
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            if (map[y][x] != PATH) continue;

            int pathNeighbors = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue;
                    int ny = y + dy, nx = x + dx;
                    if (map[ny][nx] == PATH) pathNeighbors++;
                }
            }

            // Si está en una zona muy abierta, chance de convertir en muro
            if (pathNeighbors >= 5 && rand() % 100 < chance) {
                map[y][x] = WALL;
            }
        }
    }
}

void generateMap() {
    initializeMap();
    applyCellularAutomata();      // Estructura base
    applyRandomWalker();          // Asegura conexión
    placeSoftBlocksAndPowerUps(); // Inserta power-ups y bloques blandos
    scatterHardBlocksInOpenAreas(); //bloques duros en zonas abiertas <%
}

int main() {
    srand(time(0));
    char input;

    do {
        do {
            generateMap();
        } while (!isMapFullyConnected());

        cout << "\033[2J\033[1;1H"; // Limpia la consola
        printMap();
        cout << "\nPresiona ENTER para generar otro mapa o escribe 'q' y ENTER para salir: ";
        input = cin.get();
    } while (input != 'q');

    return 0;
}
