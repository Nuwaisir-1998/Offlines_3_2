#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef vector<vector<int>> vvi;
typedef pair<int,int> pii;

int xx[] = {0, 1, -1, 0};
int yy[] = {1, 0, 0, -1};

void p(vvi &v){
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            cout << v[i][j] << " ";
        }
        cout << "\n";
    }
    cout << '\n';
}

int heuristic_misplaced(vvi &v, vvi &goal){
    int cnt = 0;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(v[i][j] != 0)
            if(goal[i][j] != v[i][j]){
                cnt++;
            }
        }
    }
    return cnt;
}

int heuristic_manhattan(vvi &v, vvi &goal){
    vector<pii> posGoal(16), posState(16);
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            posGoal[goal[i][j]].first = i;
            posGoal[goal[i][j]].second = j;
            posState[v[i][j]].first = i;
            posState[v[i][j]].second = j;
        }
    }
    int sum = 0;
    for(int i=1;i<=15;i++){
        sum += abs(posGoal[i].first - posState[i].first) + abs(posGoal[i].second - posState[i].second);
    }
    return sum;
}


bool solvable(vvi &v){
    vector<int> a;
    int x0;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            if(v[i][j] == 0){
                x0 = i;
            }
            if(v[i][j]) {
                a.push_back(v[i][j]);
            }
        }
    }
    int cnt = 0;
    for(int i=0;i<a.size();i++){
        for(int j=i+1;j<a.size(); j++){
            if(a[j] < a[i]) cnt++;
        }
    }
    //cout << cnt << " " << x0 << "\n";
    return (x0 % 2) != (cnt % 2);

}


void solve(vvi &init_state, vvi &goal_state, bool flag){
    int cost, xt, yt, total, xf, yf, x0, y0, check, hcost;
    map<vvi, int> mp, g;
    map<vvi, vvi> par;
    mp[init_state] = 1;
    g[init_state] = 0;
    priority_queue<pair<int, vvi>, vector<pair<int, vvi> >, greater< pair<int, vvi> > > pq;
    if(!flag)
        cost = heuristic_misplaced(init_state, goal_state);
    else cost = heuristic_manhattan(init_state, goal_state);
    if(solvable(init_state)) {
        par[init_state] ;
        pq.push({cost, init_state});
    }else {
        cout << "Not solvable\n";
        return;
    }
    int cnt = 1;
    while(true){
        vvi u = pq.top().second;
        //p(u);
        cost = pq.top().first;
        pq.pop();
        cnt++;
        if(!flag)
            check = heuristic_misplaced(u, goal_state);
        else check = heuristic_manhattan(u, goal_state);
        if(check != 0) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (u[i][j] == 0) {
                        x0 = i;
                        y0 = j;
                        break;
                    }
                }
            }
            for (int i = 0; i < 4; i++) {
                xt = x0 + xx[i];
                yt = y0 + yy[i];
                vvi temp = u;
                if (xt < 0 || yt < 0 || xt > 3 || yt > 3) continue;
                swap(temp[x0][y0], temp[xt][yt]);
                if(solvable(temp) and !mp[temp]){
                    if(!flag) hcost = heuristic_misplaced(temp, goal_state) + g[u] + 1;
                    else hcost = heuristic_manhattan(temp, goal_state) + g[u] + 1;
                    pq.push({hcost, temp});
                    mp[temp] = 1;
                    g[temp] = g[u] + 1;
                    par[temp] = u;
                }
            }
        }else{
            vector<vvi> list;
            while(u != init_state){
                //p(u);
                list.push_back(u);
                u = par[u];
            }
            reverse(list.begin(), list.end());
            p(init_state);
            for(auto m : list){
                p(m);
            }
            cout << "Cost : " << list.size() << "\n";
            break;
        }
    }
    cout << "Nodes expanded : ";
    cout << cnt << "\n";
}

void in(){
    int n, i, j, x, x0, y0;
    cin >> n;
    vvi goal_state(4), init_state(4);
    for(i=0;i<16;i++){
        cin >> x;
        goal_state[i/4].push_back(x);
    }

    n--;
    int cs = 1;
    while(n--) {
        for(i=0;i<4;i++){
            init_state[i].clear();
        }
        for (i = 0; i < 16; i++) {
            cin >> x;
            init_state[i / 4].push_back(x);
        }

        cout << "------------ Case " << cs++ << ": ------------ \n";
        cout << "Misplaced solve commented out in main code\n";
        // cout << "-> Misplaced : \n";
        // solve(init_state, goal_state, false);

        cout << "-> Manhattan : \n";
        solve(init_state, goal_state, true);

    }

}

int main() {
    ios::sync_with_stdio(false);

    freopen("in2.txt", "r", stdin);
    freopen("out", "w", stdout);
    in();

    return 0;
}