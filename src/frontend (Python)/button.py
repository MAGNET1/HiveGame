import pygame
import time
import assets

class Button:
    def __init__(self, surface, colorIdle, colorHovered, pos, text, textColor, textSize, function):
        self.surface = surface
        self.colorIdle = colorIdle
        self.colorHovered = colorHovered
        self.pos = pos
        self.text = text
        self.textColor = textColor
        self.function = function
        self.lastClicked = 0
        self.font = pygame.font.SysFont('lemonmilk', textSize)

    def isHovered(self):
        if assets.mousePos[0] > self.pos[0] and assets.mousePos[0] < self.pos[0] + self.pos[2] and assets.mousePos[1] > self.pos[1] and assets.mousePos[1] < self.pos[1] + self.pos[3]:
            return True
        return False

    def draw(self):
        textC = self.colorIdle
        if self.isHovered():
            textC = self.colorHovered
            pygame.draw.rect(self.surface, self.colorHovered, self.pos)
            if assets.mouseClicked is True:
                if int(time.time()) > self.lastClicked:
                    self.lastClicked = int(time.time())
                    self.function()
        else:
            pygame.draw.rect(self.surface, self.colorIdle, self.pos)

        text = self.font.render(self.text, True, self.textColor, textC)
        textRect = text.get_rect()
        textRect.center = (self.pos[0] + (self.pos[2]//2), self.pos[1] + (self.pos[3]//2))
        self.surface.blit(text, textRect)