import pygame
import math
import numpy as np
from shapely.geometry import Point
from shapely.geometry.polygon import Polygon
import assets

class Field:
    def __init__(self, belongsTo, centerX, centerY, surface):
        self.belongsTo = belongsTo
        self.centerX = centerX
        self.centerY = centerY
        self.realCenterX = self.centerX
        self.realCenterY = self.centerY
        self.convertCoords()
        self.radius = 50
        self.surface = surface
        self.vertexes = self.CalculateVertexes()
        self.hovered = False
        self.clicked = False

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

    def isHovered(self):
        if Polygon(self.vertexes).contains(Point(assets.mousePosScaled[0],assets.mousePosScaled[1])):
            return True
        return False

    def draw(self):

        self.hovered = self.isHovered()

        if self.hovered == True:
            pygame.draw.lines(self.surface, (122,184,0), True, self.vertexes, 6)
        else:
            pygame.draw.lines(self.surface, (237,46,12), True, self.vertexes, 6)
        
        if assets.mouseClicked == True:
            if self.hovered == True:
                self.clicked = True
                self.belongsTo.fieldClicked(self.centerX, self.centerY)
        else:
            self.clicked = False

    def move(self, x, y):
        self.centerX = x
        self.centerY = y
        self.realCenterX = self.centerX
        self.realCenterY = self.centerY
        self.convertCoords()
        self.vertexes = self.CalculateVertexes()