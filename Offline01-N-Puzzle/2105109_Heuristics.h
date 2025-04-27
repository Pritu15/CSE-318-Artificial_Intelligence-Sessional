#include <bits/stdc++.h>
using namespace std;
float Hamming_Distance(vector<vector<int>> &Board)
{
    int count = 0;
    int n = Board.size();
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
float Manhatten_Distance(vector<vector<int>> &Board)
{
    int count = 0;
    int n = Board.size();

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





float Euclidean_Distance(vector<vector<int>> &Board)
{
    float count = 0;
    int n = Board.size();
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

float Linear_Conflict(vector<vector<int>> &Board)
{
    int count = 0;
    int n = Board.size();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (Board[i][j] != 0)
            {
                int x = Board[i][j] - 1;
                if (i == x / n) 
                {
                    for (int k = j + 1; k < n; k++)
                    {
                        if (Board[i][k] != 0)
                        {
                            int y = Board[i][k] - 1;
                            if (i == y / n && x % n > y % n) 
                            {
                                count += 1; 
                            }
                        }
                    }
                }
                else if (j == x % n) 
                {
                    for (int k = i + 1; k < n; k++)
                    {
                        if (Board[k][j] != 0)
                        {
                            int y = Board[k][j] - 1;
                            if (j == y % n && x / n > y / n) 
                            {
                                count += 1; 
                            }
                        }
                    }
                }
            }
        }
    }
    return (2*count)+Manhatten_Distance(Board);
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
                // cout << arr[i] << " " << arr[j] << endl;
            }
        }
    }
    return count;
}