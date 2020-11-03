import pygame as p
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
	while running:
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
					sqSeleceted = ()
					playerClicks = []



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

