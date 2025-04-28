#include "2105109_Heuristics.h"
using namespace std;
map<vector<vector<int>>, bool> visited;
int noOfExploredStates = 0;
int noOfExpandedStates = 0;
typedef float (*HeuristicFunction)(vector<vector<int>> &);
HeuristicFunction hieuristics = nullptr;

class State
{
public:
    vector<vector<int>> Board;
    int priority;
    int hieuristic;
    int moves;
    State *parent;
    bool visted = false;

    State(vector<vector<int>> Board, int priority, int hieuristic, int moves, State *parent = nullptr)
    {
        this->Board = Board;
        this->priority = priority;
        this->hieuristic = hieuristic;
        this->moves = moves;
        this->parent = parent;
    }
    vector<vector<int>> &getBoard()
    {
        return Board;
    }
    int getPriority()
    {
        return priority;
    }
    int getMoves()
    {
        return moves;
    }
    State *getParent()
    {
        return parent;
    }
};
pair<int, int> getBlankTilePosition(vector<vector<int>> &Board)
{
    for (int i = 0; i < Board.size(); i++)
    {
        for (int j = 0; j < Board[i].size(); j++)
        {
            if (Board[i][j] == 0)
            {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}
struct CompareState
{
    bool operator()(const State *a, const State *b)
    {
        if (a->priority == b->priority)
        {
            return a->hieuristic > b->hieuristic;
        }
        return a->priority > b->priority;
    }
};
void FreeMemory(State *state)
{
    if (state == nullptr)
        return;
    FreeMemory(state->getParent());
    delete state;
}
void FreePriorityQueue(priority_queue<State *, vector<State *>, CompareState> &pq)
{
    while (!pq.empty())
    {
        State *state = pq.top();
        pq.pop();
        delete state;
    }
}

void Print(State *goal)
{
    State *current = goal;
    vector<vector<vector<int>>> path;
    while (current != nullptr)
    {
        path.push_back(current->getBoard());
        current = current->getParent();
    }
    reverse(path.begin(), path.end());
    for (const auto &board : path)
    {
        for (const auto &row : board)
        {
            for (const auto &val : row)
            {
                cout << val << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

void a_Star(State *start, State *goal)
{
    priority_queue<State *, vector<State *>, CompareState> pq;
    pq.push(start);
    noOfExploredStates++;
    visited[start->getBoard()] = true;

    while (!pq.empty())
    {
        auto current = pq.top();
        noOfExpandedStates++;
        pq.pop();
        State *currentState = current;
        // PrintBoard(currentState->getBoard());

        if (currentState->getBoard() == goal->getBoard())
        {
            cout << "Minimum number of moves = " << currentState->getMoves() << "\n\n";
            Print(currentState);
            FreeMemory(currentState);
            FreePriorityQueue(pq);
            delete goal;
            return;
        }

        vector<vector<int>> currentBoard = currentState->getBoard();
        int n = currentBoard.size();
        int blank_x, blank_y;

        blank_x = getBlankTilePosition(currentBoard).first;
        blank_y = getBlankTilePosition(currentBoard).second;

        vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        for (auto dir : directions)
        {
            int new_x = blank_x + dir.first;
            int new_y = blank_y + dir.second;

            if (new_x >= 0 && new_x < n && new_y >= 0 && new_y < n)
            {
                vector<vector<int>> newBoard = currentBoard;
                swap(newBoard[blank_x][blank_y], newBoard[new_x][new_y]);

                if (!visited[newBoard])
                {
                    int newHieuristic = hieuristics(newBoard);

                    int newPriority = currentState->getMoves() + newHieuristic; // Update priority (can include heuristic)
                    State *newState = new State(newBoard, newPriority, newHieuristic, currentState->getMoves() + 1, currentState);

                    pq.push(newState);
                    noOfExploredStates++;
                    visited[newBoard] = true;
                }
            }
        }
    }
}

bool isSolvable(int k, vector<vector<int>> &Board_Input)
{
    int blank_row = getBlankTilePosition(Board_Input).first;
    if (k % 2 == 1)
    {
        return Inversion_Count(Board_Input, k) % 2 == 0;
    }
    else
    {
        bool first_Condition = (blank_row % 2 == 0) && (Inversion_Count(Board_Input, k) % 2 == 1);
        bool second_Condition = (blank_row % 2 == 1) && (Inversion_Count(Board_Input, k) % 2 == 0);
        return (first_Condition || second_Condition);
    }
}

int main(int argc, char *argv[])
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    hieuristics = Manhatten_Distance;

    int n, k;
    cin >> k;
    n = k * k;
    vector<vector<int>> Board_Input(k, vector<int>(k));
    vector<vector<int>> Final_Board(k, vector<int>(k));

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            cin >> Board_Input[i][j];
            Final_Board[i][j] = i * k + j + 1;
        }
    }
    Final_Board[k - 1][k - 1] = 0;
    // cout << "Euclidean Distance Heuristic:" << Euclidean_Distance(Board_Input) << "\n";
    // cout << "Manhatten Distance Heuristic:" << Manhatten_Distance(Board_Input) << "\n";
    // cout << "Hamming Distance Heuristic:" << Hamming_Distance(Board_Input) << "\n";
    // cout << "Linear Conflict Heuristic:" << Linear_Conflict(Board_Input) << "\n";
    // cout << "Inversion Count Heuristic:" << Inversion_Count(Board_Input, k) << "\n";
    if (isSolvable(k, Board_Input))
    {

        if (argv[1] == "Hamming")
        {
            hieuristics = Hamming_Distance;
        }
        else if (argv[1] == "Manhatten")
        {
            hieuristics = Manhatten_Distance;
        }
        else if (argv[1] == "Linear")
        {
            hieuristics = Linear_Conflict;
        }
        else if (argv[1] == "Euclidean")
        {
            hieuristics = Euclidean_Distance;
        }

        State *start = new State(Board_Input, 0, 0, 0);
        State *goal = new State(Final_Board, 0, 0, 0);
        a_Star(start, goal);
        cout << "No of explored states: " << noOfExploredStates << "\n";
        cout << "No of expanded states: " << noOfExpandedStates << "\n";
        // Print(start);
    }

    else
    {
        cout << "Unsolvable puzzle\n";
    }

    return 0;
}

/*
g++ -fsanitize=address -g 2105109.cpp -o 2105109
./2105109  Hamming
./2105109  Linear
./2105109  Manhatten
./2105109  Euclidean
*/