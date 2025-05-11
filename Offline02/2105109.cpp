#include <bits/stdc++.h>

using namespace std;
struct pair_hash
{
    template <typename T1, typename T2>
    size_t operator()(const pair<T1, T2> &p) const
    {
        return hash<T1>()(p.first) ^ (hash<T2>()(p.second) << 1);
    }
};
class Edge
{
public:
    int u, v, w;
    Edge(int u, int v, int w)
    {
        this->u = u;
        this->v = v;
        this->w = w;
    }
};
class Graph
{
private:
    int numVertices;
    int numEdges;
    vector<Edge> edges;
    vector<vector<pair<int, int>>> adj;
    unordered_map<pair<int, int>, int, pair_hash> weights;

public:
    Graph(int n, int m)
    {
        numVertices = n;
        numEdges = m;
        adj.resize(n);
    }

    void addEdge(int u, int v, int w)
    {
        u--;
        v--;
        weights[{u, v}] = w;
        weights[{v, u}] = w;
        edges.push_back(Edge(u, v, w));
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    int getNumVertices()
    {
        return numVertices;
    }
    const vector<Edge> &getEdges()
    {
        return edges;
    }
    const vector<vector<pair<int, int>>> &getAdj()
    {
        return adj;
    }

    int getNumEdges()
    {
        return numEdges;
    }

    int getWeight(int u, int v)
    {
        if (weights.find({u, v}) != weights.end())
        {
            return weights.at({u, v});
        }
        return 0;
    }
    pair<int, int> getMaxmiumEdge()
    {
        int maxWeight = -1;
        pair<int, int> maxEdge = {-1, -1};
        for (const auto &edge : edges)
        {
            if (edge.w > maxWeight)
            {
                maxWeight = edge.w;
                maxEdge = {edge.u, edge.v};
            }
        }
        return maxEdge;
    }
    pair<int, int> findMaxWeightEdge()
    {
        int maxWeight = -1;
        pair<int, int> maxEdge = {-1, -1};
        for (const auto &edge : weights)
        {
            if (edge.second > maxWeight)
            {
                maxWeight = edge.second;
                maxEdge = edge.first;
            }
        }
        return maxEdge;
    }
};
double randomProbability()
{
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    return static_cast<double>(rng()) / rng.max();
}
double RandomizedMaxCut(Graph &graph, int n)
{
    int totalCutWeight = 0;
    const auto &edges = graph.getEdges();
    for (int i = 1; i <= n; i++)
    {
        unordered_set<int> X;
        unordered_set<int> Y;
        int node = graph.getNumVertices();
        for (int j = 0; j < node; j++)
        {
            if (randomProbability() >= 0.5)
            {
                X.insert(j);
            }
            else
            {
                Y.insert(j);
            }
        }
        int cutWeight = 0;
        for (int j = 0; j < node; j++)
        {
            for (int k = j + 1; k < node; k++)
            {
                if (X.count(j) && Y.count(k))
                {
                    cutWeight += graph.getWeight(j, k);
                }
                else if (Y.count(j) && X.count(k))
                {
                    cutWeight += graph.getWeight(j, k);
                }
            }
        }
        totalCutWeight += cutWeight;
    }
    double averageCutWeight = totalCutWeight / n;
    return averageCutWeight;
}
pair<unordered_set<int>, unordered_set<int>> GreedyMaxCut(Graph &graph)
{
    unordered_set<int> X;
    unordered_set<int> Y;
    int node = graph.getNumVertices();
    pair<int, int> maxEdge = graph.findMaxWeightEdge();
    X.insert(maxEdge.first);
    Y.insert(maxEdge.second);
    for (int i = 0; i < node; i++)
    {
        if (X.count(i) || Y.count(i))
        {
            continue;
        }
        int weightX = 0;
        int weightY = 0;
        for (int j = 0; j < node; j++)
        {
            if (X.count(j))
            {
                weightX += graph.getWeight(i, j);
            }
            else if (Y.count(j))
            {
                weightY += graph.getWeight(i, j);
            }
        }
        if (weightX > weightY)
        {
            X.insert(i);
        }
        else
        {
            Y.insert(i);
        }
    }
    return {X, Y};
}

int semiGreedyMaxCut(Graph &graph, double alpha)
{
    int n = graph.getNumVertices();
    const auto &adj = graph.getAdj();
    vector<int> partitionSet(n, -1);

    const auto &edges = graph.getEdges();
    int maxWeight = -1, startU = -1, startV = -1;
    const auto &maxEdge = graph.getMaxmiumEdge();
    startU = maxEdge.first;
    startV = maxEdge.second;
    partitionSet[startU] = 0;
    partitionSet[startV] = 1;

    set<int> unassignedVertex;
    for (int i = 0; i < n; ++i)
    {
        if (partitionSet[i] == -1)
            unassignedVertex.insert(i);
    }

    while (!unassignedVertex.empty())
    {
        vector<pair<int, int>> scores;

        int wmin = INT_MAX, wmax = INT_MIN;
        for (int v : unassignedVertex)
        {
            int scoreA = 0, scoreB = 0;
            for (const auto &p : adj[v])
            {
                int vertex = p.first;
                int w = p.second;
                if (partitionSet[vertex] == 0)
                    scoreB += w;
                else if (partitionSet[vertex] == 1)
                    scoreA += w;
            }
            int val = max(scoreA, scoreB);
            scores.emplace_back(val, v);
            wmin = min(wmin, val);
            wmax = max(wmax, val);
        }

        int mu = wmin + alpha * (wmax - wmin);
        vector<int> rcl;
        for (const auto &p : scores)
        {
            int val = p.first;
            int v = p.second;
            if (val >= mu)
                rcl.push_back(v);
        }

        if (rcl.empty())
            rcl.push_back(scores[0].second);

        int chosen = rcl[rand() % rcl.size()];

        int scoreA = 0, scoreB = 0;
        for (const auto &p : adj[chosen])
        {
            int nei = p.first;
            int w = p.second;
            if (partitionSet[nei] == 0)
                scoreB += w;
            else if (partitionSet[nei] == 1)
                scoreA += w;
        }
        partitionSet[chosen] = (scoreA > scoreB) ? 0 : 1;
        unassignedVertex.erase(chosen);
    }

    int cutWeight = 0;
    for (const auto &e : edges)
    {
        if (partitionSet[e.u] != partitionSet[e.v])
            cutWeight += e.w;
    }
    return cutWeight;
}
int calculateCutWeight(Graph &graph, unordered_set<int> &X, unordered_set<int> &Y)
{
    int cutWeight = 0;
    for (auto x : X)
    {
        for (auto y : Y)
        {
            cutWeight += graph.getWeight(x, y);
        }
    }
    return cutWeight;
}
pair<int, int> LocalSearch(Graph &graph, vector<int> &partitionSet)
{
    const auto &adj = graph.getAdj();
    int n = graph.getNumVertices();
    bool isImproved;
    int iterationCount = 0;

    do
    {
        isImproved = false;
        for (int v = 0; v < n; ++v)
        {
            int gain = 0;
            for (const auto &p : adj[v])
            {
                int vertex = p.first, w = p.second;
                if (partitionSet[v] == partitionSet[vertex])
                    gain += w;
                else
                    gain -= w;
            }
            if (gain > 0)
            {
                partitionSet[v] ^= 1;
                isImproved = true;
            }
        }
        if (isImproved)
            iterationCount++;
    } while (isImproved);

    int cutWeight = 0;
    const auto &edges = graph.getEdges();
    for (const auto &e : edges)
    {
        if (partitionSet[e.u] != partitionSet[e.v])
            cutWeight += e.w;
    }

    return {cutWeight, iterationCount};
}
pair<int,int> graspMaxCut(Graph &graph, double alpha, int iterations)
{
    int bestCut = 0;
    vector<int> bestPartition;
    int totalLocalIterations = 0;
    for (int i = 0; i < iterations; i++)
    {
        vector<int> partitionSet(graph.getNumVertices(), -1);
        const auto &edges = graph.getEdges();
        int maxWeight = -1, startU = -1, startV = -1;
        const auto &maxEdge = graph.getMaxmiumEdge();
        startU = maxEdge.first;
        startV = maxEdge.second;
        partitionSet[startU] = 0;
        partitionSet[startV] = 1;

        set<int> unassignedVertex;
        for (int i = 0; i < graph.getNumVertices(); ++i)
        {
            if (partitionSet[i] == -1)
                unassignedVertex.insert(i);
        }

        while (!unassignedVertex.empty())
        {
            vector<pair<int, int>> scores;
            int wmin = INT_MAX, wmax = INT_MIN;

            for (int v : unassignedVertex)
            {
                int scoreA = 0, scoreB = 0;
                for (const auto &p : graph.getAdj()[v])
                {
                    int nei = p.first;
                    int w = p.second;
                    if (partitionSet[nei] == 0)
                        scoreB += w;
                    else if (partitionSet[nei] == 1)
                        scoreA += w;
                }
                int val = max(scoreA, scoreB);
                scores.emplace_back(val, v);
                wmin = min(wmin, val);
                wmax = max(wmax, val);
            }

            int mu = wmin + alpha * (wmax - wmin);
            vector<int> rcl;
            for (const auto &[val, v] : scores)
            {
                if (val >= mu)
                    rcl.push_back(v);
            }

            if (rcl.empty())
                rcl.push_back(scores[0].second);

            int chosen = rcl[rand() % rcl.size()];

            int scoreA = 0, scoreB = 0;
            for (const auto &p : graph.getAdj()[chosen])
            {
                int nei = p.first;
                int w = p.second;
                if (partitionSet[nei] == 0)
                    scoreB += w;
                else if (partitionSet[nei] == 1)
                    scoreA += w;
            }
            partitionSet[chosen] = (scoreA > scoreB) ? 0 : 1;
            unassignedVertex.erase(chosen);
        }
        pair<int, int> localResult = LocalSearch(graph, partitionSet);
        int cutWeight = localResult.first;
        int localIterations = localResult.second;
        totalLocalIterations += localIterations;
        if (cutWeight > bestCut)
        {
            bestCut = cutWeight;
            bestPartition = partitionSet;
        }
    }
    totalLocalIterations /= iterations;
    return {bestCut, totalLocalIterations};
}
Graph readGraph(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    int n, m;
    file >> n >> m;
    Graph graph(n, m);

    for (int i = 0; i < m; i++)
    {
        int u, v, w;
        file >> u >> v >> w;
        graph.addEdge(u, v, w);
    }
    cout << "Graph read successfully: " << filename << endl;
    cout << "Number of vertices: " << n << ", Number of edges: " << m << endl;

    file.close();
    return graph;
}
int main()
{
    vector<string> graphFiles;
    for (int i = 1; i <= 54; i++)
    {
        graphFiles.push_back("set1/g" + to_string(i) + ".rud");
    }

    int randomizedRuns = 30;
    int localIterations = 5;
    int graspIterations = 10;
    double alpha = 0.7;
    string studentID = "2105109"; 

    ofstream csv(studentID + ".csv");
    csv << "Name,|V|,|E|,Randomized-1,Greedy-1,Semi-greedy-1 (α=" << alpha
        << "),Local-1 Iter,Local-1 Avg,GRASP-1 Iter,GRASP-1 Best,Known Best\n";

    unordered_map<string, int> knownBest = {
        {"G1", 12078}, {"G22", 14123}, {"G43", 7027}, {"G2", 12084}, {"G3", 12077}, {"G14", 3187}, {"G15", 3169}, {"G16", 3172},
        {"G11", 627}, {"G12", 621}, {"G13", 645}, {"G23", 14129}, {"G24", 14131}, {"G32", 1560}, {"G33", 1537}, {"G34", 1541},
        {"G35", 8000}, {"G36", 7996}, {"G37", 8009}, {"G44", 7022}, {"G45", 7020}, {"G48", 6000}, {"G49", 6000}, {"G50", 5988}
    };

    for (const auto &filename : graphFiles)
    {
        Graph g = readGraph(filename);
        int n = g.getNumVertices(), m = g.getNumEdges();
        string graphName = filename.substr(filename.find_last_of('/') + 1);
        graphName = graphName.substr(0, graphName.find('.')); 
        graphName[0] = toupper(graphName[0]); 

        cout << "Processing " << graphName << "..." << endl;

        // Randomized
        double randomizedAvg = RandomizedMaxCut(g, randomizedRuns);

        // Greedy
        auto [gx, gy] = GreedyMaxCut(g);
        int greedyCut = calculateCutWeight(g, gx, gy);

        // Semi-Greedy
        int semiGreedyCut = semiGreedyMaxCut(g, alpha);

        // Local Search average
        int localCutSum = 0;
        for (int i = 0; i < localIterations; i++) {
            vector<int> tempPartition(n, -1);
            auto [su, sv] = g.getMaxmiumEdge();
            tempPartition[su] = 0;
            tempPartition[sv] = 1;

            set<int> unassignedVertex;
            for (int j = 0; j < n; j++)
                if (tempPartition[j] == -1)
                    unassignedVertex.insert(j);
            for (int v : unassignedVertex)
                tempPartition[v] = rand() % 2;

            auto [cut, _] =LocalSearch(g, tempPartition);
            localCutSum += cut;
        }
        int localCutAvg = localCutSum / localIterations;

        // GRASP
        auto [graspCut, graspLocalAvg] = graspMaxCut(g, alpha, graspIterations);

       
        string knownBestStr = "-";
        if (knownBest.count(graphName)) {
            knownBestStr = to_string(knownBest[graphName]);
        }

        
        csv << graphName << "," << n << "," << m << ","
            << randomizedAvg << "," << greedyCut << "," << semiGreedyCut << ","
            << localIterations << "," << localCutAvg << ","
            << graspIterations << "," << graspCut << "," << knownBestStr << "\n";

        cout << "Done with " << graphName << endl;
    }

    csv.close();
    cout << "Results saved to " << studentID << ".csv" << endl;
    return 0;
}
