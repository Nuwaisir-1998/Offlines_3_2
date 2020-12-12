import pygame as p
import subprocess
import pygame_menu
from Ghost_hunter import ghost_sensor

WIDTH = HEIGHT = 512
DIMENSION = 3
SQ_SIZE = HEIGHT // DIMENSION
MAX_FPS = 15
IMAGES = {}

COLOR = 1


def set_color(value, color):
	global COLOR
	COLOR = color

def set_dimension(value, dim):
	global DIMENSION
	DIMENSION = dim


# Initialize a global dictionary of images. This will be called exactly in the main

def loadImages():
	# pieces = ['wp', 'wR', 'wN', 'wB', 'wK', 'wQ', 'bp', 'bR', 'bN', 'bB', 'bK', 'bQ']
	# for piece in pieces:
	# 	IMAGES[piece] = p.transform.scale(p.image.load("images/" + piece + ".png"), (SQ_SIZE, SQ_SIZE))

	IMAGES[0] = p.transform.scale(p.image.load("images/bp.png"), (SQ_SIZE, SQ_SIZE))
	IMAGES[1] = p.transform.scale(p.image.load("images/wp.png"), (SQ_SIZE, SQ_SIZE))

def drawText(screen, text):
	print("inside drawText")
	font = p.font.SysFont("Helvetica", 32, True, False)
	textObject = font.render(text, 0, p.Color('Black'))
	textLocation = p.Rect(0, 0, WIDTH, HEIGHT).move(WIDTH/2 - textObject.get_width()/2, HEIGHT/2 - textObject.get_height() / 2)
	screen.blit(textObject, textLocation)
	textObject = font.render(text, 0, p.Color("Gray"))
	screen.blit(textObject, textLocation.move(2, 2))

def highlighSquare(screen, gs, sqSelected):
	if sqSelected != ():
		r, c = sqSelected
		if gs.board[r][c] == (COLOR if gs.whiteToMove else abs(1 - COLOR)):
			s = p.Surface((SQ_SIZE, SQ_SIZE))
			s.set_alpha(100)
			s.fill(p.Color('blue'))
			screen.blit(s, (c * SQ_SIZE, r * SQ_SIZE))

			s.fill(p.Color('yellow'))
			# for move in validMoves:
			# 	# if move.startRow == r and move.startCol == c:
			# 	screen.blit(s, (SQ_SIZE * move[1], SQ_SIZE * move[0]))

def highlighSquare2(screen, gs, sqSelected, color):
	if sqSelected != ():
		r, c = sqSelected
		# if gs.board[r][c] == (COLOR if gs.whiteToMove else abs(1 - COLOR)):
		s = p.Surface((SQ_SIZE, SQ_SIZE))
		s.set_alpha(1000)
		s.fill(p.Color(color))
		screen.blit(s, (c * SQ_SIZE, r * SQ_SIZE))

		# s.fill(p.Color('yellow'))
		# for move in validMoves:
		# 	# if move.startRow == r and move.startCol == c:
		# 	screen.blit(s, (SQ_SIZE * move[1], SQ_SIZE * move[0]))

# The main driver for oue code. This will handle user input and updating the graphics

def main():
	# p.init()
	screen = p.display.set_mode((WIDTH, HEIGHT))
	clock = p.time.Clock()
	screen.fill(p.Color("white"))
	gs = ghost_sensor.GameState(DIMENSION, COLOR)
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
	# subprocess.run('g++ -O2 main.cpp -o main', shell=True, capture_output=True)  # compiling the cpp code
	while running:
		color = -1
		if my_turn:
			for e in p.event.get():
				if e.type == p.QUIT:
					running = False


				# Mouse handler

				elif e.type == p.MOUSEBUTTONDOWN:
					location = p.mouse.get_pos()  # x, y location of mouse
					col = location[0] // SQ_SIZE
					row = location[1] // SQ_SIZE
					gs.generate_move()
					color = gs.fix_manhattan((row, col), (gs.cur_x, gs.cur_y))
					print("color:", color)
					gs.sense((row, col, color))
					dim = len(gs.board)
					for i in range(dim):
						for j in range(dim):
							# p.draw.ci
							# p.draw.circle(surface, "red", (row + SQ_SIZE / 2, col + SQ_SIZE / 2), 2)
							highlighSquare(screen, gs, (row, col))
							# s = p.Surface((SQ_SIZE, SQ_SIZE))
							# s.set_alpha(100)
							# s.fill(p.Color('red'))
							# screen.blit(s, (col * SQ_SIZE, row * SQ_SIZE))

		else:
			pass


		# highlighSquare(screen, gs, [(2, 0)], (0, 2))
		drawGameState(screen, gs, sqSeleceted)
		if(color != -1):
			if color == 0:
				rong = 'red'
			elif color == 1:
				rong = 'orange'
			else:
				rong = 'yellow'
			highlighSquare2(screen, gs, (row, col), rong)
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
def drawGameState(screen, gs, sqSelected):
	drawBoard(screen)
	drawPieces(screen, gs.board)
	highlighSquare(screen, gs, sqSelected)




def dummy():
	pass


# if __name__ == "__main__":
# 	main()


p.init()
surface = p.display.set_mode((600, 400))
menu = pygame_menu.Menu(400, 600, 'Welcome', theme=pygame_menu.themes.THEME_BLUE)

menu.add_text_input('Name :', default='Unnamed')
menu.add_selector('Color :', [('White', 1), ('Black', 0)], onchange=set_color)
menu.add_selector('Board Size :', [('8X8', 8), ('6X6', 6)], onchange=set_dimension)
menu.add_button('Play', main)
menu.add_button('Quit', pygame_menu.events.EXIT)

menu.mainloop(surface)

