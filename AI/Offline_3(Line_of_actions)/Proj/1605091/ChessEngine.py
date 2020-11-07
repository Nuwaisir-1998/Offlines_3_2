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
        self.create_board()
        self.whiteToMove = True
        self.moveLog = []

        self.init_totals()

    def create_board(self):
        self.board = []
        dim = self.dimension
        for j in range(dim):
            row = []
            for i in range(dim):
                row.append(2)
            self.board.append(row)

        for i in range(1, self.dimension-1):
            self.board[0][i] = self.color
            self.board[self.dimension-1][i] = self.color
        for i in range(1, self.dimension-1):
            self.board[i][0] = abs(1 - self.color)
            self.board[i][self.dimension-1] = abs(1 - self.color)



    def init_totals(self):
        dim = self.dimension
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

    def undoMove(self):
        if len(self.moveLog) != 0:
            move = self.moveLog.pop()
            self.board[move.startRow][move.startCol] = move.pieceMoved
            self.board[move.endRow][move.endCol] = move.pieceCaptured
            self.whiteToMove = not self.whiteToMove


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