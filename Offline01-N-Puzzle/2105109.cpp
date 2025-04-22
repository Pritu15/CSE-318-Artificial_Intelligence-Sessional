#include <bits/stdc++.h>
using namespace std;

int Hamming_Distance(vector<vector<int>> &Board, int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (Board[i][j] != 0 && Board[i][j] != (i * n + j + 1))
            {
                count++;
            }
        }
    }
    return count;
}
int Manhatten_Distance(vector<vector<int>> &Board, int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (Board[i][j] != 0)
            {
                int x = Board[i][j] - 1;
                count += abs(i - x / n) + abs(j - x % n);
            }
        }
    }
    return count;
}

int Euclidean_Distance(vector<vector<int>> &Board, int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (Board[i][j] != 0)
            {
                int x = Board[i][j] - 1;
                count += sqrt(pow(i - x / n, 2) + pow(j - x % n, 2));
            }
        }
    }
    return count;
}

int Linear_Conflict(vector<vector<int>> &Board, int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (Board[i][j] != 0)
            {
                int x = Board[i][j] - 1;
                if (i == x / n)
                {
                    count += abs(j - x % n);
                }
            }
        }
    }
    return count + Manhatten_Distance(Board, n);
}
int Inversion_Count(vector<vector<int>> &Board, int n)
{
    int count = 0;
    vector<int> arr;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (Board[i][j] != 0)
            {
                arr.push_back(Board[i][j]);
            }
        }
    }
    for (int i = 0; i < arr.size(); i++)
    {
        for (int j = i + 1; j < arr.size(); j++)
        {
            if (arr[i] > arr[j])
            {
                count++;
                cout<<arr[i]<<" "<<arr[j]<<endl;
            }
        }
    }
    return count;
}

void solve_N_Puzzle(vector<vector<int>> &Board, int n)
{
}
bool isOdd(int n)
{
    return n % 2 != 0;
}
bool isEven(int n)
{
    return n % 2 == 0;
}
/*
0->3
1->2
2->1
*/

int main()
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    int n;
    cin >> n;
    int blank_row;
    vector<vector<int>> Board(n, vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> Board[i][j];
            if (Board[i][j] == 0)
            {
                blank_row = n - i;
            }
        }
    }
    if (isOdd(n))
    {
        if (isOdd(Inversion_Count(Board, n)))
            cout << "Unsolvable puzzle\n";
        else
            solve_N_Puzzle(Board, n);
    }
    else
    {
        bool firs_Condition = isEven(blank_row) && isOdd(Inversion_Count(Board, n));
        bool second_Condition = isOdd(blank_row) && isEven(Inversion_Count(Board, n));
        if (firs_Condition || second_Condition)
        {
            solve_N_Puzzle(Board, n);
        }
        else
        {
            cout << "Unsolvable puzzle\n";
        }
    }

    return 0;
}