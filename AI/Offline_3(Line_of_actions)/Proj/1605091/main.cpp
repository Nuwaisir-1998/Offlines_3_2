#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/numeric>
#include <windows.h>
#include <time.h>

using namespace std;
using namespace __gnu_pbds;
using namespace __gnu_cxx;
using namespace std::chrono;

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
#define INF 100000000
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
#define DEPTH 4

std::ofstream ofs ("check_moves", std::ofstream::out);

int TIME_LIMIT = 2;
int BOTS_COLOR = 0;
bool time_over = false;
int lim = 10;

int COUNTER = 0;

clock_t start_t;

struct Position {
    int x, y;
    Position(){
        x = -1, y = -1;
    }
    Position(int x, int y){
        this -> x = x;
        this -> y = y;
    }
    void print(){
        cout << x << " " << y << endl;
    }
};

struct Game {
    int dimension;
    int depth_limit;
    Position move_from;
    Position move_to;
    vector<int> row_total;
    vector<int> col_total;
    vector<int> diag_rl_total;  // slope = 1, starting from (0, 0), then right 
    vector<int> diag_lr_total;  // slope = -1, starting from (0, dimension - 1), then down
    vector<vector<int>> board;
    vector<Position> pos_color[2];  // pos_color[0] -> black's position
    vector<vector<int>> cum_white;
    vector<vector<int>> cum_black;
    // map<int, Position> map_id_pos[2];
    vector<vector<int>> piece_square_table = 
    {
        {-80, -25, -20, -20, -20, -20, -25, -80},
        {-25,  10,  10,  10,  10,  10,  10,  -25},
        {-20,  10,  20,  20,  20,  20,  10,  -20},
        {-20,  10,  20,  40,  40,  20,  10,  -20},
        {-20,  10,  20,  40,  40,  20,  10,  -20},
        {-20,  10,  20,  20,  20,  20,  10,  -20},
        {-25,  10,  10,  10,  10,  10,  10,  -25},
        {-80, -25, -20, -20, -20, -20, -25, -80}
    };
    Game(vector<vector<int>> board){
        init(board);
    }

    void init(vector<vector<int>> & board){
        dimension = board.size();
        this -> board = board;
        row_total.assign(dimension, 0);
        col_total.assign(dimension, 0);
        diag_rl_total.assign(dimension * 2 - 1, 0);
        diag_lr_total.assign(dimension * 2 - 1, 0);
        if(dimension == 6){
            piece_square_table = {
                {25, -20, -20, -20, -20, -25},
                {-25,  10,  10,  10,  10, -25},
                {-20,  20,  40,  40,  20, -20},
                {-20,  20,  40,  40,  20, -20},
                {-25,  10,  10,  10,  10, -25},
                {25, -20, -20, -20, -20, -25}
            };
        }
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

    pair<Position, Position> sort_left_to_right_col(Position p1, Position p2){
        Position start, end;
        if(p1.y < p2.y) {
            start = p1;
            end = p2;
        }else {
            start = p2;
            end = p1;
        }
        return {start, end};
    }

    pair<Position, Position> sort_top_to_bottom_row(Position p1, Position p2){
        Position start, end;
        if(p1.x < p2.x) {
            start = p1;
            end = p2;
        }else {
            start = p2;
            end = p1;
        }
        return {start, end};
    }

    bool exists_row(int color, Position p1, Position p2){
        pair<Position, Position> p = sort_left_to_right_col(p1, p2);
        Position start = p.first;
        Position end = p.second;
        for(int i=start.y+1; i<end.y; i++){
            // if(i == start.y) continue;
            if(board[start.x][i] == color){
                return true;
            }
        }
        return false;

    }

    bool exists_col(int color, Position p1, Position p2){
        pair<Position, Position> p = sort_top_to_bottom_row(p1, p2);
        Position start = p.first;
        Position end = p.second;
        for(int i=start.x+1; i<end.x; i++){
            // if(i == start.x) continue;
            if(board[i][start.y] == color){
                return true;
            }
        }
        return false;
    }

    bool exists_right_diag(int color, Position p1, Position p2){
        pair<Position, Position> p = sort_left_to_right_col(p1, p2);
        Position start = p.first;
        Position end = p.second;
        for(int i=1;i<abs(start.x-end.x);i++){
            // if(i == 0) continue;
            if(board[start.x - i][start.y + i] == color){
                return true;
            }
        }
        return false;
    }

    bool exists_left_diag(int color, Position p1, Position p2){
        pair<Position, Position> p = sort_left_to_right_col(p1, p2);
        Position start = p.first;
        Position end = p.second;
        for(int i=1;i<abs(start.x-end.x);i++){
            // if(i == 0) continue;
            if(board[start.x + i][start.y + i] == color){
                return true;
            }
        }
        return false;
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
            if(exists_col(abs(1 - color), p, end_p))
                return {-1, -1};
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
            if(exists_col(abs(1 - color), p, end_p))
                return {-1, -1};
        }
        return end_p;
    }

    Position move_left(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_r = row_total[r];
        if(c - cnt_r >= 0 && board[r][c - cnt_r] != color){
            end_p.x = r;
            end_p.y = c - cnt_r;
            if(exists_row(abs(1 - color), p, end_p))
                return {-1, -1};
        }
        return end_p;
    }

    Position move_right(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt_r = row_total[r];
        if(c + cnt_r < dimension && board[r][c + cnt_r] != color){
            end_p.x = r;
            end_p.y = c + cnt_r;
            if(exists_row(abs(1 - color), p, end_p))
                return {-1, -1};
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
            if(exists_left_diag(abs(1 - color), p, end_p))
                return {-1, -1};
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
            if(exists_left_diag(abs(1 - color), p, end_p))
                return {-1, -1};
        }
        return end_p;
    }

    Position move_up_right(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt = diag_rl_total[r + c];
        if(c + cnt < dimension && r - cnt >= 0 && board[r - cnt][c + cnt] != color){
            end_p.x = r - cnt;
            end_p.y = c + cnt;
            if(exists_right_diag(abs(1 - color), p, end_p))
                return {-1, -1};
        }
        return end_p;
    }

    Position move_down_left(Position p, int color){
        Position end_p(-1, -1);
        int r = p.x;
        int c = p.y;
        int cnt = diag_rl_total[r + c];
        if(c - cnt >= 0 && r + cnt < dimension && board[r + cnt][c - cnt] != color){
            end_p.x = r + cnt;
            end_p.y = c - cnt;
            if(exists_right_diag(abs(1 - color), p, end_p))
                return {-1, -1};
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

    void make_move(Position p1, Position p2){
        board[p2.x][p2.y] = board[p1.x][p1.y];
        board[p1.x][p1.y] = BLANK;
        init(board);
    }

    int count_pieces(int color){
        int cnt = 0;
        for(int i=0;i<dimension;i++){
            for(int j=0;j<dimension;j++){
                if(board[i][j] == color) cnt++;
            }
        }
        return cnt;
    }

    int bfs(Position s){
        vector<int> dx = {1, 1, 0, -1, -1, -1, 0, 1};
        vector<int> dy = {0, -1, -1, -1, 0, 1, 1, 1};
        pair<int,int> s_new = {s.x, s.y};
        map<pair<int, int>, bool> vis;
        queue<pair<int,int>> q;
        q.push(s_new);
        vis[s_new] = true;
        int cnt = 1; // component_size
        while(!q.empty()){
            pair<int,int> u = q.front();
            q.pop();
            int r = u.ff;
            int c = u.ss;
            for(int i=0;i<8;i++){
                pair<int,int> to = {r + dx[i], c + dy[i]};
                if(r + dx[i] < dimension and r + dx[i] >= 0 and c + dy[i] >= 0 and c + dy[i] < dimension){
                    if((vis.find(to) == vis.end()) && (board[r][c] == board[r + dx[i]][c + dy[i]])){
                        q.push(to);
                        vis[to] = true;
                        cnt++;
                    }
                }
            }
        }
        return cnt;
    }

    bool is_white_winner(){
        int white_cnt = count_pieces(W);
        Position pos_w = pos_color[W][0];
        return (white_cnt == bfs(pos_w));
    }

    bool is_black_winner(){
        int black_cnt = count_pieces(B);
        Position pos_b = pos_color[B][0];
        return (black_cnt == bfs(pos_b));
    }

    /***************************** Heuristics ********************************/

    Position center_of_mass(vector<Position> & vp){
        int sumx = 0, sumy = 0;
        for(auto ele : vp){
            sumx += ele.x;
            sumy += ele.y;
        }
        Position ret(sumx / vp.size(), sumy / vp.size());
        return ret;
    }

    int dist(Position p1, Position p2){
        return max(abs(p1.x - p2.x), abs(p1.y - p2.y));
    }
    
    int calc_piece_square_table(int color){
        int n = pos_color[color].size();
        int sum = 0;
        for(int i=0;i<n;i++){
            int x = pos_color[color][i].x;
            int y = pos_color[color][i].y;
            // cout << x << "," << y << " -> " << piece_square_table[x][y] << endl;
            sum += piece_square_table[x][y];
        }
        return sum;
    }

    int heuristic_piece_square_table(int color){
        return calc_piece_square_table(color) - calc_piece_square_table(abs(1 - color));
    }

    int calc_area(int color){
        int n = pos_color[color].size();
        int mn_x = INF;
        int mn_y = INF;
        int mx_x = -1;
        int mx_y = -1;
        for(int i=0;i<n;i++){
            int x = pos_color[color][i].x;
            int y = pos_color[color][i].y;
            mn_x = min(mn_x, x);
            mn_y = min(mn_y, y);
            mx_x = max(mx_x, x);
            mx_y = max(mx_y, y);
        }
        return 64 - (mx_x - mn_x + 1) * (mx_y - mn_y + 1);
    }

    int heuristic_area(int color){
        return calc_area(color) - calc_area(abs(1 - color));
    }

    int connectedness_of(int color){
        int n = pos_color[color].size();
        vector<int> dx = {1, 1, 0, -1, -1, -1, 0, 1};
        vector<int> dy = {0, -1, -1, -1, 0, 1, 1, 1};
        int connectedness = 0;
        for(int i=0;i<n;i++){
            int x = pos_color[color][i].x;
            int y = pos_color[color][i].y;
            int val = 0;
            for(int j=0;j<8;j++){
                if(x + dx[j] < dimension and x + dx[j] >= 0 and y + dy[j] < dimension and y + dy[j] >= 0){
                    if(board[x + dx[j]][y + dy[j]] == color){
                        val++;
                    }
                }
            }
            connectedness += val;
        }
        return connectedness;
    }

    int heuristic_connectedness(int color){
        return connectedness_of(color) - connectedness_of(abs(1 - color));
    }

    int count_quad(int color){
        Position com = center_of_mass(pos_color[color]);
        int start_x = max(0, com.x - 2);
        int start_y = max(0, com.y - 2);
        int end_x = min(dimension - 2, com.x + 2);
        int end_y = min(dimension - 2, com.y + 2);
        int q_cnt = 0;
        for(int i=start_x;i<=end_x;i++){
            for(int j=start_y;j<=end_y;j++){
                // if(i < 0 or i >= dimension or j < 0 or j >= dimension) continue;
                int cnt = 0;
                if(board[i][j] == color) cnt++;
                if(board[i][j+1] == color) cnt++;
                if(board[i+1][j] == color) cnt++;
                if(board[i+1][j+1] == color) cnt++;
                if(cnt >= 3){
                    q_cnt++;
                }
            }
        }
        return q_cnt;
    }

    int heuristic_qaud_count(int color){
        return count_quad(color) - count_quad(abs(1 - color));
    }


    int utility(int color){
        int c1 = 2;
        int c2 = 10;
        int c3 = 15;
        int c4 = 115;
        int h1 = heuristic_piece_square_table(color);
        int h2 = heuristic_area(color);
        int h3 = heuristic_connectedness(color);
        int h4 = heuristic_qaud_count(color);
        int h5 = 0;
        if(color == W) {
            if(is_white_winner()){
                // ofs << "is_white\n";
                h5 = INF;
            }else if(is_black_winner()){
                h5 = -INF;
            }
        }else {
            if(is_black_winner()){
                // ofs << "is_black\n";
                h5 = INF;
            }else if(is_white_winner()){
                h5 = -INF;
            }
        }
        if(lim){
            lim--;
            ofs << c1*h1 << " " << c2*h2 << " " << c3*h3 << " " << c4*h4 << " " << h5 << endl;
        }
        return c1 * h1 + c2 * h2 + c3 * h3 + c4 * h4 + h5;
    }



    /************************************** get, set **********************************************/

    int get_ij(int i, int j) {
        return board[i][j];
    }

    pair<Position,Position> get_best_move(){
        return {move_from, move_to};
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

    // sets the next_move
    int backtrack(Game & game, int depth, int color, bool isMax, int alpha, int beta){
        if(time_over) return 0;
        if(depth == 0){
            // cout << "time_stamp" << (double)(clock() - start_t) * 1.0 / CLOCKS_PER_SEC << endl;
            if((double)(clock() - start_t) * 1.0 / CLOCKS_PER_SEC > TIME_LIMIT){
                time_over = true;
                return 0;
            }
            return game.utility(BOTS_COLOR);
        }
        int n = game.pos_color[color].size();
        int mx = -INF;
        int mn = INF;
        Position from;
        Position next_move;
        for(int i=0;i<n;i++){
            vector<Position> end_positions = game.generate_all_moves(game.pos_color[color][i], color);
            for(auto ep : end_positions){
                vector<vector<int>> new_board = game.board;
                Position start_pos = game.pos_color[color][i];
                Position end_pos = ep;
                new_board[start_pos.x][start_pos.y] = BLANK;
                new_board[end_pos.x][end_pos.y] = color;
                Game new_game(new_board);


                // new_game.print_board();
                // cout << "heu : " << new_game.heuristic_piece_square_table(color) << endl;
                
                int result = backtrack(new_game, depth - 1, abs(1 - color), !isMax, alpha, beta);
                // cout << "d : " << depth << endl;
                // start_pos.print();
                // end_pos.print();
                // cout << result << endl;
                // cout << "-----\n";

                if(isMax){
                    if(mx < result){
                        mx = result;
                        from = start_pos;
                        next_move = ep;
                    }
                    alpha = max(alpha, mx);
                    if(beta <= alpha) {
                        this -> move_from = start_pos;
                        this -> move_to = next_move;
                        // cout << "mx : " << mx << endl;
                        return mx;
                    }
                    // mx = max(mx, result);
                    
                }else{
                    if(mn > result){
                        mn = result;
                        from = start_pos;
                        next_move = ep;
                    }
                    beta = min(beta, mn);
                    if(beta <= alpha) {
                        this -> move_from = start_pos;
                        this -> move_to = next_move;
                        // cout << "mn : " << mn << endl;
                        return mn;
                    }
                }
            }
        }
        this -> move_from = from;
        this -> move_to = next_move;
        if(isMax){
            // cout << "mx : " << mx << endl;
            return mx;
        }
        else {
            // cout << "mn : " << mn << endl;
            return mn;
        }
    }

    /****************************** Printer ************************************/

    void print_best_move(){
        move_from.print();
        move_to.print();
    }

    void print_board(){
        for(int i=0;i<dimension;i++){
            for(int j=0;j<dimension;j++){
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
    }

    void print(){
        cout << "Dimension : " << dimension << endl;
        cout << "Board:\n";
        for(auto i : board){
            for(auto j : i){ 
                cout << j << " ";
            }
            cout << endl;
        }
        cout << "------------------\n";
        cout << "row_total:" << endl;
        printv(row_total);

        cout << "col_total:" << endl;
        printv(col_total);

        cout << "diag_rl_total" << endl;
        printv(diag_rl_total);

        cout << "diag_lr_total" << endl;
        printv(diag_lr_total);

        cout << "move : from -> to" << endl;
        move_from.print();
        move_to.print();

        // Position p(6, 5);
        // vector<Position> v = generate_all_moves(p, 0);
        // for(auto ele : v){
        //     ele.print();
        // }
    }
};



void solve(ll cs){
    int i, j, dimension = 8;
    int bots_color;
    cin >> dimension >> bots_color;
    BOTS_COLOR = bots_color;
    vector<vector<int>> board(dimension, vector<int> (dimension));
    for(i=0;i<dimension;i++) {
        for(j=0;j<dimension;j++){
            cin >> board[i][j];
        }
    }
    if(dimension == 6) TIME_LIMIT = 1;

    // vector<vector<int>> board = {
    //     {2, 1, 1, 1, 1, 1, 1, 2},
    //     {0, 2, 2, 2, 2, 2, 2, 0},
    //     {0, 2, 2, 2, 2, 2, 2, 0},
    //     {0, 2, 0, 2, 2, 2, 2, 0},
    //     {0, 2, 2, 2, 2, 2, 2, 0},
    //     {0, 2, 2, 2, 2, 2, 2, 0},
    //     {0, 2, 2, 2, 2, 2, 2, 0},
    //     {2, 1, 1, 1, 1, 1, 1, 2}
    // };
    int start_y, start_x, end_x, end_y;
    char ch;
    cin >> start_x >> start_y;
    cin >> end_x >> end_y;

    
    ofs << "Bots color " << bots_color << endl;
    ofs << start_x << " " << start_y << endl;
    ofs << end_x << " " << end_y << endl;
    
    start_t = clock();
    // auto start_c = high_resolution_clock::now(); 

    Position p1(start_x, start_y);
    Position p2(end_x, end_y);

    Position p_win(9, 0);
    Position p_win2(0, 9);

    Game b(board);



    // Position p(4, 7);
    // vector<Position> v = b.generate_all_moves(p, B);
    // // for(auto ele : v){
    // //     ele.print();
    // // }
    // cout << "---" << endl;
    // Position com = b.center_of_mass(b.pos_color[W]);
    // com.print();




    if(bots_color == 0){
        if(b.is_white_winner()){
            cout << 9 << " " << 0 << endl;
            cout << 9 << " " << 0 << endl;
            return;
        }
    }else{
        if(b.is_black_winner()){
            cout << 9 << " " << 0 << endl;
            cout << 9 << " " << 0 << endl;
            return;
        }
    }
    // b.make_move(p1, p2);
    pair<Position, Position> from_to;
    for(int i=1;i<7;i++){
        b.backtrack(b, i, bots_color, true, -INF, INF);
        // cout << 
        // cout << "time: " << start_t * 1.0 / CLOCKS_PER_SEC << endl;
        if(time_over) {
            ofs << "depth : " << i << endl;
            break;
        }
        from_to = b.get_best_move();
        
    }
    // b.backtrack(b, 5, W, true, -INF, INF);
    // b.make_move(b.get_best_move().first, b.get_best_move().second);
    

    b.make_move(from_to.first, from_to.second);
    if(bots_color == 0){
        if(b.is_black_winner()){
            cout << 0 << " " << 9 << endl;
            cout << 0 << " " << 9 << endl;
        }
    }else{
        if(b.is_white_winner()){
            cout << 0 << " " << 9 << endl;
            cout << 0 << " " << 9 << endl;
        }
    }
    // b.print_best_move();
    cout << from_to.first.x << " " << from_to.first.y << endl;
    cout << from_to.second.x << " " << from_to.second.y << endl;

    double elapsed = (double)(clock() - start_t) / CLOCKS_PER_SEC;
    ofs << "Time: " << elapsed << endl;
    
    // auto stop_c = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop_c - start_c);
    // ofs << "Time taken by the code: " << duration.count() / 1e6 << " seconds" << endl;
    ofs.close();
}



int main()
{
    ios::sync_with_stdio(false);
#ifndef ONLINE_JUDGE
    freopen("in", "r", stdin);
    // freopen("out", "w", stdout);
#endif // ONLINE_JUDGE

    ll tt = 1;
    // cin >> tt;
    ll cs = 1;
    while (tt--)
        solve(cs++);


    return 0;
}