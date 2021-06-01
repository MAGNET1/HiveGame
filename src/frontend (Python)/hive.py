from tile import Tile
from field import Field
import assets
import time

class Hive:
    def __init__(self, window, board):
        self.assetss = assets.Assets()
        self.board = board
        self.window = window
        self.move_ = 0
        self.gap = 250
        self.distance = int((assets.sizeX-300)/5)
        self.tilesPlayed_ = {}
        self.tilesUnplayed_ = []
        self.tilesUnplayed_.append({})
        self.tilesUnplayed_.append({})
        self.tileToChange = False
        self.tileToChangeCoordsX = 0
        self.tileToChangeCoordsY = 0
        self.shouldClearBoard = False
        self.tilesUnplayed_[0]['bQ1'] = Tile(self, 'bQ1', self.gap, 75, self.assetss.LoadImage('QueenBlack'), self.window, self.board, 'b', False)
        self.tilesUnplayed_[0]['bA3'] = Tile(self, 'bA3', self.gap+self.distance, 55, self.assetss.LoadImage('AntBlack'), self.window, self.board, 'b', False, True)
        self.tilesUnplayed_[0]['bA2'] = Tile(self, 'bA2', self.gap+self.distance, 65, self.assetss.LoadImage('AntBlack'), self.window, self.board, 'b', False, True)
        self.tilesUnplayed_[0]['bA1'] = Tile(self, 'bA1', self.gap+self.distance, 75, self.assetss.LoadImage('AntBlack'), self.window, self.board, 'b', False)
        self.tilesUnplayed_[0]['bG3'] = Tile(self, 'bG3', self.gap+self.distance*2, 55, self.assetss.LoadImage('GrasshopperBlack'), self.window, self.board, 'b', False, True)
        self.tilesUnplayed_[0]['bG2'] = Tile(self, 'bG2', self.gap+self.distance*2, 65, self.assetss.LoadImage('GrasshopperBlack'), self.window, self.board, 'b', False, True)
        self.tilesUnplayed_[0]['bG1'] = Tile(self, 'bG1', self.gap+self.distance*2, 75, self.assetss.LoadImage('GrasshopperBlack'), self.window, self.board, 'b', False)
        self.tilesUnplayed_[0]['bS2'] = Tile(self, 'bS2', self.gap+self.distance*3, 65, self.assetss.LoadImage('SpiderBlack'), self.window, self.board, 'b', False, True)
        self.tilesUnplayed_[0]['bS1'] = Tile(self, 'bS1', self.gap+self.distance*3, 75, self.assetss.LoadImage('SpiderBlack'), self.window, self.board, 'b', False)
        self.tilesUnplayed_[0]['bB2'] = Tile(self, 'bB2', self.gap+self.distance*4, 65, self.assetss.LoadImage('BeetleBlack'), self.window, self.board, 'b', False, True)
        self.tilesUnplayed_[0]['bB1'] = Tile(self, 'bB1', self.gap+self.distance*4, 75, self.assetss.LoadImage('BeetleBlack'), self.window, self.board, 'b', False)

        self.tilesUnplayed_[1]['wQ1'] = Tile(self, 'wQ1', self.gap, assets.sizeY-75, self.assetss.LoadImage('QueenWhite'), self.window, self.board, 'w', False)
        self.tilesUnplayed_[1]['wA3'] = Tile(self, 'wA3', self.gap+self.distance, assets.sizeY-55, self.assetss.LoadImage('AntWhite'), self.window, self.board, 'w', False, True)
        self.tilesUnplayed_[1]['wA2'] = Tile(self, 'wA2', self.gap+self.distance, assets.sizeY-65, self.assetss.LoadImage('AntWhite'), self.window, self.board, 'w', False, True)
        self.tilesUnplayed_[1]['wA1'] = Tile(self, 'wA1', self.gap+self.distance, assets.sizeY-75, self.assetss.LoadImage('AntWhite'), self.window, self.board, 'w', False)
        self.tilesUnplayed_[1]['wG3'] = Tile(self, 'wG3', self.gap+self.distance*2, assets.sizeY-55, self.assetss.LoadImage('GrasshopperWhite'), self.window, self.board, 'w', False, True)
        self.tilesUnplayed_[1]['wG2'] = Tile(self, 'wG2', self.gap+self.distance*2, assets.sizeY-65, self.assetss.LoadImage('GrasshopperWhite'), self.window, self.board, 'w', False, True)
        self.tilesUnplayed_[1]['wG1'] = Tile(self, 'wG1', self.gap+self.distance*2, assets.sizeY-75, self.assetss.LoadImage('GrasshopperWhite'), self.window, self.board, 'w', False)
        self.tilesUnplayed_[1]['wS2'] = Tile(self, 'wS2', self.gap+self.distance*3, assets.sizeY-65, self.assetss.LoadImage('SpiderWhite'), self.window, self.board, 'w', False, True)
        self.tilesUnplayed_[1]['wS1'] = Tile(self, 'wS1', self.gap+self.distance*3, assets.sizeY-75, self.assetss.LoadImage('SpiderWhite'), self.window, self.board, 'w', False)
        self.tilesUnplayed_[1]['wB2'] = Tile(self, 'wB2', self.gap+self.distance*4, assets.sizeY-65, self.assetss.LoadImage('BeetleWhite'), self.window, self.board, 'w', False, True)
        self.tilesUnplayed_[1]['wB1'] = Tile(self, 'wB1', self.gap+self.distance*4, assets.sizeY-75, self.assetss.LoadImage('BeetleWhite'), self.window, self.board, 'w', False)

    def draw(self):
        if self.tileToChange != False:
            self.performMovement()
            self.board.fill((255,255,255,0))
        if self.shouldClearBoard:
            self.board.fill((255,255,255,0))
            self.shouldClearBoard = False
        for tileBlack in self.tilesUnplayed_[0].values():
            tileBlack.draw()
        for tileWhite in self.tilesUnplayed_[1].values():
            tileWhite.draw()

        # we want to draw beetles at the very top, since they may overlap with other bugs once they stand onto one
        beetlesTab = []
        for tile in self.tilesPlayed_.values():
            if 'B' in tile.name:
                beetlesTab.append(tile)
            else:
                tile.draw()
        
        # there can also be beetle on top of another beetle. For this reason, we draw blocked beetles first...
        for beetle in beetlesTab:
            if beetle.isUnderBeetle != False:
                beetle.draw()

        # and now the beetles at the very top
        for beetle in beetlesTab:
            if beetle.isUnderBeetle == False:
                beetle.draw()


    def generate(self):
        for tileBlack in self.tilesUnplayed_[0].values():
            tileBlack.generateFields()
        for tileWhite in self.tilesUnplayed_[1].values():
            tileWhite.generateFields()

        for tile in self.tilesPlayed_.values():
            tile.generateFields()

    def getTileByName(self, tileName):
        print(f'Szukam: {tileName}')
        whichArray = 1
        if tileName[0] is 'b':
            whichArray = 0

        for tile in self.tilesUnplayed_[whichArray].values():
            print(f'Znaleziono: {tile.name}')
            if tile.name == tileName:
                print(f'BUYA!')
                return tile

        for tile in self.tilesPlayed_.values():
            print(f'Znaleziono: {tile.name}')
            if tile.name == tileName:
                return tile

    def moveTile(self, tile, X, Y):
        print(f'Figura {tile.name} idzie na pole {X},{Y}')
        self.assetss.PlaySound_Place()
        self.tileToChange = tile
        self.tileToChangeCoordsX = X
        self.tileToChangeCoordsY = Y

    def performMovement(self):
        if self.tileToChange.played == False:
            self.tileToChange.played = True
            self.tileToChange.surface = self.board
            self.tilesPlayed_[self.tileToChange.name] = self.tileToChange

            nextUnplayedTile = self.findNextUnplayedTile(self.tileToChange.name)
            if self.tileToChange.color == 'b':
                del self.tilesUnplayed_[0][self.tileToChange.name]
                if nextUnplayedTile != -1:
                    self.tilesUnplayed_[0][nextUnplayedTile].isBlocked = False
            else:
                del self.tilesUnplayed_[1][self.tileToChange.name]
                if nextUnplayedTile != -1:
                    self.tilesUnplayed_[1][nextUnplayedTile].isBlocked = False

        # we unlock tiles being blocked by the beetle and giving a lock to the new ones being impaired
        if 'B' in self.tileToChange.name:
            for tile in self.tilesPlayed_.values():
                if tile.name != self.tileToChange.name:
                    if tile.centerX == self.tileToChange.centerX and tile.centerY == self.tileToChange.centerY and tile.isUnderBeetle == self.tileToChange.name:
                        tile.isUnderBeetle = False
                        self.tileToChange.isOnTop = False
                    if tile.centerX == self.tileToChangeCoordsX and tile.centerY == self.tileToChangeCoordsY and tile.isUnderBeetle == False:
                        tile.isUnderBeetle = self.tileToChange.name
                        self.tileToChange.isOnTop = True

        self.tileToChange.move(self.tileToChangeCoordsX, self.tileToChangeCoordsY)

        if assets.sharedInfo['AI_Move'] == False:
            assets.sharedInfo['tileMovedWhere'] = [self.tileToChangeCoordsX,self.tileToChangeCoordsY]
            assets.sharedInfo['tileMoved'] = self.tileToChange.name
            assets.sharedInfo['messageType'] = 'Move'
            assets.sharedInfo['socket'] = True
        else:
            assets.sharedInfo['AI_Move'] = False
        self.tileToChange = False


    def findNextUnplayedTile(self, name):
        tileColor = name[0]
        tileType = name[1]
        tileNumber = name[2]

        if tileType == 'Q':
            return -1
        elif tileType == 'A':
            if tileNumber == '1':
                return tileColor+'A2'
            elif tileNumber == '2':
                return tileColor+'A3'
            else:
                return -1
        elif tileType == 'G':
            if tileNumber == '1':
                return tileColor+'G2'
            elif tileNumber == '2':
                return tileColor+'G3'
            else:
                return -1
        elif tileType == 'S':
            if tileNumber == '1':
                return tileColor+'S2'
            else:
                return -1
        elif tileType == 'B':
            if tileNumber == '1':
                return tileColor+'B2'
            else:
                return -1
        return -1
            


