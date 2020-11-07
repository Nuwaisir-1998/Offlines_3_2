import pygame as p
import subprocess
import pygame_menu
from Chess import ChessEngine

WIDTH = HEIGHT = 512
DIMENSION = 8
SQ_SIZE = HEIGHT // DIMENSION
MAX_FPS = 15
IMAGES = {}


# Initialize a global dictionary of images. This will be called exactly in the main

def loadImages():
	# pieces = ['wp', 'wR', 'wN', 'wB', 'wK', 'wQ', 'bp', 'bR', 'bN', 'bB', 'bK', 'bQ']
	# for piece in pieces:
	# 	IMAGES[piece] = p.transform.scale(p.image.load("images/" + piece + ".png"), (SQ_SIZE, SQ_SIZE))

	IMAGES[0] = p.transform.scale(p.image.load("images/bp.png"), (SQ_SIZE, SQ_SIZE))
	IMAGES[1] = p.transform.scale(p.image.load("images/wp.png"), (SQ_SIZE, SQ_SIZE))

def highlighSquare(screen, gs, validMoves, sqSelected):
	if sqSelected != ():
		r, c = sqSelected
		if gs.board[r][c] == (1 if gs.whiteToMove else 0):
			s = p.Surface((SQ_SIZE, SQ_SIZE))
			s.set_alpha(100)
			s.fill(p.Color('blue'))
			screen.blit(s, (c * SQ_SIZE, r * SQ_SIZE))

			s.fill(p.Color('yellow'))
			for move in validMoves:
				# if move.startRow == r and move.startCol == c:
				screen.blit(s, (SQ_SIZE * move[1], SQ_SIZE * move[0]))



# The main driver for oue code. This will handle user input and updating the graphics

def main():
	# p.init()
	screen = p.display.set_mode((WIDTH, HEIGHT))
	clock = p.time.Clock()
	screen.fill(p.Color("white"))
	gs = ChessEngine.GameState()
	loadImages()
	running = True
	sqSeleceted = ()  # will keep track of the last click of the user (row, col)
	playerClicks = []  # keep track of the player clicks (e.g. [(6, 4), (4, 4)]
	print(gs.row_total)
	print(gs.col_total)
	print(gs.diag_rl_total)
	print(gs.diag_lr_total)
	my_turn = True
	validMoves = []
	first_move = True
	subprocess.run('g++ -O2 main.cpp -o main', shell=True, capture_output=True)  # compiling the cpp code
	while running:
		if my_turn:
			for e in p.event.get():
				if e.type == p.QUIT:
					running = False


				# Mouse handler
				elif e.type == p.MOUSEBUTTONDOWN:
					location = p.mouse.get_pos()  # x, y location of mouse
					col = location[0] // SQ_SIZE
					row = location[1] // SQ_SIZE
					if sqSeleceted == (row, col):
						sqSeleceted = ()  # deselect
						playerClicks = []  # clear player clicks
					else:
						sqSeleceted = (row, col)
						playerClicks.append(sqSeleceted)  # append first and second clicks
						if len(playerClicks) == 1 and gs.board[row][col] != 2:
							color = gs.board[row][col]
							print("row col color:", row, col, color)
							validMoves = gs.generate_all_moves((row, col), color)
							for i in gs.board:
								for j in i:
									print(j, end=' ')
								print()
							print(gs.row_total)
							print(gs.col_total)
							print(gs.diag_rl_total)
							print(gs.diag_lr_total)
							print(validMoves)
						elif len(playerClicks) == 1 and gs.board[row][col] == 2:
							sqSeleceted = ()  # deselect
							playerClicks = []  # clear player clicks
					if len(playerClicks) == 2:
						if playerClicks[1] not in validMoves:
							sqSeleceted = ()  # deselect
							playerClicks = []  # clear player clicks
						else:
							move = ChessEngine.Move(playerClicks[0], playerClicks[1], gs.board)
							print(move.getChessNotation())
							gs.makeMove(move)
							# new_board = change_board_notation(gs.board)
							f = open("in", "w")
							for i in range(DIMENSION):
								for j in range(DIMENSION):
									f.write(str(gs.board[i][j]) + ' ')
								f.write('\n')
							f.write(str(playerClicks[0][0]) + ' ' + str(playerClicks[0][1]) + '\n' + str(
								playerClicks[1][0]) + ' ' + str(playerClicks[1][1]))
							f.close()
							my_turn = False
							sqSeleceted = ()
							playerClicks = []
				# key handler
				# Undo
				# elif e.type == p.KEYDOWN:
				# 	if e.key == p.K_z:
				# 		gs.undoMove()
		else:
			my_turn = True
			out = subprocess.run('main.exe', shell=True, capture_output=True)
			val = out.stdout.decode()
			print(val)
			start_x = int(val[0])
			start_y = int(val[2])
			end_x = int(val[5])
			end_y = int(val[7])
			if start_x == 0 and start_y == 9:
				start_x = int(val[10])
				start_y = int(val[12])
				end_x = int(val[15])
				end_y = int(val[17])
				move = ChessEngine.Move((start_x, start_y), (end_x, end_y), gs.board)
				print(move.getChessNotation())
				gs.makeMove(move)
				print('black wins')
			elif start_x == 9 and start_y == 0:
				print('white wins')
			else:
				move = ChessEngine.Move((start_x, start_y), (end_x, end_y), gs.board)
				print(move.getChessNotation())
				gs.makeMove(move)

		drawGameState(screen, gs, validMoves, sqSeleceted)
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
			if piece != 2:
				screen.blit(IMAGES[piece], p.Rect(c * SQ_SIZE, r * SQ_SIZE, SQ_SIZE, SQ_SIZE))


# Responsible for all graphics within a current game state
def drawGameState(screen, gs, validMoves, sqSelected):
	drawBoard(screen)
	drawPieces(screen, gs.board)
	highlighSquare(screen, gs, validMoves, sqSelected)

def dummy():
	pass


if __name__ == "__main__":
	main()


# p.init()
# surface = p.display.set_mode((600, 400))
# menu = pygame_menu.Menu(300, 400, 'Welcome', theme=pygame_menu.themes.THEME_BLUE)
#
# menu.add_text_input('Name :', default='Unnamed')
# # menu.add_selector('Difficulty :', [('Hard', 1), ('Easy', 2)], onchange=set_difficulty)
# menu.add_button('Play', main)
# menu.add_button('Quit', pygame_menu.events.EXIT)
# menu.mainloop(surface)

