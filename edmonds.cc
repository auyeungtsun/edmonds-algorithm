#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

struct Edge {
    int from, to, weight;
};

/**
 * @brief Implements the Chu-Liu-Edmonds algorithm to find the minimum spanning arborescence (MSA) of a directed graph.
 * 
 * @param n The number of nodes in the graph.
 * @param root The root node of the arborescence.
 * @param edges A vector of Edge structs representing the directed edges of the graph.
 * @return The total weight of the minimum spanning arborescence, or -1 if no arborescence exists.
 * 
 * The algorithm works by iteratively finding the minimum incoming edge for each node,
 * detecting cycles, contracting cycles into single nodes, and recalculating edge weights.
 * This process continues until no cycles are found, at which point the minimum spanning
 * arborescence has been found.
 * 
 * @note Time Complexity: O(VE), where V is the number of vertices and E is the number of edges.
 * @note Space Complexity: O(V + E), where V is the number of vertices and E is the number of edges.
 */
int chuLiuEdmonds(int n, int root, vector<Edge>& edges) {
    // minWeight stores the total weight of the minimum spanning tree
    // inEdge stores for each node, the index of the incoming edge with the minimum weight
    // cycle stores for each node, the id of the cycle it belongs to, or -1 if it doesn't belong to any cycle
    // visited is a helper array used in cycle detection
    // contractedEdges stores the edges after a cycle is contracted into a single node
    int minWeight = 0;
    vector<int> inEdge(n, -1), cycle(n, -1), visited(n, 0);
    vector<Edge> contractedEdges;

    while (true) {
        for (int i = 0; i < n; i++) {
            inEdge[i] = -1;
        }

        for (Edge& edge : edges) {
            if (edge.from == edge.to) continue;
            if (inEdge[edge.to] == -1 || edge.weight < edges[inEdge[edge.to]].weight) {
                inEdge[edge.to] = &edge - &edges[0];
            }
        }

        for(int i = 0; i < n; i++){
            if(i != root && inEdge[i] == -1) return -1;
        }

        int cycleCount = 0;
        fill(cycle.begin(), cycle.end(), -1);
        fill(visited.begin(), visited.end(), 0);
        visited[root] = 2;
        for (int i = 0; i < n; i++) {
            if(visited[i] == 0){
                int u = i;
                bool hasCycle = false;
                while(visited[u] != 2 && !hasCycle){
                    if(visited[u] == 1) {
                        hasCycle = true;
                        break;
                    }
                    visited[u] = 1;
                    u = edges[inEdge[u]].from;
                }
                if(hasCycle){
                    cycleCount++;
                    int cycleId = cycleCount - 1;
                    int v = u;
                    do {
                        cycle[v] = cycleId;
                        v = edges[inEdge[v]].from;
                    } while (v != u);
                }
                int u2 = i;
                while(visited[u2] != 2) {
                    visited[u2] = 2;
                    u2 = edges[inEdge[u2]].from;
                }
            }
        }

        for (int i = 0; i < n; i++) {
            if (i != root && inEdge[i] != -1) {
                minWeight += edges[inEdge[i]].weight;
            }
        }
        
        if (cycleCount == 0) {
            return minWeight;
        }

        contractedEdges.clear();
        vector<int> id(n, 0);
        int numNodes = 0;
        for (int i = 0; i < n; ++i) {
            if (cycle[i] != -1) {
                if (id[i] == 0) {
                    id[i] = ++numNodes;
                    int current = edges[inEdge[i]].from;
                    while (current != i) {
                        id[current] = numNodes;
                        current = edges[inEdge[current]].from;
                    }
                }
            }else {
                id[i] = ++numNodes;
            }
        }

        for(auto& edge : edges){
            int u = id[edge.from]-1;
            int v = id[edge.to]-1;
            if (u != v)
            {
                int w = edge.weight;
                w -= edges[inEdge[edge.to]].weight;
                contractedEdges.push_back({u, v, w});
            }
        }
        edges = contractedEdges;
        n = numNodes;
        root = id[root]-1;
    }
}

void testChuLiuEdmonds() {
    cout << "Running ChuLiuEdmonds Tests..." << endl;

    // Test Case 1: Simple tree, no cycles
    {
        cout << "  Test Case 1: Simple Tree..." << flush;
        int n = 3;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}, {0, 2, 5}};
        int expected = 15;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected );
        cout << " Passed." << endl;
    }

    // Test Case 2: Simple cycle
    {
        cout << "  Test Case 2: Simple Cycle..." << flush;
        int n = 3;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}, {1, 2, 20}, {2, 1, 5}};
        int expected = 30;
        int result = chuLiuEdmonds(n, root, edges);
         assert(result == expected);
        cout << " Passed." << endl;
    }

     // Test Case 3: Unreachable node (no incoming edges)
    {
        cout << "  Test Case 3: Unreachable Node..." << flush;
        int n = 3;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}};
        int expected = -1;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

    // Test Case 4: Root unreachable (from some node perspective)
    {
        cout << "  Test Case 4: Node with no incoming edge..." << flush;
        int n = 3;
        int root = 0;
        vector<Edge> edges = {{1, 0, 10}, {1, 2, 5}};
        int expected = -1;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }


    // Test Case 5: More Complex Cycle
    {
        cout << "  Test Case 5: More Complex Cycle..." << flush;
        int n = 4;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}, {1, 2, 10}, {2, 3, 10}, {3, 1, 10}, {0, 3, 30}};
        int expected = 30;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

    // Test Case 6: Disconnected Graph (nodes unreachable from root component)
    {
        cout << "  Test Case 6: Disconnected Graph..." << flush;
        int n = 4;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}, {2, 3, 5}}; 
        int expected = -1;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

    // Test Case 7: Single Node
    {
        cout << "  Test Case 7: Single Node..." << flush;
        int n = 1;
        int root = 0;
        vector<Edge> edges = {};
        int expected = 0;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

     // Test Case 8: Two Nodes, Path Exists
    {
        cout << "  Test Case 8: Two Nodes Path..." << flush;
        int n = 2;
        int root = 0;
        vector<Edge> edges = {{0, 1, 5}};
        int expected = 5;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

    // Test Case 9: Two Nodes, No Path
    {
        cout << "  Test Case 9: Two Nodes No Path..." << flush;
        int n = 2;
        int root = 0;
        vector<Edge> edges = {};
        int expected = -1;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

    // Test Case 10: Negative Weights, No Cycle
    {
        cout << "  Test Case 10: Negative Weights No Cycle..." << flush;
        int n = 3;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}, {1, 2, -5}, {0, 2, 8}};
        int expected = 5;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

     // Test Case 11: Negative Weights with Cycle
    {
        cout << "  Test Case 11: Negative Weights Cycle..." << flush;
        int n = 3;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}, {1, 2, 5}, {2, 1, -8}};
        int expected = 15;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected);
        cout << " Passed." << endl;
    }

     // Test Case 12: Alternative Path Chosen After Contraction
    {
        cout << "  Test Case 12: Alt Path Post-Contraction..." << flush;
        int n = 4;
        int root = 0;
        vector<Edge> edges = {{0, 1, 10}, {0, 2, 12}, {1, 2, 5}, {2, 1, 3}, {0, 3, 20}};
        vector<Edge> edges_copy = edges;
        int expected = 35;
        int result = chuLiuEdmonds(n, root, edges);
        assert(result == expected );
        cout << " Passed." << endl;
    }

    // Test Case 13: Two Disjoint Cycles
    {
        cout << "  Test Case 13: Two Disjoint Cycles..." << flush;
        int n = 5;
        int root = 4;
        vector<Edge> edges = {
            {4, 0, 10}, {0, 1, 5}, {1, 0, 6},
            {4, 2, 12}, {2, 3, 7}, {3, 2, 8},
            {4, 1, 18}, {4, 3, 22}
        };
        vector<Edge> edges_copy = edges;
        int expected = 34;
        int result = chuLiuEdmonds(n, root, edges_copy);
        assert(result == expected);
        cout << " Passed." << endl;
    }

    cout << "All test cases passed!" << endl;
}

void runChuLiuEdmondsSample() {
    int n = 5;
    int root = 0;
    vector<Edge> edges = {
        {0, 1, 4}, {0, 2, 2},
        {1, 2, 5}, {2, 3, 2},
        {3, 4, 3}, {4, 3, 1}
    };
    int result = chuLiuEdmonds(n, root, edges);
    cout << "Chu-Liu-Edmonds Sample Result: " << result << endl;
}

int main() {
    testChuLiuEdmonds();
    runChuLiuEdmondsSample();
    return 0;
}
