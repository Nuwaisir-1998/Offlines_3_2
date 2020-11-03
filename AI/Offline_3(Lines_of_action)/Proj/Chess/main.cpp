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
#define BLANK 2
#define SOMETHING 0
#define ALPHA 0
#define BETA 1

struct Position {
    int x, y;
    Position(int x, int y){
        this -> x = x;
        this -> y = y;
    }
    void print(){
        cout << "(" << x << "," << y << ")" << endl;
    }
};

struct Game {
    int dimension;
    int depth_limit;
    vector<int> row_total;
    vector<int> col_total;
    vector<int> diag_rl_total;  // slope = 1, starting from (0, 0), then right 
    vector<int> diag_lr_total;  // slope = -1, starting from (0, dimension - 1), then down
    vector<vector<int>> board;
    vector<Position> pos_color[2];  // pos_color[0] -> black's position
    // map<int, Position> map_id_pos[2];
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
    Game(vector<vector<int>> board){
        dimension = board.size();
        this -> board = board;
        row_total.assign(dimension, 0);
        col_total.assign(dimension, 0);
        diag_rl_total.assign(dimension * 2 - 1, 0);
        diag_lr_total.assign(dimension * 2 - 1, 0);
        make_pos_vectors();
    }

    // makes the position vectors and calculates the row_total and col_total
    void make_pos_vectors(){
        int n = dimension;
        int white_cnt = 0, black_cnt = 0;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                Position p(i, j);
                if(board[i][j] != BLANK) {
                    row_total[i]++;
                    col_total[j]++;
                    diag_rl_total[i + j]++;
                    diag_lr_total[i - j + n - 1]++;
                }
                if(board[i][j] == W) pos_color[1].push_back(p);
                else if(board[i][j] == B) pos_color[0].push_back(p);
            }
        }
    }

    // move_** functions return the end Position if the move is valid, otherwise returns {-1, -1}
    Position move_up(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_c = col_total[c];
        if(r - cnt_c >= 0 && board[r - cnt_c][c] != color){
            end_p.x = r - cnt_c;
            end_p.y = c;
            return end_p;
        }
        return end_p;
    }

    Position move_down(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_c = col_total[c];
        if(r + cnt_c < dimension && board[r + cnt_c][c] != color){
            end_p.x = r + cnt_c;
            end_p.y = c;
            return end_p;
        }
        return end_p;
    }

    Position move_left(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_r = col_total[r];
        if(c - cnt_r >= 0 && board[r][c - cnt_r] != color){
            end_p.x = r;
            end_p.y = c - cnt_r;
            return end_p;
        }
        return end_p;
    }

    Position move_right(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_r = col_total[r];
        if(c + cnt_r < dimension && board[r][c + cnt_r] != color){
            end_p.x = r;
            end_p.y = c + cnt_r;
            return end_p;
        }
        return end_p;
    }

    Position move_up_left(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_ul = diag_lr_total[r - c + dimension - 1];
        if(c - cnt_ul >= 0 && r - cnt_ul >= 0 && board[r - cnt_ul][c - cnt_ul] != color){
            end_p.x = r - cnt_ul;
            end_p.y = c - cnt_ul;
            return end_p;
        }
        return end_p;
    }

    Position move_down_right(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_ul = diag_lr_total[r - c + dimension - 1];
        if(c + cnt_ul < dimension && r + cnt_ul < dimension && board[r + cnt_ul][c + cnt_ul] != color){
            end_p.x = r + cnt_ul;
            end_p.y = c + cnt_ul;
            return end_p;
        }
        return end_p;
    }

    Position move_up_right(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt = diag_lr_total[r + c];
        if(c + cnt < dimension && r - cnt >= 0 && board[r - cnt][c + cnt] != color){
            end_p.x = r - cnt;
            end_p.y = c + cnt;
            return end_p;
        }
        return end_p;
    }

    Position move_down_left(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt = diag_lr_total[r + c];
        if(c - cnt >= 0 && r + cnt < dimension && board[r + cnt][c - cnt] != color){
            end_p.x = r + cnt;
            end_p.y = c - cnt;
            return end_p;
        }
        return end_p;
    }

    
    // generates all possible VALID moves from Position p, for the given color

    /*  I SHOULD TEST THIS IN UI  */

    vector<Position> generate_all_moves(Position p, int color){
        vector<Position> moves;
        
        Position p_end = move_up(p, color);
        if(p_end.x != -1) moves.push_back(p_end);

        p_end = move_down(p, color);
        if(p_end.x != -1) moves.push_back(p_end);

        p_end = move_left(p, color);
        if(p_end.x != -1) moves.push_back(p_end);

        p_end = move_right(p, color);
        if(p_end.x != -1) moves.push_back(p_end);

        p_end = move_up_left(p, color);
        if(p_end.x != -1) moves.push_back(p_end);

        p_end = move_up_right(p, color);
        if(p_end.x != -1) moves.push_back(p_end);

        p_end = move_down_right(p, color);
        if(p_end.x != -1) moves.push_back(p_end);

        p_end = move_down_left(p, color);
        if(p_end.x != -1) moves.push_back(p_end);
        
        return moves;

    }

    /***************************** Heuristics ********************************/
    
    int heuristic_piece_square_table(int color){
        int n = dimension;
        int sum = 0;
        for(int i=0;i<n;i++){
            int x = pos_color[color][i].x;
            int y = pos_color[color][i].y;
            sum += piece_square_table[x][y];
        }
        return sum;
    }

    /************************************** get, set **********************************************/

    char get(int i, int j) {
        return board[i][j];
    }
    void set(int i, int j, char c) {
        board[i][j] = c;
    }

    void set(int depth_limit){
        this -> depth_limit = depth_limit;
    }

    void set_pos(Position p, int val){
        board[p.x][p.y] = val;
    }




    /****************************** Minimax with alpha beta pruning ************************************/

    int backtrack(Game game, int depth, int color, int level){
        assert(depth_limit != -1);
        if(depth == game.depth_limit){
            return SOMETHING; // start from here tomorrow
        }
        int n = game.pos_color[color].size();
        int mx = -1e9;
        int mn = 1e9;
        for(int i=0;i<n;i++){
            vector<Position> end_positions = game.generate_all_moves(game.pos_color[color][i], color);
            for(auto ep : end_positions){
                vector<vector<int>> new_board = game.board;
                Position start_pos = game.pos_color[color][i];
                Position end_pos = ep;
                new_board[start_pos.x][start_pos.y] = BLANK;
                new_board[end_pos.x][end_pos.y] = color;
                Game new_game(new_board);
                int result = backtrack(new_game, depth + 1, abs(color - 1), abs(level - 1));
                if(level == ALPHA){
                    if(result <= mx) return mx;
                    mx = max(mx, result);
                }else{
                    if(result >= mn) return mn;
                    mn = min(mn, result);
                }
            }
        }
    }

    /****************************** Printer ************************************/

    void print(){
        // cout << "Dimension : " << dimension << endl;
        // cout << "Board:\n";
        // for(auto i : board){
        //     for(auto j : i){ 
        //         cout << j << " ";
        //     }
        //     cout << endl;
        // }
        // cout << "------------------\n";
        // cout << "row_total:" << endl;
        // printv(row_total);

        // cout << "col_total:" << endl;
        // printv(col_total);

        // cout << "diag_rl_total" << endl;
        // printv(diag_rl_total);

        // cout << "diag_lr_total" << endl;
        // printv(diag_lr_total);

        Position p(6, 5);
        vector<Position> v = generate_all_moves(p, 0);
        for(auto ele : v){
            ele.print();
        }
    }
};

struct Lines_of_action{


};


void solve(ll cs){
    int i, j, dimension;
    cin >> dimension;
    vector<vector<int>> board(dimension, vector<int> (dimension));
    for(i=0;i<dimension;i++) {
        for(j=0;j<dimension;j++){
            cin >> board[i][j];
        }
    }

    Game b(board);
    b.print();
    // cout << "finished 2 \n";
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
    while (tt--)
        solve(cs++);


    return 0;
}