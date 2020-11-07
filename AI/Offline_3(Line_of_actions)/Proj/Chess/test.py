# # import subprocess
# # out = subprocess.run('sh test.sh', shell=True, capture_output=True)
# # val = out.stdout.decode()
# # print(val)
# # print(ord(val[0]), ord(val[1]))
# # print(ord(val[2]), ord(val[3]))
# # print(ord(val[4]), ord(val[5]))
# # print(ord(val[6]), ord(val[7]))
# # print()
# # # print(ord(val[8]), ord(val[9]))
# # print(ord(val[10]), ord(val[11]))
# # print(ord(val[12]), ord(val[13]))
# # print(ord(val[14]), ord(val[15]))
# # print(ord(val[16]), ord(val[17]))
# #
#
#
import pygame as p
import pygame_menu

p.init()
surface = p.display.set_mode((600, 400))


def set_difficulty(value, color):
    print(value, color)
    pass

def start_the_game():
    # Do the job here !
    p.init()
    screen = p.display.set_mode((512, 512))
    clock = p.time.Clock()
    screen.fill(p.Color("white"))


menu = pygame_menu.Menu(300, 400, 'Welcome',
                       theme=pygame_menu.themes.THEME_BLUE)

menu.add_text_input('Name :', default='Unnamed')
menu.add_selector('Color :', [('White', 1), ('Black', 0)], onchange=set_difficulty)
menu.add_button('Play', start_the_game)
menu.add_button('Quit', pygame_menu.events.EXIT)

menu.mainloop(surface)

