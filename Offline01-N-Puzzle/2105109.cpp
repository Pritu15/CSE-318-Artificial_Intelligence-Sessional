#include "2105109_Heuristics.h"
using namespace std;
map<vector<vector<int>>, bool> visited;

class State
{
    vector<vector<int>> Board;
    int priority;
    int moves;
    State *parent;
public:
    State(vector<vector<int>> Board, int priority, int moves, State *parent = nullptr)
    {
        this->Board = Board;
        this->priority = priority;
        this->moves = moves;
        this->parent = parent;
    }
    vector<vector<int>>& getBoard()
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
    void setNext(State *next)
    {
        this->parent = next;
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
void PrintBoard(const vector<vector<int>> &Board)
{
    for (const auto &row : Board)
    {
        for (const auto &val : row)
        {
            cout << val << " ";
        }
        cout << endl;
    }
    cout << endl;
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
    priority_queue<pair<int, State *>, vector<pair<int, State *>>, greater<pair<int, State *>>> pq;
    pq.push({0, start});
    visited[start->getBoard()] = true;

    while (!pq.empty())
    {
        auto current = pq.top();
        pq.pop();
        State *currentState = current.second;
        // PrintBoard(currentState->getBoard());

        if (currentState->getBoard() == goal->getBoard())
        {
            cout << "Minimum number of moves = " << currentState->getMoves() << "\n\n";
            Print(currentState);
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
                    int newPriority = currentState->getMoves() + Manhatten_Distance(newBoard); // Update priority (can include heuristic)
                    State *newState = new State(newBoard, newPriority, currentState->getMoves() + 1, currentState);
                    // PrintBoard(newState->getParent()->getBoard());  
                    pq.push({newPriority, newState});
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



int main()
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

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
    if (isSolvable(k, Board_Input))
    {
        // cout << "The puzzle is solvable.\n";
        State *start = new State(Board_Input, 0, 0);
        State *goal = new State(Final_Board, 0, 0);
        a_Star(start, goal);
        // Print(start);
    }
    else
    {
        cout << "The puzzle is not solvable.\n";
    }

    return 0;
}