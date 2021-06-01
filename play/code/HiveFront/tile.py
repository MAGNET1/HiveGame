import pygame
import math
import numpy as np
from shapely.geometry import Point
from shapely.geometry.polygon import Polygon
import assets
from field import Field

class Tile:
    def __init__(self, hive, name, centerX, centerY, bug, surface, boardSurface, color='w', played=True, blocked=False):
        self.hive = hive
        self.name = name
        self.centerX = centerX
        self.centerY = centerY
        self.realCenterX = self.centerX
        self.realCenterY = self.centerY
        if played == True:
            self.convertCoords()
        self.radius = 50
        self.bug = bug
        self.surface = surface
        self.boardSurface = boardSurface
        self.vertexes = self.CalculateVertexes()
        self.isBlocked = blocked
        self.color = color
        self.played = played
        self.hovered = False
        self.clicked = False
        self.marked = False
        self.isUnderBeetle = False
        self.isOnTop = False
        self.fields = []

    def CalculateVertexes(self):
        v = []
        for i in [30,90,150,210,270,330]:
            v.append((math.sin(math.radians(i))*self.radius+self.realCenterX, (math.cos(math.radians(i))*self.radius+self.realCenterY)))
        return v

    def convertCoords(self):
        self.realCenterX = int(assets.sizeX/2)
        self.realCenterY = int(assets.sizeY/2)
        self.realCenterX += self.centerX*76
        self.realCenterY -= self.centerY*43
        self.realCenterY += int(assets.sizeY/2)

        if self.isOnTop == True:
            self.realCenterY -= 10

    def generateFields(self):
        self.fields.clear()

        if assets.sharedInfo['whoMoves'] == self.color:
            if self.played == True:
                if self.name in assets.sharedInfo['moves']:
                    for coords in assets.sharedInfo['moves'][self.name]:
                        self.fields.append(Field(self, coords[0], coords[1], self.boardSurface))
            else:
                for coords in assets.sharedInfo['placements']:
                    self.fields.append(Field(self, coords[0], coords[1], self.boardSurface))

    def isHovered(self):
        # can't focus on tile if: It's blocked (in queue, unplayed), its color does not match the player, It's a queen in 1st turn or it's 4th move - only queen can be introduced onto the board if it hasn't been already
        
        # LEGACY IF STATEMENT
        # if self.isBlocked == True or self.color is not assets.playerTurn or (assets.sharedInfo['round'] < 2 and 'Q' in self.name) or ((assets.sharedInfo['round'] == 6 or assets.sharedInfo['round'] == 7) and 'Q' not in self.name and self.color+'Q1' not in self.hive.tilesPlayed_):
        #     return False 

        if self.isUnderBeetle != False or self.isBlocked == True or self.color is not assets.sharedInfo['whoMoves'] or (assets.sharedInfo['round'] < 2 and 'Q' in self.name) or ((self.color is 'b' and assets.sharedInfo['roundBlack'] == 3 and self.name is not 'bQ1' and 'bQ1' not in self.hive.tilesPlayed_) or (self.color is 'w' and assets.sharedInfo['roundWhite'] == 3 and self.name is not 'wQ1' and 'wQ1' not in self.hive.tilesPlayed_)):
            return False

        if self.played == True:
            if Polygon(self.vertexes).contains(Point(assets.mousePosScaled[0],assets.mousePosScaled[1])):
                return True
            else:
                return False
        else:
            if Polygon(self.vertexes).contains(Point(assets.mousePos[0],assets.mousePos[1])):
                return True
            else:
                return False
        return False

    def fieldClicked(self, x, y):
        self.hive.moveTile(self, x, y)

    def draw(self):

        self.hovered = self.isHovered()

        if self.marked == True:
            pygame.draw.polygon(self.surface, (237,46,12), self.vertexes)
            for field in self.fields:
                field.draw()
        elif self.hovered == True:
            if self.played == True:
                pygame.draw.polygon(self.surface, (245,56,232), self.vertexes)
            else:
                pygame.draw.polygon(self.surface, (56,191,245), self.vertexes)
        else:
            pygame.draw.polygon(self.surface, (79,79,79), self.vertexes)
        
        if assets.mouseClicked == True:
            oldMarked = self.marked
            self.marked = self.hovered
            if oldMarked != self.marked and oldMarked == True:
                self.hive.shouldClearBoard = True
            if self.hovered == True:
                self.clicked = True
        else:
            self.clicked = False
        self.surface.blit(pygame.transform.smoothscale(self.bug, (100, 100)), (self.realCenterX-51, self.realCenterY-51))

    def move(self, x, y):
        self.centerX = x
        self.centerY = y
        self.realCenterX = self.centerX
        self.realCenterY = self.centerY
        if self.played == True:
            self.convertCoords()
        self.vertexes = self.CalculateVertexes()