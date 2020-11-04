import pygame as p
import subprocess
from Chess import ChessEngine

WIDTH = HEIGHT = 512
DIMENSION = 8
SQ_SIZE = HEIGHT // DIMENSION
MAX_FPS = 15
IMAGES = {}

# Initialize a global dictionary of images. This will be called exactly in the main

def loadImages():
	pieces = ['wp', 'wR', 'wN', 'wB', 'wK', 'wQ', 'bp', 'bR', 'bN', 'bB', 'bK', 'bQ']
	for piece in pieces:
		IMAGES[piece] = p.transform.scale(p.image.load("images/" + piece + ".png"), (SQ_SIZE, SQ_SIZE))


# The main driver for oue code. This will handle user input and updating the graphics

def change_board_notation(board):
	new_board = board
	for i in range(DIMENSION):
		for j in range(DIMENSION):
			if board[i][j] == 'bp':
				new_board[i][j] = '0'
			elif board[i][j] == 'wp':
				new_board[i][j] = '1'
			else:
				new_board[i][j] = '2'
	return new_board

def main():
	p.init()
	screen = p.display.set_mode((WIDTH, HEIGHT))
	clock = p.time.Clock()
	screen.fill(p.Color("white"))
	gs = ChessEngine.GameState()
	loadImages()
	running = True
	sqSeleceted = () # will keep track of the last click of the user (row, col)
	playerClicks = [] # keep track of the player clicks (e.g. [(6, 4), (4, 4)]

	my_turn = True

	while running:
		if my_turn:
			for e in p.event.get():
				if e.type == p.QUIT:
					running = False
				elif e.type == p.MOUSEBUTTONDOWN:
					location = p.mouse.get_pos() # x, y location of mouse
					col = location[0] // SQ_SIZE
					row = location[1] // SQ_SIZE
					if sqSeleceted == (row, col):
						sqSeleceted = () # deselect
						playerClicks = [] #clear player clicks
					else:
						sqSeleceted = (row, col)
						playerClicks.append(sqSeleceted) #append first and second clicks
					if len(playerClicks) == 2:
						move = ChessEngine.Move(playerClicks[0], playerClicks[1], gs.board)
						print(move.getChessNotation())
						gs.makeMove(move)
						# new_board = change_board_notation(gs.board)
						f = open("in", "w")
						for i in range(DIMENSION):
							for j in range(DIMENSION):
								if gs.board[i][j] == 'bp':
									f.write('0 ')
								elif gs.board[i][j] == 'wp':
									f.write('1 ')
								else:
									f.write('2 ')
							f.write('\n')
						f.write(str(playerClicks[0][0]) + ' ' + str(playerClicks[0][1]) + '\n' + str(playerClicks[1][0]) + ' ' + str(playerClicks[1][1]))
						f.close()
						my_turn = False
						sqSeleceted = ()
						playerClicks = []
		else:
			my_turn = True
			out = subprocess.run('sh main.sh', shell=True, capture_output=True)
			val = out.stdout.decode()

			start_x = int(val[0])
			start_y = int(val[2])
			end_x = int(val[5])
			end_y = int(val[7])

			move = ChessEngine.Move((start_x, start_y), (end_x, end_y), gs.board)
			print(move.getChessNotation())
			gs.makeMove(move)




		drawGameState(screen, gs)
		clock.tick(MAX_FPS)
		p.display.flip()


# Draw the squares on the board
def drawBoard(screen):
	colors = [p.Color("white"), p.Color("gray")]
	for r in range(DIMENSION):
		for c in range(DIMENSION):
			color = colors[((r + c) % 2)]
			p.draw.rect(screen, color, p.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))


# Draw the peices on the board using the current gamestate.board
def drawPieces(screen, board):
	for r in range(DIMENSION):
		for c in range(DIMENSION):
			piece = board[r][c]
			if piece != "--":
				screen.blit(IMAGES[piece], p.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))


# Responsible for all graphics within a current game state
def drawGameState(screen, gs):
	drawBoard(screen)
	# add piece highilighting and move suggestion later
	drawPieces(screen, gs.board)


if __name__ == "__main__":
	main()

