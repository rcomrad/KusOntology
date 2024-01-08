#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int main()
{
    freopen("bfs.in", "r", stdin);
    freopen("bfs.out", "w", stdout);

    int n, start, finish;
    cin >> n >> start >> finish;

    --start;
    --finish;

    vector<vector<int>> gr(n);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            int t;
            cin >> t;
            if (t == 1)
            {
                gr[i].emplace_back(j);
            }
        }
    }

    vector<int> dist(n, 1e9);
    dist[start] = 0;

    queue<int> q;
    q.push(start);
    while (!q.empty())
    {
        int cur = q.front();
        q.pop();

        for (int i = 0; i < gr[cur].size(); ++i)
        {
            int newNode = gr[cur][i];
            if (dist[cur] + 1 < dist[newNode])
            {
                q.push(newNode);
                dist[newNode] = dist[cur] + 1;
            }
        }
    }

    if (dist[finish] != 1e9) cout << dist[finish];
    else cout << 0;
}