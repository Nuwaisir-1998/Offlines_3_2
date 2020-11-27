#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "main.cpp"
using namespace std;

int main (int argc, char * argv[]) {

    char my_color = argv[1][0];
    int arena_size = atoi(argv[2]);
    char heuristic = 'T';
    int bot_color;
    if(my_color == 'r') bot_color = 1;
    else bot_color = 0;

    int search_depth = 5;
    if (arena_size == 6) search_depth = 5;
    // Game bot(my_color, search_depth, arena_size, heuristic);

    string cmd;

    while (true) {
        vector<vector<int>> board;
        string s;
        for(int i=0;i<arena_size;i++){
            cin >> s;
            vector<int> v;
            for(auto ele : s){
                if(s[i] == 'e'){
                    v.push_back(2);
                }else if(s[i] == 'r') v.push_back(1);
                else v.push_back(0);
            }
            board.push_back(v);
        }
        Game game(board);
        // bot.get_map();
        game.backtrack(game, search_depth, bot_color, true, -1000000000, 1000000000);
        pair<Position, Position> pp = game.get_best_move();
        cout << pp.first.x << " " << pp.first.y << " " << pp.second.x << " " << pp.second.y << endl;
    }
    return 0;
}
