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
            vector<int> v;
            for(int j=0;j<arena_size;j++){
                char ch;
                cin >> ch;
                if(ch == 'e'){
                    v.push_back(2);
                }else if(ch == 'r') v.push_back(1);
                else v.push_back(0);
            }
            board.push_back(v);
        }
        Game game(board);
        // bot.get_map();
        Position from, to;
        from = game.pos_color[bot_color][0];
        to = game.generate_all_moves(from, bot_color)[0];
        for(int i=1;i<=search_depth;i++){
            game.backtrack(game, search_depth, bot_color, true, -1000000000, 1000000000);
            if(game.get_best_move().first.x != -1){
                from = game.get_best_move().first;
                to = game.get_best_move().second;
            }
        }
        // pair<Position, Position> pp = game.get_best_move();
        cout << from.x << " " << from.y << " " << to.x << " " << to.y << endl;
    }
    return 0;
}
