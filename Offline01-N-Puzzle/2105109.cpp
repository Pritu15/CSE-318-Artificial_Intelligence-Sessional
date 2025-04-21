#include<bits/stdc++.h>
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

int Euclidean_Distance(vector<vector<int>> &Board,int n)
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
    return count;
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
            }
        }
    }
    return count;
}

void solve_N_Puzzle(vector<vector<int>> &Board, int n)
{
    
}

int main()
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    int n;
    cin>>n;
    vector<vector<int>> Board(n, vector<int>(n));
    for(int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> Board[i][j];
        }
    }
    
    return 0;
}