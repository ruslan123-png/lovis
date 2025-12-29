#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Глобальные переменные
int V, ** graph, ** transposedGraph, * visited, * stack, top;

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int safeInputInt(const char* prompt) {
    int value;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';
            if (sscanf(buffer, "%d", &value) == 1) return value;
            printf("Ошибка: введите корректное целое число!\n");
        }
        else {
            printf("Ошибка ввода. Попробуйте снова.\n");
            clearInputBuffer();
        }
    }
}

// ==================== ОСНОВНЫЕ ФУНКЦИИ ГРАФА ====================

void createGraph(int vertices) {
    V = vertices; top = -1;
    graph = (int**)malloc(V * sizeof(int*));
    transposedGraph = (int**)malloc(V * sizeof(int*));
    visited = (int*)calloc(V, sizeof(int));
    stack = (int*)malloc(V * sizeof(int));
    for (int i = 0; i < V; i++) {
        graph[i] = (int*)calloc(V, sizeof(int));
        transposedGraph[i] = (int*)calloc(V, sizeof(int));
    }
}

void addEdge(int from, int to) {
    if (from >= 0 && from < V && to >= 0 && to < V) graph[from][to] = 1;
}

void printMatrix(int** matrix, const char* title) {
    printf("\n%s:\n   ", title);
    for (int i = 0; i < V; i++) printf("%2d ", i);
    printf("\n");
    for (int i = 0; i < V; i++) {
        printf("%2d ", i);
        for (int j = 0; j < V; j++) printf("%2d ", matrix[i][j]);
        printf("\n");
    }
}

// ==================== ФУНКЦИИ СТЕКА ====================

void push(int vertex) { if (top < V - 1) stack[++top] = vertex; }
int pop() { return top >= 0 ? stack[top--] : -1; }
int isStackEmpty() { return top == -1; }

// ==================== ПОИСК В ГЛУБИНУ ====================

void DFS_first(int vertex) {
    visited[vertex] = 1;
    for (int i = 0; i < V; i++)
        if (graph[vertex][i] == 1 && visited[i] == 0) DFS_first(i);
    push(vertex);
}

void DFS_second(int vertex) {
    visited[vertex] = 1;
    printf("%d ", vertex);
    for (int i = 0; i < V; i++)
        if (transposedGraph[vertex][i] == 1 && visited[i] == 0) DFS_second(i);
}

// ==================== АЛГОРИТМ КОСАРАЙЮ ====================

void transposeGraph() {
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            transposedGraph[i][j] = graph[j][i];
}

void findSCC() {
    printf("\n=== ПОИСК КОМПОНЕНТ СИЛЬНОЙ СВЯЗНОСТИ ===\n");

    // Первый проход
    for (int i = 0; i < V; i++) visited[i] = 0;
    top = -1;
    for (int i = 0; i < V; i++)
        if (visited[i] == 0) DFS_first(i);

    printf("Стек после первого прохода: ");
    for (int i = 0; i <= top; i++) printf("%d ", stack[i]);
    printf("\n");

    // Транспонирование
    transposeGraph();
    printMatrix(transposedGraph, "Транспонированный граф");

    // Второй проход
    for (int i = 0; i < V; i++) visited[i] = 0;
    int componentCount = 0;

    printf("\nРЕЗУЛЬТАТ:\n");
    printf("========================================\n");
    while (!isStackEmpty()) {
        int vertex = pop();
        if (vertex != -1 && visited[vertex] == 0) {
            printf("Компонента %d: { ", ++componentCount);
            DFS_second(vertex);
            printf("}\n");
        }
    }
    printf("========================================\n");
    printf("Всего компонент: %d\n", componentCount);
    printf("========================================\n");
}

// ==================== УПРАВЛЕНИЕ ПАМЯТЬЮ ====================

void freeMemory() {
    if (graph) {
        for (int i = 0; i < V; i++) free(graph[i]);
        free(graph);
    }
    if (transposedGraph) {
        for (int i = 0; i < V; i++) free(transposedGraph[i]);
        free(transposedGraph);
    }
    if (visited) free(visited);
    if (stack) free(stack);
    V = 0; graph = transposedGraph = NULL; visited = stack = NULL; top = -1;
}

// ==================== ТЕСТОВЫЕ ПРИМЕРЫ ====================

void example1() {
    printf("\nПРИМЕР 1: Граф с 5 вершинами\n");
    printf("0->2, 2->1, 1->0, 0->3, 3->4\n");
    createGraph(5);
    addEdge(0, 2); addEdge(2, 1); addEdge(1, 0);
    addEdge(0, 3); addEdge(3, 4);
    printMatrix(graph, "Исходный граф");
    findSCC();
    freeMemory();
}

void example2() {
    printf("\n\nПРИМЕР 2: Граф с 8 вершинами\n");
    createGraph(8);
    addEdge(0, 1); addEdge(1, 2); addEdge(2, 3); addEdge(2, 4);
    addEdge(3, 0); addEdge(4, 5); addEdge(5, 6); addEdge(6, 4); addEdge(6, 7);
    printMatrix(graph, "Исходный граф");
    findSCC();
    freeMemory();
}

void example3() {
    printf("\n\nПРИМЕР 3: Свой граф\n");
    int vertices = safeInputInt("Введите количество вершин: ");
    if (vertices <= 0) {
        printf("Ошибка: количество вершин должно быть > 0\n");
        return;
    }

    createGraph(vertices);
    int edges = safeInputInt("Введите количество ребер: ");

    printf("Введите ребра (from to), где from и to от 0 до %d:\n", vertices - 1);
    for (int i = 0; i < edges; i++) {
        int from, to;
        char input[100];
        while (1) {
            printf("Ребро %d: ", i + 1);
            if (fgets(input, sizeof(input), stdin) != NULL) {
                if (sscanf(input, "%d %d", &from, &to) == 2 &&
                    from >= 0 && from < vertices && to >= 0 && to < vertices) {
                    addEdge(from, to);
                    break;
                }
                printf("Ошибка: введите два числа от 0 до %d\n", vertices - 1);
            }
        }
    }

    printMatrix(graph, "Ваш граф");
    findSCC();
    freeMemory();
}

// ==================== ГЛАВНОЕ МЕНЮ ====================

void displayMenu() {
    printf("\n=================================================\n");
    printf("АЛГОРИТМ КОСАРАЙЮ - Компоненты сильной связности\n");
    printf("=================================================\n");

    char input[100];
    while (1) {
        printf("\nМЕНЮ:\n");
        printf("1. Пример 1 (5 вершин)\n");
        printf("2. Пример 2 (8 вершин, 3 компоненты)\n");
        printf("3. Ввести свой граф\n");
        printf("4. Выход\n");
        printf("Выберите: ");

        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';
            if (strlen(input) != 1 || input[0] < '1' || input[0] > '4') {
                printf("Ошибка: введите число от 1 до 4\n");
                continue;
            }

            switch (input[0] - '0') {
            case 1: example1(); break;
            case 2: example2(); break;
            case 3: example3(); break;
            case 4: printf("Выход...\n"); return;
            }
        }
    }
}

// ==================== ГЛАВНАЯ ФУНКЦИЯ ====================

int main() {
    setlocale(LC_ALL, "rus");
    displayMenu();
    printf("\nНажмите Enter для выхода...");
    getchar();
    return 0;
}