#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/numeric>

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
#define INF 1000000000
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
void printst(set<T> &st){for (auto e : st) cout << e << ' ';cout << '\n';}
template <typename T>
void dbg(T x) {cerr << "x is " << x << '\n';}

/********************     Function Prototypes    ********************/

void chin(int n);               // reads n characters form input stream
vector<vector<int>> input();    // takes the main input

/********************************************************************/

class Vertex{
public:
    int id;
    int val;
    int pos_x, pos_y;
    set<int> dom;

    Vertex(int id, int val, int posX, int posY, const set<int> &dom) : id(id), val(val), pos_x(posX), pos_y(posY),
                                                                          dom(dom) {}

    void print(){
        cout << id << " " << pos_x << " " << pos_y << " " << val << endl;
        cout << "dom : ";
        for(auto ele : dom){
            cout << ele << ' ';
        }
        cout << endl;
    }

    void setDom(const set<int> &dom) {
        Vertex::dom = dom;
    }

};

class Graph{
public:
    int V;
    int node_cnt = 0;
    int cnt_backtrack = 0;
    vector<set<int>> adj;
    vector<vector<int>> board;
    vector<Vertex> vertices;
    vector<set<int>> row_nums;
    vector<set<int>> col_nums;
    set<int> blanks;
    int init_blanks_cnt;

    Graph(vector<vector<int>> & board){
        this->board = board;
        init();
    }

    void init(){
        int rows = board.size();
        row_nums.resize(rows);
        col_nums.resize(rows);
        set<int> dom;
//        for(int i=1;i<=rows;i++) dom.insert(i);
        for(int i=0;i<rows;i++){
            for(int j=0;j<rows;j++){
//                dom.erase(board[i][j]);
                Vertex vertex(rows * i + j, board[i][j], i, j, dom);
//                pq_sdf.push({dom.size(), rows * i + j});
//                if(board[i][j] != 0)
//                    dom.insert(board[i][j]);
                vertices.push_back(vertex);
                if(board[i][j] == 0){
                    blanks.insert(rows * i + j);
                }else{
                    row_nums[i].insert(board[i][j]);
                    col_nums[j].insert(board[i][j]);
                }
                int at = rows * i + j;
                init_blanks_cnt = blanks.size();
                adj.resize(rows * rows);
                // making horizontal edges
                for(int k=0;k<rows;k++){
                    int to = rows * i + k;
                    if(at != to) {
                        adj[at].insert(to);
                        adj[to].insert(at);
                    }
                }

                // making vertical edges
                for(int k=0;k<rows;k++){
                    int to = k * rows + j;
                    if(at != to) {
                        adj[at].insert(to);
                        adj[to].insert(at);
                    }
                }
            }
        }
        calculate_domains();
//        print();
        V = vertices.size();
    }

    int get_vertex_id(int r, int c){
        int rows = board.size();
        return (r * rows + c);
    }


    void calculate_domains(){
        int rows = board.size();
        for(int ele=0;ele<rows*rows;ele++){
            int val = vertices[ele].val;
//            Vertex vertex = vertices[ele];
//            cout << ele << "|" << val << endl;
            if(val != 0){
//                map_vertex_sdf[ele] = 1;
                set<int> temp{val};
                vertices[ele].setDom(temp);
                continue;
            }
            assert(val == 0);
            int r = vertices[ele].pos_x;
            int c = vertices[ele].pos_y;
//            int sz = 0;
            for(int i=1;i<=rows;i++){
                if(row_nums[r].find(i) == row_nums[r].end() and col_nums[c].find(i) == col_nums[c].end()){
                    vertices[ele].dom.insert(i);
//                    sz++;
                }
            }
//            vertices[ele] = vertex;
//            map_vertex_sdf[ele] = sz;
        }
    }


    int get_next_sdf(){
        int mn = INF;
        int idx = -1;
        for(auto ele : blanks){
            Vertex vertex = vertices[ele];
            if((int)vertex.dom.size() < mn){
                mn = vertex.dom.size();
                idx = vertex.id;
            }
        }
        return idx;
    }

    int get_next_max_dynamic_degree(){
        int mn = INF;
        int idx = -1;
        for(auto ele : blanks){
            Vertex vertex = vertices[ele];
            int tot = (int)row_nums[vertex.pos_x].size() + (int)col_nums[vertex.pos_y].size();
            if(tot < mn){
                mn = tot;
                idx = vertex.id;
            }
        }
        return idx;
    }

    int get_next_no_algo(){
        if(blanks.empty()) return -1;
        auto itr = blanks.begin();
        return *itr;
    }

    bool backtrack(int s){
        node_cnt++;
        Vertex s_v = vertices[s];
        assert(s_v.val == 0);
        int cur_row = s_v.pos_x;
        int cur_col = s_v.pos_y;
        int rows = board.size();
        bool possible = false;
        blanks.erase(s);

        set<int> dom;

        for(int i=1;i<=rows;i++){
            if(row_nums[cur_row].find(i) == row_nums[cur_row].end() and col_nums[cur_col].find(i) == col_nums[cur_col].end()){
                dom.insert(i);
            }
        }

        for(auto i : dom){
                // possible
                possible = true;
                board[cur_row][cur_col] = i;
                row_nums[cur_row].insert(i);
                col_nums[cur_col].insert(i);
                int next = get_next_no_algo();
                if(next == -1) {
                    cout << "no blanks any more\n";
                    for(auto el : board){
                        printv(el);
                    }
                    cout << "Board seems " << checker() << endl;
                    return true;
                }

                if(backtrack(next)){
                    return true;
                }else{
                    cnt_backtrack++;
                }
                board[cur_row][cur_col] = 0;
                row_nums[cur_row].erase(i);
                col_nums[cur_col].erase(i);
        }
        blanks.insert(s);
        return false;
    }

    bool backtrack_fc(int s){
        node_cnt++;
        Vertex s_v = vertices[s];
        assert(s_v.val == 0);
        int cur_row = s_v.pos_x;
        int cur_col = s_v.pos_y;
        int rows = board.size();

        blanks.erase(s);
        for(auto i : s_v.dom){
            board[cur_row][cur_col] = i;
            vertices[s].val = i;
            row_nums[cur_row].insert(i);
            col_nums[cur_col].insert(i);
            bool impos = false;
            set<int> prev_dom = vertices[s].dom;
            set<int> new_dom{vertices[s].val};
            vertices[s].setDom(new_dom);
            set<int> needs_restoration;
            for(int j=0;j<rows;j++) {
                int v1 = get_vertex_id(cur_row, j);
                int v2 = get_vertex_id(j, cur_col);
                if (board[cur_row][j] == 0) {
                    if(vertices[v1].dom.erase(i))
                        needs_restoration.insert(v1);
                }
                if (board[j][cur_col] == 0) {
                    if(vertices[v2].dom.erase(i))
                        needs_restoration.insert(v2);
                }
                if (vertices[v1].dom.empty() or vertices[v2].dom.empty()) {
                    impos = true;
                }
            }

            int next = get_next_sdf();
            if(next == -1) {
                cout << "Nodes visited : " << node_cnt << endl;
                cout << "no blanks any more\n";
                for(auto el : board){
                    printv(el);
                }
                cout << "Board seems " << checker() << endl;
                return true;
            }

            if(!impos) {
                if(backtrack_fc(next)){
                    return true;
                }else{
                    cnt_backtrack++;
                }
            }
            board[cur_row][cur_col] = 0;
            vertices[s].val = 0;
            row_nums[cur_row].erase(i);
            col_nums[cur_col].erase(i);
            vertices[s].setDom(prev_dom);
            for(auto ele : needs_restoration){
                vertices[ele].dom.insert(i);
            }
        }

        blanks.insert(s);
        return false;
    }
    bool g_f = true;
    bool AC3(int x){
        queue<pair<int,int>> q;
        int rows = board.size();

        set<int> adj_blanks[rows*rows];

        // building the queue
        for(int i=0;i<rows*rows;i++){
            int r = vertices[i].pos_x;
            int c = vertices[i].pos_y;
            for(int j=0;j<rows;j++){
                int v1 = get_vertex_id(r, j);
                int v2 = get_vertex_id(j, c);
                if(v1 != i){
                    q.push({i, v1});
                    adj_blanks[i].insert(v1);
                }
                if(v2 != i) {
                    q.push({i, v2});
                    adj_blanks[i].insert(v2);
                }
            }
        }


        while(!q.empty()){
            pair<int,int> u = q.front();
//            if(x == 2 and vertices[u.ff].id == 2 and vertices[u.ff].val == 9 and g_f) {
////                cout << "u: " << u.ff << " " << u.ss << endl;
//                cout << "ub:\n";
//                cout << vertices[u.ff].id << " " << vertices[u.ff].val << endl;
//                g_f = false;
//                for(auto ele : board){
//                    printv(ele);
//                }
//            }
            q.pop();
            if(Revise(u)){
                int xi = u.ff;
                int xj = u.ss;
                if(vertices[xi].dom.empty()) {
//                    cout << xi << " " << " false\n";
                    return false;
                }
                for(auto neighbor : adj_blanks[xi]){
                    if(neighbor == xj) continue;
                    q.push({neighbor, xi});
                }
            }
        }
//        cout << "printing from ac-3\n";
//        print();
        return true;
    }

    bool Revise(pair<int,int> u){
        set<int> Di = vertices[u.ff].dom;
        set<int> Dj = vertices[u.ss].dom;
        if(u.ff == 17){
//            cout << "Di\n";
//            printst(Di);
//            cout << "Dj of " << u.ss << endl;
//            printst(Dj);
        }
        bool revised = false;
        set<int> to_be_deleted;
        for(auto x : Di){
            bool flag = false;
            for(auto y : Dj){
                if(x != y) flag = true;
            }
            if(!flag){
                to_be_deleted.insert(x);
                revised = true;
            }
        }
        for(auto ele : to_be_deleted){
            vertices[u.ff].dom.erase(ele);
        }
        return revised;
    }

    bool backtrack_mac(int s){
        cout << s << endl;
        node_cnt++;

        Vertex s_v = vertices[s];
        assert(s_v.val == 0);
        int cur_row = vertices[s].pos_x;
        int cur_col = vertices[s].pos_y;
        int rows = board.size();
        blanks.erase(s);
        vector<Vertex> saved_vertices;
        for(int i=0;i<vertices.size();i++){
            saved_vertices.push_back(vertices[i]);
        }

        for(auto i : s_v.dom){
//            if(vertices[s].dom.find(i) == vertices[s].dom.end()) continue;
            board[cur_row][cur_col] = i;
            vertices[s].val = i;
            set<int> prev_dom = vertices[s].dom;
            set<int> new_dom{i};
            vertices[s].setDom(new_dom);
            set<int> needs_restoration;
            bool impos = false;

//            cout << s << " color: " << i << endl;
//            print();

            bool ac3_result = AC3(s);
//            print();
            if(!ac3_result) {
                impos = true;
//                cout << "Impossible\n";
            }

            int next = get_next_sdf();
            if(next == -1) {
                cout << "Nodes visited : " << node_cnt << endl;
                cout << "no blanks any more\n";
                for(auto el : board){
                    printv(el);
                }
                cout << "Board seems " << checker() << endl;
                return true;
            }

            if(!impos) {
                for(int j=0;j<rows*rows;j++){
                    if(vertices[j].dom.size() == 1){
                        board[vertices[j].pos_x][vertices[j].pos_y] = *vertices[j].dom.begin();
                        vertices[j].val = *vertices[j].dom.begin();
                        blanks.erase(vertices[j].id);
                    }
                }
                if(blanks.empty()){
                    cout << "blanks empty\n";
                    cout << "Nodes visited : " << node_cnt << endl;
                    cout << "no blanks any more\n";
                    for(auto el : board){
                        printv(el);
                    }
                    cout << "Board seems " << checker() << endl;
                    return true;
                }
//                print();
//                cout << "updated board\n";
//                for(auto el : board){
//                    printv(el);
//                }
                next = get_next_sdf();
                if(backtrack_mac(next)){
                    return true;
                }else{
                    cnt_backtrack++;
                }
            }
            board[cur_row][cur_col] = 0;
//            vertices[s].val = 0;
            for(int j=0;j<vertices.size();j++){
                vertices[j] = saved_vertices[j];
                int r = vertices[j].pos_x;
                int c = vertices[j].pos_y;
                board[r][c] = vertices[j].val;
                if(board[r][c] == 0 and j != s) blanks.insert(vertices[j].id);
            }
        }
        blanks.insert(s);
        return false;
    }

    bool checker(){
        int rows = board.size();
        for(int i=0;i<rows;i++){
            set<int> st1, st2;
            for(int j=0;j<rows;j++){
                st1.insert(board[i][j]);
                st2.insert(board[j][i]);
            }
            if(st1.size() != rows or st2.size() != rows){
                if(st1.size() != rows){
                    cout << "row te jhamela\n";
                }else{
                    cout << "col e jhamlea\n";
                }
                cout << i << "," << endl;
                return false;
            }
        }
        return true;
    }

    void print(){
        cout << "Board\n";
        for(auto ele : board){
            printv(ele);
        }
//        cout << "Blanks: \n";
        for(auto ele : blanks){
            cout << ele << " ";
        }
        cout << endl;
        int rows = board.size();
        for(int i=0;i<rows*rows;i++){
            cout << i << ": ";
            printst(vertices[i].dom);
        }
    }
};

void chin(int n){
    char ch;
    while(n--)
        cin >> ch;
}

vector<vector<int>> input(){
    int n, i, j;
    char ch;
    chin(2);
    cin >> n;
    chin(1);
    vt<vt<int>> v(n, vt<int> (n));
    string s;
    cin >> s;
    cin >> s;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            cin >> v[i][j] >> ch;
        }
    }
    return v;
}

vector<vector<int>> my_input(){
    int n, i, j, k;
    cin >> n;
    vector<vector<int>> v(n, vector<int> (n));
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            cin >> v[i][j];
        }
    }
    return v;
}

void solve(ll cs){
//    vector<vector<int>> board = input();
    vector<vector<int>> board = my_input();
    Graph g(board);
//    g.print();
    g.AC3(0);
    cout << "first : " << g.get_next_sdf() << endl;
    g.backtrack_fc(g.get_next_sdf());
    cout << "node_cnt: " <<  g.node_cnt << " " << g.cnt_backtrack << endl;
    cout << "init blanks: " << g.init_blanks_cnt << endl;
//    g.print();
}


int main()
{
    ios::sync_with_stdio(false);
#ifndef ONLINE_JUDGE
    freopen("../in1", "r", stdin);
    freopen("../out", "w", stdout);
#endif // ONLINE_JUDGE
    ll tt = 1;
//    cin >> tt;
    ll cs = 1;
    while (tt--)
        solve(cs++);

    return 0;
}