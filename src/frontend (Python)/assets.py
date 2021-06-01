import pygame
import os
import random

def init():
    global sharedInfo
    global mousePos
    global mousePosScaled
    global mouseClicked
    global matchEnd
    global sizeX
    global sizeY

    sharedInfo = {
        'socket': False,
        'matchType': '-',
        'ComputerFirst': False,
        'messageType': '-',
        'GameOver': False,
        'Winner': '-',
        'round': 0,
        'roundBlack': 0,
        'roundWhite': 0,
        'whoMoves': 'w',
        'moves': {},
        'placements': [],
        'nextMove': False,
        'tileMoved': '-',
        'tileMovedWhere': [0,0],
        'AI_Move': False,
        'AI_Which': '',
        'AI_Where': [0,0]
    }

    mousePos = [0,0]
    mousePosScaled = [0,0]
    mouseClicked = False
    matchEnd = False
    sizeX = 1200
    sizeY = 800

class Assets:
    def __init__(self):
        self.background = pygame.image.load(os.path.join("Images", "background.jpg")).convert_alpha()

        self.QueenWhite = pygame.image.load(os.path.join("Tiles", "QueenWhite.png")).convert_alpha()
        self.QueenBlack = pygame.image.load(os.path.join("Tiles", "QueenBlack.png")).convert_alpha()
        self.AntWhite = pygame.image.load(os.path.join("Tiles", "AntWhite.png")).convert_alpha()
        self.AntBlack = pygame.image.load(os.path.join("Tiles", "AntBlack.png")).convert_alpha()
        self.BeetleWhite = pygame.image.load(os.path.join("Tiles", "BeetleWhite.png")).convert_alpha()
        self.BeetleBlack = pygame.image.load(os.path.join("Tiles", "BeetleBlack.png")).convert_alpha()
        self.GrasshopperWhite = pygame.image.load(os.path.join("Tiles", "GrasshopperWhite.png")).convert_alpha()
        self.GrasshopperBlack = pygame.image.load(os.path.join("Tiles", "GrasshopperBlack.png")).convert_alpha()
        self.SpiderWhite = pygame.image.load(os.path.join("Tiles", "SpiderWhite.png")).convert_alpha()
        self.SpiderBlack = pygame.image.load(os.path.join("Tiles", "SpiderBlack.png")).convert_alpha()

        self.soundPlaceTab = []
        for i in range(1,5):
            self.soundPlaceTab.append(pygame.mixer.Sound('Sounds/tilePlace_'+str(i)+'.wav'))

    def LoadImage(self, name):
        if name == 'Background':
            return self.background
        elif name == 'QueenWhite':
            return self.QueenWhite
        elif name == 'QueenBlack':
            return self.QueenBlack
        elif name == 'AntWhite':
            return self.AntWhite
        elif name == 'AntBlack':
            return self.AntBlack
        elif name == 'BeetleWhite':
            return self.BeetleWhite
        elif name == 'BeetleBlack':
            return self.BeetleBlack
        elif name == 'GrasshopperWhite':
            return self.GrasshopperWhite
        elif name == 'GrasshopperBlack':
            return self.GrasshopperBlack
        elif name == 'SpiderWhite':
            return self.SpiderWhite
        elif name == 'SpiderBlack':
            return self.SpiderBlack

    def PlaySound_Place(self):
        self.soundPlaceTab[int(random.uniform(0,len(self.soundPlaceTab)))].play()
            