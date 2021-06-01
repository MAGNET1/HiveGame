import pygame

class Text:
    def __init__(self, surface, text, textSize, pos, color):
        self.surface = surface
        self.pos = pos
        self.color = color
        self.text = text
        self.lastClicked = 0
        self.font = pygame.font.SysFont('lemonmilk', textSize)

    def draw(self):
        text = self.font.render(self.text, True, self.color)
        textRect = text.get_rect()
        textRect.center = (self.pos[0] + (self.pos[2]//2), self.pos[1] + (self.pos[3]//2))
        self.surface.blit(text, textRect)

    def updateText(self, newText):
        self.text = newText