import pygame
from tile import Tile
from field import Field
from button import Button
import assets
from hive import Hive
from text import Text

class Game:
    def __init__(self):
        pygame.mixer.pre_init(44100, -16, 2, 2048)
        pygame.mixer.init()
        pygame.init()
        pygame.display.set_caption("Hive Game")

        self.hive = -1
        self.round = 0
        self.scale = 1.0
        self.window = pygame.display.set_mode((assets.sizeX, assets.sizeY))
        self.board = pygame.surface.Surface((assets.sizeX*2, assets.sizeY*2), pygame.SRCALPHA, 32)
        self.board = self.board.convert_alpha()
        self.offsetX = 0
        self.offsetY = 0
        assets.mousePos = pygame.mouse.get_pos()
        assets.mousePosScaled = [0,0]
        self.movingBoard = False
        self.rotatingBoard = False
        self.rotatingBoardOffsetY = 0
        self.movingBoardOffsetX = 0
        self.movingBoardOffsetY = 0
        self.assetss = assets.Assets()
        self.runGame = True
        self.font = pygame.font.SysFont('lemonmilk', 64)
        self.background = self.assetss.LoadImage('Background')
        self.scene = 'menu'
        self.clickOff = False

        # Game buttons
        self.buttonPlay = Button(self.window, (191,80,162), (68,166,166), [450, 450, 300, 160], 'Play!', (0,0,0), 128, self.initMode)
        self.buttonModePlayer = Button(self.window, (191,80,162), (68,166,166), [80, 450, 450, 160], 'Player vs Player', (0,0,0), 64, self.setPVP)
        self.buttonModeAI = Button(self.window, (191,80,162), (68,166,166), [700, 450, 450, 160], 'Player vs Computer', (0,0,0), 64, self.setPVC)
        self.buttonPass = Button(self.window, (191,80,162), (68,166,166), [700, 450, 450, 100], 'Skip', (0,0,0), 64, self.skip)
        self.buttonEndMatch = Button(self.window, (191,80,162), (68,166,166), [1040, 650, 150, 50], 'Abort', (0,0,0), 32, self.endCurrentMatch)
        self.buttonQuit = Button(self.window, (191,80,162), (68,166,166), [1040, 700, 150, 50], 'Exit', (0,0,0), 32, self.attemptOff)
        self.buttonEndGame = Button(self.window, (191,80,162), (68,166,166), [500, 600, 280, 150], 'Finish the game', (0,0,0), 64, self.endGame)
        self.buttonWhoFirstPlayer = Button(self.window, (191,80,162), (68,166,166), [80, 450, 450, 160], 'Player', (0,0,0), 128, self.whoFirstPlayer)
        self.buttonWhoFirstComputer = Button(self.window, (191,80,162), (68,166,166), [700, 450, 450, 160], 'Computer', (0,0,0), 128, self.whoFirstComputer)

        # self.sampleText = Text(self.window, 'Siema xd', 64, [240, 200, 250, 150], (0,0,0))
        self.textMoveInfo = Text(self.window, 'Move: ', 64, [910, 200, 250, 150], (59,44,115))
        self.textWhoseMove = Text(self.window, '', 64, [900, 245, 250, 150], (0,0,0))
        self.textWinner = Text(self.window, '', 128, [500, 400, 250, 150], (0,0,0))
        self.textTitle = Text(self.window, 'Hive', 128, [100, 100, 1000, 200], (255,180,236))
        self.textTitle2 = Text(self.window, 'with AI', 128, [350, 250, 500, 100], (45,47,89))
        self.textChooseMode = Text(self.window, 'Choose mode:', 128, [350, 250, 500, 100], (255,255,255))
        self.textWhoPlaysWhite = Text(self.window, 'Who plays as white:', 128, [350, 250, 500, 100], (255,255,255))


    def updateMouse(self):
        assets.mousePos = pygame.mouse.get_pos()
        assets.mousePosScaled[0] = (assets.mousePos[0] - self.offsetX)/self.scale
        assets.mousePosScaled[1] = ((assets.mousePos[1] - self.offsetY)/self.scale)+int(assets.sizeY/2/self.scale)

    def updateEvents(self):
        if self.clickOff == True:
            assets.mouseClicked = False

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.runGame = False
            #
            if event.type == pygame.MOUSEBUTTONDOWN:

                if event.button == 3:
                    if self.movingBoard == False:
                        self.movingBoard = True
                        self.movingBoardOffsetX = assets.mousePos[0]
                        self.movingBoardOffsetY = assets.mousePos[1]
                else:
                    if self.movingBoard == True:
                        self.movingBoard = False

                if event.button == 2:
                    if self.rotatingBoard == False:
                        self.rotatingBoard = True
                        self.rotatingBoardOffsetY = assets.mousePos[1]
                else:
                    if self.rotatingBoard == True:
                        self.rotatingBoard = False


            if event.type == pygame.MOUSEBUTTONUP:
                if event.button == 3:
                    self.movingBoard = False
                if event.button == 2:
                    self.rotatingBoard = False
                if event.button == 1:
                    assets.mouseClicked = True
                    self.clickOff = True

            if self.movingBoard == True:
                self.offsetX -= self.movingBoardOffsetX-assets.mousePos[0]
                self.offsetY -= self.movingBoardOffsetY-assets.mousePos[1]
                self.movingBoardOffsetX = assets.mousePos[0]
                self.movingBoardOffsetY = assets.mousePos[1]

            if self.rotatingBoard == True:
                difference = self.rotatingBoardOffsetY-assets.mousePos[1]
                self.scale += difference/200
                if self.scale > 1.2:
                    self.scale = 1.2
                elif self.scale < 0.6:
                    self.scale = 0.6
                else:
                    self.offsetX -= difference*2.8
                    self.offsetY -= difference*3.8
                self.rotatingBoardOffsetY = assets.mousePos[1]

    def endGame(self):
        self.runGame = False

    def skip(self):
        assets.sharedInfo['tileMoved'] = "pass"
        assets.sharedInfo['messageType'] = "Pass"
        assets.sharedInfo['socket'] = True
        self.board.fill((255,255,255,0))

    def setPVP(self):
        self.hive = Hive(self.window, self.board)
        assets.sharedInfo['matchType'] = 'PvP'
        assets.sharedInfo['socket'] = True
        self.initPlay()

    def setPVC(self):
        self.scene = 'whoFirst'

    def whoFirstPlayer(self):
        self.hive = Hive(self.window, self.board)
        assets.sharedInfo['ComputerFirst'] = False
        assets.sharedInfo['socket'] = True
        assets.sharedInfo['matchType'] = 'PvC'
        self.initPlay()

    def whoFirstComputer(self):
        self.hive = Hive(self.window, self.board)
        assets.sharedInfo['ComputerFirst'] = True
        assets.sharedInfo['socket'] = True
        assets.sharedInfo['matchType'] = 'PvC'
        self.initPlay()

    def initMode(self):
        self.scene = 'mode'
        self.board.fill((255,255,255,0))

    def initPlay(self):
        self.scene = 'game'
        self.board.fill((255,255,255,0))

    def run(self):
        while self.runGame:
            self.loop()
            pygame.display.update(pygame.Rect(0,0,assets.sizeX, assets.sizeY))
            # pygame.display.flip()
        
    def attemptOff(self):
        assets.sharedInfo['messageType'] = 'Quit'
        assets.sharedInfo['socket'] = True
        self.runGame = False

    def loop(self):
        if assets.matchEnd == True:
            self.endCurrentMatch()
        self.updateMouse()
        self.updateEvents()
        self.window.blit(self.background, (0,0))

        if self.scene is 'menu':
            self.sceneMenu()
        elif self.scene is 'mode':
            self.sceneMode()
        elif self.scene is 'whoFirst':
            self.sceneWho()
        else:
            self.sceneGame()

    def sceneMenu(self):
        self.textTitle.draw()
        self.textTitle2.draw()
        self.buttonPlay.draw()
        self.window.blit(self.board, (self.offsetX, self.offsetY))
        if assets.mouseClicked is True:
            self.board.fill((255,255,255,0))

    def sceneWho(self):
        self.buttonWhoFirstPlayer.draw()
        self.buttonWhoFirstComputer.draw()
        self.textWhoPlaysWhite.draw()
        self.window.blit(self.board, (self.offsetX, self.offsetY))
        if assets.mouseClicked is True:
            self.board.fill((255,255,255,0))

    def sceneMode(self):
        self.textChooseMode.draw()
        self.buttonModeAI.draw()
        self.buttonModePlayer.draw()
        self.window.blit(self.board, (self.offsetX, self.offsetY))
        if assets.mouseClicked is True:
            self.board.fill((255,255,255,0))

    def sceneGame(self):
        if assets.sharedInfo['nextMove'] == True:

            if assets.sharedInfo['AI_Move'] == True:
                self.hive.moveTile(self.hive.getTileByName(assets.sharedInfo['AI_Which']), assets.sharedInfo['AI_Where'][0], assets.sharedInfo['AI_Where'][1])

            if assets.sharedInfo['GameOver'] == True:
                self.buttonEndGame.draw()
                if assets.sharedInfo['Winner'] is 'w':
                    self.textWinner.updateText('Winner: White!')
                    self.textWinner.color = (255,255,255)
                elif assets.sharedInfo['Winner'] is 'b':
                    self.textWinner.updateText('Winner: White!')
                    self.textWinner.color = (0,0,0)
                else:
                    self.textWinner.updateText('Remis!')
                    self.textWinner.color = (150,150,150)
                self.textWinner.draw()


            if assets.sharedInfo['whoMoves'] is 'w':
                self.textWhoseMove.updateText('White')
                self.textWhoseMove.color = (255,255,255)
            else:
                self.textWhoseMove.updateText('Black')
                self.textWhoseMove.color = (0,0,0)

            if len(self.hive.tilesPlayed_) == 0 and assets.sharedInfo['ComputerFirst'] is False:
                assets.sharedInfo['placements'] = [[0,0]]

            self.hive.generate()

        self.hive.draw()
        if len(assets.sharedInfo['moves']) == 0 and len(assets.sharedInfo['placements']) == 0:
            self.buttonPass.draw()

        self.buttonEndMatch.draw()
        self.buttonQuit.draw()
        self.window.blit(pygame.transform.smoothscale(self.board, (int(assets.sizeX*self.scale)*2, int(assets.sizeY*self.scale)*2)), (self.offsetX, self.offsetY-int(assets.sizeY/2)))

        self.textMoveInfo.draw()
        self.textWhoseMove.draw()

    def endCurrentMatch(self):
        assets.matchEnd = False
        assets.sharedInfo['socket'] = False
        assets.sharedInfo['matchType'] = '-'
        assets.sharedInfo['GameOver'] = False
        assets.sharedInfo['Winner'] = '-'
        assets.sharedInfo['round'] = 0
        assets.sharedInfo['roundBlack'] = 0
        assets.sharedInfo['roundWhite'] = 0
        assets.sharedInfo['whoMoves'] = 'w'
        assets.sharedInfo['moves'] = {}
        assets.sharedInfo['placements'] = []
        assets.sharedInfo['nextMove'] = False
        assets.sharedInfo['tileMoved'] = '-'
        assets.sharedInfo['tileMovedWhere'] = [0,0]
        assets.sharedInfo['messageType'] = 'EndMatch'
        
        del self.hive
        self.hive = -1
        self.round = 0
        self.scale = 1.0
        self.offsetX = 0
        self.offsetY = 0
        self.scene = 'menu'
        assets.sharedInfo['socket'] = True
        self.board.fill((255,255,255,0))