class GameState():
    def __init__(self):
        self.board = [
            ["--", "wp", "wp", "wp", "wp", "wp", "wp", "--"],
            ["bp", "--", "--", "--", "--", "--", "--", "bp"],
            ["bp", "--", "--", "--", "--", "--", "--", "bp"],
            ["bp", "--", "--", "--", "--", "--", "--", "bp"],
            ["bp", "--", "--", "--", "--", "--", "--", "bp"],
            ["bp", "--", "--", "--", "--", "--", "--", "bp"],
            ["bp", "--", "--", "--", "--", "--", "--", "bp"],
            ["--", "wp", "wp", "wp", "wp", "wp", "wp", "--"]]
        self.whiteToMove = True
        self.moveLog = []
    def makeMove(self, move):
        self.board[move.startRow][move.startCol] = "--"
        self.board[move.endRow][move.endCol] = move.pieceMoved
        self.moveLog.append(move) # log the move, so we can undo it later
        self.whiteToMove = not self.whiteToMove #swap players

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