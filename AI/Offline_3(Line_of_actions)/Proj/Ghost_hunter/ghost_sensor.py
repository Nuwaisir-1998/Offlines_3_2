import random
import copy
class GameState():
    def __init__(self, dim, color):
        self.dimension = dim
        self.color = color
        self.row_total = [0, 0, 0, 0, 0, 0, 0, 0]
        self.col_total = [0, 0, 0, 0, 0, 0, 0, 0]
        self.diag_rl_total = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.diag_lr_total = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        # self.board = [
        #     [2, 1, 1, 1, 1, 1, 1, 2],
        #     [0, 2, 2, 2, 2, 2, 2, 0],
        #     [0, 2, 2, 2, 2, 2, 2, 0],
        #     [0, 2, 2, 2, 2, 2, 2, 0],
        #     [0, 2, 2, 2, 2, 2, 2, 0],
        #     [0, 2, 2, 2, 2, 2, 2, 0],
        #     [0, 2, 2, 2, 2, 2, 2, 0],
        #     [2, 1, 1, 1, 1, 1, 1, 2]]
        self.cur_x = 0              # make it random later
        self.cur_y = 1              # make it random later
        self.P = []
        self.P_ij = {}              # P(i | j)
        self.R_ej = {}
        side_prob = float(random.uniform(0, 0.8))
        self.self_loop_prob = float(random.uniform(0, 0.1))
        self.side_move_prob = side_prob
        self.diag_move_prob = (1.0 - side_prob - self.self_loop_prob)
        print(self.side_move_prob + self.diag_move_prob + self.self_loop_prob)
        assert self.side_move_prob + self.diag_move_prob + self.self_loop_prob == 1
        self.create_board()
        self.whiteToMove = True
        self.moveLog = []




        self.init_totals()

    def create_board(self):
        self.board = []
        dim = self.dimension
        # print("dim :", dim)
        for j in range(dim):
            row = []
            for i in range(dim):
                row.append(2)
            self.board.append(row)

        self.cur_x = random.randint(0, dim-1)
        self.cur_y = random.randint(0, dim-1)
        print("pos: ", (self.cur_x, self.cur_y))
        self.board[self.cur_x][self.cur_y] = 0

        # for i in range(1, self.dimension-1):
        #     self.board[0][i] = self.color
        #     self.board[self.dimension-1][i] = self.color
        # for i in range(1, self.dimension-1):
        #     self.board[i][0] = abs(1 - self.color)
        #     self.board[i][self.dimension-1] = abs(1 - self.color)

        # making P0
        P0 = []
        for i in range(dim):
            row = []
            for j in range(dim):
                row.append(1.0 / (dim * dim))
            P0.append(row)
        # self.P.append(P0)

        self.P = P0;
        dx1 = [1, 0, -1, 0]
        dy1 = [0, 1, 0, -1]
        dx2 = [1, -1, -1, 1]
        dy2 = [1, 1, -1, -1]

        for i in range(dim):
            for j in range(dim):


                #add self edge

                self.P_ij[((i, j), (i, j))] = self.self_loop_prob

                for k in range(len(dx1)):
                    to_x = (i + dx1[k])
                    to_y = (j + dy1[k])
                    if to_x < 0 or to_x >= dim or to_y < 0 or to_y >= dim:
                        to_x = i
                        to_y = j
                        val = self.P_ij.get(((to_x, to_y), (i, j)), 0) + self.side_move_prob / 4.0
                        self.P_ij[((to_x, to_y), (i, j))] = val
                        continue
                    self.P_ij[((to_x, to_y), (i, j))] = self.side_move_prob / 4.0
                for k in range(len(dx2)):
                    to_x = (i + dx2[k])
                    to_y = (j + dy2[k])
                    if to_x < 0 or to_x >= dim or to_y < 0 or to_y >= dim:
                        to_x = i
                        to_y = j
                        val = self.P_ij.get(((to_x, to_y), (i, j)), 0) + self.diag_move_prob / 4.0
                        self.P_ij[((to_x, to_y), (i, j))] = val
                        continue
                    self.P_ij[((to_x, to_y), (i, j))] = self.diag_move_prob / 4.0

        print(self.P_ij)
        self.set_R_ej()
        # self.calculate_probability()


    def move_side(self):
        dx = [1, 0, -1, 0, 0]
        dy = [0, 1, 0, -1, 0]
        rand = random.randint(0, 3)

        dim = len(self.board)
        # board er baire ber hoye jay ekhane
        if self.cur_x + dx[rand] < 0 or self.cur_x + dx[rand] >= dim:
            rand = 4
        if self.cur_y + dy[rand] < 0 or self.cur_y + dy[rand] >= dim:
            rand = 4
        cur_pos = (self.cur_x, self.cur_y)
        # print("rand", rand)

        next_pos = ((self.cur_x + dx[rand]), (self.cur_y + dy[rand]))
        self.cur_x = next_pos[0]
        self.cur_y = next_pos[1]
        return Move(cur_pos, next_pos, self.board)

    def move_diag(self):
        dx = [1, -1, -1, 1, 0]
        dy = [1, 1, -1, -1, 0]
        rand = random.randint(0, 3)

        dim = len(self.board)
        # board er baire ber hoye jay ekhane
        if self.cur_x + dx[rand] < 0 or self.cur_x + dx[rand] >= dim:
            rand = 4
        if self.cur_y + dy[rand] < 0 or self.cur_y + dy[rand] >= dim:
            rand = 4
        cur_pos = (self.cur_x, self.cur_y)
        next_pos = ((self.cur_x + dx[rand]), (self.cur_y + dy[rand]))
        # print("rand", rand)
        self.cur_x = next_pos[0]
        self.cur_y = next_pos[1]
        return Move(cur_pos, next_pos, self.board)

    def move_self(self):
        cur_pos = (self.cur_x, self.cur_y)
        return Move(cur_pos, cur_pos, self.board)

    def generate_move(self):
        rand = random.uniform(0, 1)
        if rand <= self.side_move_prob:
            print("side move")
            self.makeMove(self.move_side())
        elif rand <= self.self_loop_prob + self.side_move_prob:
            print("self loop")
            self.makeMove(self.move_self())
        else:
            print("diagonal move")
            self.makeMove(self.move_diag())


    def calculate_probability(self):
        # print(type(self.P[0][0]))
        n = len(self.board)
        # for i in range(n):
        #     for j in range(n):
        #         print(self.P[i][j], end=" ")
        #     print()

        temp = []
        for i in range(n):
            row = []
            for j in range(n):
                row.append(self.P[i][j])
            temp.append(row)
        # print(self.P[0][0])

        for i in range(n):
            for j in range(n):
                temp[i][j] = 0
                for k in range(n):
                    for l in range(n):
                        # print(type(self.P[k][l]), type(self.P_ij.get(((i, j), (k, l)), 0)))
                        # print(k, l, self.P[k][l], ',', self.P_ij.get(((i, j), (k, l)), 0.0))
                        temp[i][j] += float(self.P[k][l]) * float(self.P_ij.get(((i, j), (k, l)), 0.0))
                # print("-------------")
        self.P = temp
        # for i in range(n):
        #     for j in range(n):
        #         print(self.P[i][j], end=" ")
        #     print()

        # Now the second part
        # P(X | e) = P(X) * P(e | X) / sum

    def sense(self, e):
        # make sure e is 3 var tuple
        dim = len(self.board)
        temp = {}
        sum = 0.0
        for i in range(dim):
            for j in range(dim):
                temp[(i, j)] = self.P[i][j] * self.R_ej[(e, (i, j))]
                sum += temp[(i, j)]
        for i in range(dim):
            for j in range(dim):
                self.P[i][j] = temp[(i, j)] / sum
            print(self.P[i])

    def init_totals(self):
        dim = self.dimension
        # for i in range(len(self.board)):
        #     print(self.board[i])
        self.row_total = [0 for _ in range(self.dimension)]
        self.col_total = [0 for _ in range(self.dimension)]
        self.diag_rl_total = [0 for _ in range(2 * self.dimension - 1)]
        self.diag_lr_total = [0 for _ in range(2 * self.dimension - 1)]
        for i in range(dim):
            for j in range(dim):
                if self.board[i][j] != 2:
                    self.row_total[i] += 1
                    self.col_total[j] += 1
                    self.diag_rl_total[i + j] += 1
                    self.diag_lr_total[i - j + dim - 1] += 1
        # self.calculate_probability()
        # self.cur_x = random.randint(0, dim-1)
        # self.cur_y = random.randint(0, dim-1)
        # print("pos: ", (self.cur_x, self.cur_y))
        # self.board[self.cur_x][self.cur_y] = 0

    def fix_manhattan(self, x, y):
        i = x[0]
        j = x[1]
        k = y[0]
        l = y[1]
        dim = len(self.board)
        dist = abs(i - k) + abs(j - l)
        mx_dist = dim - 1 + dim - 1
        if(3 * dist <= mx_dist):
            return 0
        elif 3 * dist <= mx_dist * 2:
            return 1
        return 2


    def set_R_ej(self):
        dim = len(self.board)
        for i in range(dim):
            for j in range(dim):
                for k in range(dim):
                    for l in range(dim):
                        for col in range(3):
                            val = 0.0
                            if self.fix_manhattan((i, j), (k, l)) == col:
                                val = 1.0
                            self.R_ej[((i, j, col), (k, l))] = val


    def sort_left_to_right_col(self, p1, p2):
        if p1[1] < p2[1]:
            start = p1
            end = p2
        else:
            start = p2
            end = p1
        return (start, end)

    def sort_top_to_bottom_row(self, p1, p2):
        if p1[0] < p2[0]:
            start = p1
            end = p2
        else:
            start = p2
            end = p1
        return (start, end)


    def exists_row(self, color, p1, p2):
        p = self.sort_left_to_right_col(p1, p2)
        start = p[0]
        end = p[1]
        for i in range(start[1]+1, end[1]):
            if self.board[start[0]][i] == color:
                return True
        return False

    def exists_col(self, color, p1, p2):
        p = self.sort_top_to_bottom_row(p1, p2)
        start = p[0]
        end = p[1]
        for i in range(start[0]+1, end[0]):
            if self.board[i][start[1]] == color:
                return True
        return False

    def exists_right_diag(self, color, p1, p2):
        p = self.sort_left_to_right_col(p1, p2)
        start = p[0]
        end = p[1]
        for i in range(1, abs(start[0] - end[0])):
            if self.board[start[0] - i][start[1] + i] == color:
                return True
        return False

    def exists_left_diag(self, color, p1, p2):
        p = self.sort_left_to_right_col(p1, p2)
        start = p[0]
        end = p[1]
        for i in range(1, abs(start[0] - end[0])):
            if self.board[start[0] + i][start[1] + i] == color:
                return True
        return False


    def move_up(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt_c = self.col_total[c]
        if r - cnt_c >= 0 and self.board[r - cnt_c][c] != color:
            end_p = (r - cnt_c, c)
            if self.exists_col(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p

    def move_down(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt_c = self.col_total[c]
        if r + cnt_c < self.dimension and self.board[r + cnt_c][c] != color:
            end_p = (r + cnt_c, c)
            if self.exists_col(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p

    def move_left(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt_r = self.row_total[r]
        if c - cnt_r >= 0 and self.board[r][c - cnt_r] != color:
            end_p = (r, c - cnt_r)
            if self.exists_row(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p

    def move_right(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt_r = self.row_total[r]
        if c + cnt_r < self.dimension and self.board[r][c + cnt_r] != color:
            end_p = (r, c + cnt_r)
            if self.exists_row(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p

    def move_up_left(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt_ul = self.diag_lr_total[r - c + self.dimension - 1]
        if c - cnt_ul >= 0 and r - cnt_ul >= 0 and self.board[r - cnt_ul][c - cnt_ul] != color:
            end_p = (r - cnt_ul, c - cnt_ul)
            if self.exists_left_diag(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p

    def move_down_right(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt_ul = self.diag_lr_total[r - c + self.dimension - 1]
        if c + cnt_ul < self.dimension and r + cnt_ul < self.dimension and self.board[r + cnt_ul][c + cnt_ul] != color:
            end_p = (r + cnt_ul, c + cnt_ul)
            if self.exists_left_diag(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p

    def move_up_right(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt = self.diag_rl_total[r + c]
        if c + cnt < self.dimension and r - cnt >= 0 and self.board[r - cnt][c + cnt] != color:
            end_p = (r - cnt, c + cnt)
            if self.exists_right_diag(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p

    def move_down_left(self, p, color):
        end_p = (-1, -1)
        r = p[0]
        c = p[1]
        cnt = self.diag_rl_total[r + c]
        if c - cnt >= 0 and r + cnt < self.dimension and self.board[r + cnt][c - cnt] != color:
            end_p = (r + cnt, c - cnt)
            if self.exists_right_diag(abs(1 - color), p, end_p):
                return (-1, -1)
        return end_p



    def generate_all_moves(self, p, color):
        moves = []
        p_end = self.move_up(p, color)
        if p_end[0] != -1:
            moves.append(p_end)

        p_end = self.move_down(p, color)
        if (p_end[0] != -1):
            moves.append(p_end)

        p_end = self.move_left(p, color)
        if (p_end[0] != -1):
            moves.append(p_end)

        p_end = self.move_right(p, color)
        if (p_end[0] != -1):
            moves.append(p_end)

        p_end = self.move_up_left(p, color)
        if (p_end[0] != -1):
            moves.append(p_end)

        p_end = self.move_up_right(p, color)
        if p_end[0] != -1:
            moves.append(p_end)

        p_end = self.move_down_right(p, color)
        if (p_end[0] != -1):
            moves.append(p_end)

        p_end = self.move_down_left(p, color)
        if (p_end[0] != -1):
            moves.append(p_end)

        return moves




    def makeMove(self, move):
        self.board[move.startRow][move.startCol] = 2
        self.board[move.endRow][move.endCol] = move.pieceMoved
        self.moveLog.append(move) # log the move, so we can undo it later
        self.whiteToMove = not self.whiteToMove #swap players

        self.init_totals()
        self.calculate_probability()
        # self.sense(move.endRow, move.endCol)

    def undoMove(self):
        if len(self.moveLog) != 0:
            move = self.moveLog.pop()
            self.board[move.startRow][move.startCol] = move.pieceMoved
            self.board[move.endRow][move.endCol] = move.pieceCaptured
            self.whiteToMove = not self.whiteToMove

    def check_side(self, i, j, k, l):
        if i == k and j == l:
            return 0
        if abs(i - k) == 1 and abs(j - l) == 1:
            return 2
        v1 = abs(i - k)
        v2 = abs(j - l)
        if v1 + v2 == 1:
            return 1
        return -1
        pass


class Move():
    # maps keys to values
    # key : value
    ranksToRows = {"1": 7, "2": 6, "3": 5, "4": 4,
                   "5": 3, "6": 2, "7": 1, "8": 0}
    rowsToRanks = {v: k for k, v in ranksToRows.items()}
    filesToCols = {"a": 7, "b": 6, "c": 5, "d": 4,
                   "e": 3, "f": 2, "g": 1, "h": 0}
    colsToFiles = {v: k for k, v in filesToCols.items()}

    def __init__(self, startSq, endSq, board):
        self.startRow = startSq[0]
        self.startCol = startSq[1]
        self.endRow = endSq[0]
        self.endCol = endSq[1]
        self.pieceMoved = board[self.startRow][self.startCol]
        self.pieceCaptured = board[self.endRow][self.endCol]

    def getChessNotation(self):
        # You can add to make this like real chess notation
        return self.getRankFile(self.startRow, self.startCol) + self.getRankFile(self.endRow, self.endCol)

    def getRankFile(self, r, c):
        return self.colsToFiles[c] + self.rowsToRanks[r]