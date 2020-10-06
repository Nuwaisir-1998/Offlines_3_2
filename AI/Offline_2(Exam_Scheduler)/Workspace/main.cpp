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
typedef pair<int, int> pii;
typedef vector<pll> vpll;

#define all(x) (x).begin(), (x).end()
#define MOD 1000000007
#define PI acos(-1)
#define MAXN 200005
#define INF 1000000000000000000
#define nl '\n'
#define MAX(x) *max_element(all(x))
#define MIN(x) *min_element(all(x))

template <typename T>
void printv(vector<T> &v){for (auto e : v) cout << e << ' ';cout << '\n';}

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

class Graph
{
    public:
    // considering 0 indexing
    int V;
    int n_stu;
    vector<set<int>> adj;
    vector<int> color;
    vector<vector<int>> student_courses;
    vector<bool> vis;
    vector<int> saturation;
    vector<bool> mark;
    Graph(int V){
        init(V);
    }

    void init(int n){
        this -> V = n;
        adj.resize(n);
        vis.resize(n);
        mark.resize(n);
        color.resize(n);
        saturation.resize(n);
        fill(color.begin(), color.end(), -1);
    }

    void add_student_with_courses(vector<int> courses){
        student_courses.push_back(courses);
    }

    void addEdge(int u, int v){
        if(v >= V or u >= V or v < 0 or u < 0){
            cout << "can't add edge (" << u << ", " << v << ")\n";
            return;
        }
        adj[u].insert(v);
        adj[v].insert(u);
    }

    void print(){
        cout << "adj:\n";
        int mx = 0;
        for(int i=0;i<V;i++){
            cout << i << ": ";
            for(auto ele : adj[i]){
                cout << ele << " ";
            }
            cout << "\n";
            mx = max(mx, (int)adj[i].size());
        }
        cout << "Max degree: ";
        cout << mx << "\n";
        cout << "student_courses:\n";
        for(int i=0;i<student_courses.size();i++){
            cout << i << ": ";
            printv(student_courses[i]);
        }
        
        cout << "color:\n";
        printv(color);

        cout  << "color of neighbours:\n";
        for(int i=0;i<student_courses.size();i++){
            cout << i << ": ";
            for(auto ele : student_courses[i]){
                cout << color[ele] << " ";
                // cout << ele << " ";
            }
            cout << "\n";
        }
    }

    int MEX(int u){
        unordered_set<int> ust;
        for(auto ele : adj[u]){
            ust.insert(color[ele]);
        }
        for(int i=0;i<V;i++){
            if(ust.find(i) == ust.end()){
                return i;
            }
        }
    }

    int calculate_slots_cnt(){
        unordered_set<int> st;
        for(int i=0;i<V;i++){
            if(color[i] == -1){
                cout << "vertex " << i << " is still uncolored.\n";
            }else
                st.insert(color[i]);
        }
        return (int)st.size();
    }

    vector<int> get_highest_degree_ordering() {
        vector<pii> vp;
        for(int i=0;i<V;i++){
            vp.push_back({(int)adj[i].size(), i});
        }
        sort(vp.begin(), vp.end(), greater<pii>());
        vector<int> order(V);
        for(int i=0;i<V;i++) order[i] = vp[i].second;
        return order;
    }

    void calculate_saturation(){    /// needs unit testing
        for(int i=0;i<V;i++){
            unordered_set<int> ust;
            for(auto ele : adj[i]){
                if(color[ele] != -1){
                    ust.insert(color[ele]);
                }
            }
            saturation[i] = (int)ust.size();
        }
    }

    void fix_saturation(int v){
        unordered_set<int> ust;
        for(auto ele : adj[v]){
            if(color[ele] != -1){
                ust.insert(color[ele]);
            }
        }
        saturation[v] = (int)ust.size();
    }

    int get_max_sat_deg(){
        tuple<int,int,int> mx = {-1, -1, -1};
        for(int i=0;i<V;i++){
            tuple<int,int,int> t = {saturation[i], (int)adj[i].size(), i};
            if(color[get<2>(t)] == -1)
                mx = max(mx, t);
        }
        return get<2>(mx);
    }

    int get_uncolored(){
        for(int i=0;i<V;i++){
            if(color[i] == -1) return i;
        }
        return -1;
    }

    int penalty(int dif){
        if(dif > 5) return 0;
        return power(2, 5 - dif);
    }

    double calculate_total_penalty(){
        int n_stu = student_courses.size();
        double tot_pen = 0;
        for(int i=0;i<n_stu;i++){
            vector<ll> slots;
            for(auto course : student_courses[i]){
                slots.push_back(color[course]);
            }
            sort(all(slots));
            for(int j=1;j<slots.size();j++){
                tot_pen += penalty(slots[j] - slots[j-1]);
            }
        }
        return tot_pen / n_stu;
    }

    void dfsVis(int s, int c1, int c2){
        vis[s] = true;
        mark[s] = true;
        if(color[s] == c1) color[s] = c2;
        else color[s] = c1;
        for(auto ele : adj[s]){
            if(!vis[ele] and (color[ele] == c1 or color[ele] == c2)){
                dfsVis(ele, c1, c2);
            }
        }
    }

    void dfs(int s, int c1, int c2){
        fill(vis.begin(), vis.end(), false);
        
        dfsVis(s, c1, c2);
    }

    void kempe(int u, int v){
        // check if the 2 vertices are adjacent
        if(adj[u].find(v) == adj[u].end()) {
            cout << "KEMPE input vertices " << u << "," << v << " are not adjacent\n";
            return;
        }

        dfs(u, color[u], color[v]);
        cout << "After KEMPE at " << u << "," << v << ": " << calculate_total_penalty() << "\n";

    }

    void checker(){
        bool jhamela = false;
        for(int i=0;i<V;i++){
            int col = color[i];
            for(auto ele : adj[i]){
                if(color[ele] == col){
                    cout << "anomaly at coloring " << i << ", " << ele << "\n";
                    jhamela = true;
                }
            }
        }
        
        for(int i=0;i<student_courses.size();i++){
            unordered_map<int,bool,custom_hash> ump;
            for(auto ele : student_courses[i]){
                if(ump[color[ele]]){
                    cout << "eije jhamela\n";
                    jhamela = true;
                }
                ump[color[ele]] = true;
            }
        }

        if(!jhamela){
            cout << "Kono sheomessha nai\n";
        }
    }

    void crs_generator(){
        vector<int> crs(V);
        for(int i=0;i<student_courses.size();i++){
            for(auto ele : student_courses[i]){
                crs[ele]++;
            }
        }
        for(int i=0;i<V;i++){
            cout << setfill('0') << setw(4) << i + 1 << " " << crs[i] << "\n";
        }
    }
    
    void write_sol(){
        for(int i=0;i<V;i++){
            cout << i + 1 << " " << color[i] << "\n";
        }
    }

    void multi_kempe(){
        // int mx = 0, idx = 0;
        // for(int i=0;i<V;i++){
        //     if(mx < adj[i].size()){
        //         mx = adj[i].size();
        //         idx = i;
        //     }
        // }
        // if(adj[idx].size() >= 1){
        //     vector<int> temp(adj[idx].begin(), adj[idx].end());
        //     kempe(temp[0], idx);
        // }
        // double prev = calculate_total_penalty();

        // for(int i=0;i<student_courses.size();i++){
        //     if(student_courses[i].size() >= 2){
        //         int u = student_courses[i][0];
        //         int v = student_courses[i][1];
        //         kempe(u, v);
        //         break;
        //     }
        // }
        double mn = (double)INF;
        int t = V;
        while(t--){
            for(int i=0;i<V;i++){
                if(!mark[i]){
                    bool f = false;
                    for(auto ele : adj[i]){
                        if(!mark[ele]){
                            kempe(i, ele);
                            double now = calculate_total_penalty();
                            mn = min(mn, now);
                            f = true;
                            break;
                            
                        }
                    }
                    if(f){
                        break;
                    }
                }
            }
        }
        
        cout << "min cost : " << mn << "\n";
    }

    void pair_swap(int u, int v){
        swap(color[u], color[v]);
    }

    void multi_pair_swap(){
        for(int i=0;i<V;i++){
            for(int j=i+1;j<V;j++){

            }
        }
    }

    // Constructive heuristics
    void largest_degree_heuristic(){
        vector<int> order = get_highest_degree_ordering();
        // cout << "order:\n";
        // printv(order);
        for(int i=0;i<order.size();i++){
            set<int> adj_colors;
            for(auto ele : adj[order[i]]){
                adj_colors.insert(color[ele]);
            }
            for(int j=0;j<V;j++){
                if(adj_colors.find(j) == adj_colors.end()){
                    color[order[i]] = j;
                    break;
                }
            }
        }
        
        // cout << "Slots : " << calculate_slots_cnt() << "\n";
    }

    void d_satur(){
        priority_queue<tuple<int,int,int>> pq;
        // pushing all now, as graph can be disconnected
        for(int i=0;i<V;i++){   
            pq.push({saturation[i], adj[i].size(), i});
        }
        int colored_count = 0;
        while(colored_count < V){
            // calculate_saturation();
            // int u = get_max_sat_deg();
            int u = -1;
            while(true){
                u = get<2>(pq.top());
                if(color[u] != -1) pq.pop();
                else break;
            }
            if(u == -1) break;
            pq.pop();
            color[u] = MEX(u);
            for(auto ele : adj[u]){
                if(color[ele] == -1){
                    fix_saturation(ele);
                    pq.push({saturation[ele], adj[ele].size(), ele});
                }
            }
           
            // cout << u << " got " << color[u] << "\n";
            colored_count++;
        }
        // cout << "Slots : " << calculate_slots_cnt() << "\n";
    }
};

void read_crs(Graph & g, string filename){
    ifstream fin("inputs/" + filename);
    string s;
    int cnt = 0;
    while(getline(fin, s)){
        cnt++;
        // s.push_back(' ');
        // int n = s.size();
        // string str;
        // vector<int> courses;
        // for(int i=0;i<n;i++){
        //     if(s[i] == ' '){
        //         if(!str.empty())
        //             courses.push_back(stoi(str));
        //         str.clear();
        //     }else{
        //         if(s[i] != ' ')
        //             str.push_back(s[i]);
        //     }
        // }
        // printv(courses);
        
    }
    g.init(cnt);
    fin.close();
}

void read_stu(Graph & g, string filename){
    ifstream fin("inputs/" + filename);
    string s;
    int cnt = 0;
    while(getline(fin, s)){
        s.push_back(' ');
        int n = s.size();
        string str;
        vector<int> courses;
        for(int i=0;i<n;i++){
            if(s[i] == ' '){
                if(!str.empty())
                    courses.push_back(stoi(str) - 1);
                str.clear();
            }else{
                if(s[i] != ' ')
                    str.push_back(s[i]);
            }
        }
        // printv(courses);

        for(int i=0;i<courses.size();i++){
            for(int j=i+1;j<courses.size();j++){
                g.addEdge(courses[i], courses[j]);
            }
        }
        g.add_student_with_courses(courses);
        cnt++;
    }
    // g.set_n_stu(cnt);
    fin.close();
}

void solve(){
    ll n, m;
    
    Graph g = Graph(2);
    string crs, stu;
    cin >> crs >> stu;
    // cout << f1 << " " << f2 << "\n";
    read_crs(g, crs);
    read_stu(g, stu);
    // g.largest_degree_heuristic();
    g.d_satur();
    cout << g.calculate_slots_cnt() << " " << g.calculate_total_penalty() << "\n";
    g.multi_kempe();
    // g.multi_kempe();
    // g.multi_kempe();

    // g.checker();
    // g.write_sol();

    // g.print();
}

int main()
{
    ios::sync_with_stdio(false);
#ifndef ONLINE_JUDGE
    freopen("in", "r", stdin);
    freopen("out", "w", stdout);
#endif // ONLINE_JUDGE
    // ifstream f("in");
    // string line;
    // while(getline(f, line)){
    //     cout << line << "\n";
    // }
    // f.close();

    // ifstream f2("in2");
    
    // while(getline(f2, line)){
    //     cout << line << "\n";
    // }
    // f2.close();
    
    ll tt = 1;
    // cin >> tt;
    ll cs = 1;
    while (tt--)
        solve();
    return 0;
}