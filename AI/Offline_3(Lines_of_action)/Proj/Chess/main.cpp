#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/numeric>
#include<windows.h>

using namespace std;
using namespace __gnu_pbds;
using namespace __gnu_cxx;

#define ordered_set tree<int, null_type,less<int>, rb_tree_tag,tree_order_statistics_node_update>

typedef long long ll;
typedef vector<int> vi;
typedef vector<ll> vll;
typedef pair<ll, ll> pll;
typedef vector<pll> vpll;

#define all(x) (x).begin(), (x).end()
#define MOD 1000000007
#define MOD9 998244353
#define PI acos(-1)
#define MAXN 200005
#define INF 1000000000000000000
#define nl '\n'
#define MAX(x) *max_element(all(x))
#define MIN(x) *min_element(all(x))
#define ff first
#define ss second
#define vt vector
#define FOR(n) for(i=0;i<n;i++)

template <typename T>
void printv(vector<T> &v){for (auto e : v) cout << e << ' ';cout << '\n';}
template <typename T>
void dbg(T x) {cerr << "x is " << x << '\n';}

struct custom_hash {
    // to make unordered map safe
    static uint64_t splitmix64(uint64_t x) {
        // http://xorshift.di.unimi.it/splitmix64.c
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
};

unordered_map<ll, ll, custom_hash> safe_map;
gp_hash_table<ll, ll, custom_hash> safe_hash_table;

/********************************************************************/

#define W 1
#define B 0

struct Position {
    int x, y;
    Position(int x, int y){
        this -> x = x;
        this -> y = y;
    }
};

struct Heuristics {
    vector<vector<int>> board;
    vector<Position> pos_color[2];  // pos_color[0] -> black's position
    vector<vector<int>> piece_square_table = 
    {
        {-80, -25, -20, -20, -20, -20, -25, -80},
        {-25,  10,  10,  10,  10,  10,  10,  -25},
        {-20,  10,  25,  25,  25,  25,  10,  -20},
        {-20,  10,  25,  50,  50,  25,  10,  -20},
        {-20,  10,  25,  50,  50,  25,  10,  -20},
        {-20,  10,  25,  25,  25,  25,  10,  -20},
        {-25,  10,  10,  10,  10,  10,  10,  -25},
        {-80, -25, -20, -20, -20, -20, -25, -80}
    };
    Heuristics(vector<vector<int>> board){
        this -> board = board;
        make_pos_vectors();
    }

    void make_pos_vectors(){
        int n = board.size();
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(board[i][j] == W) pos_color[1].push_back(Position(i, j));
                else if(board[i][j] == B) pos_color[0].push_back(Position(i, j));
            }
        }
    }

    /***************************** Heuristics ********************************/
    
    int piece_square_table(int color){
        int n = pos_color[color].size();
        int sum = 0;
        for(int i=0;i<n;i++){
            int x = pos_color[color][i].x;
            int y = pos_color[color][i].y;
            sum += piece_square_table[x][y];
        }
        return sum;
    }

    char get(int i, int j) {
        return board[i][j];
    }
    void set(int i, int j, char c) {
        board[i][j] = c;
    }


    /****************************** Minimax with alpha beta pruning ************************************/

    int dfs(int source, int par){

        
    }

};

struct Lines_of_action{


};


void solve(ll cs){
    int i, j, dimension;
    cin >> dimension;
    vector<vector<int>> board;
    for(i=0;i<dimension;i++) {
        for(j=0;j<dimension;j++){
            cin >> board[i][j];
        }
    }
}

int main()
{
    ios::sync_with_stdio(false);
#ifndef ONLINE_JUDGE
    freopen("in", "r", stdin);
    freopen("out", "w", stdout);
#endif // ONLINE_JUDGE

    ll tt = 1;
    // cin >> tt;
    ll cs = 1;
    // while (tt--)
    //     solve(cs++);



    return 0;
}