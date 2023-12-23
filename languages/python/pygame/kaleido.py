#!/usr/bin/env python3
#
# Kaleidoscope rewritten from BGI to PyGame.
# By Guido Gonzato, December 2018
# By Serge Vakulenko, December 2023
#
import pygame, sys
from random import randrange

# Initialize pygame.
SCREEN_WIDTH = 600
SCREEN_HEIGHT = 600
pygame.init()

# Create the screen object.
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
screen.fill((0, 0, 0))

# Center of the screen.
xc = SCREEN_WIDTH // 2
yc = SCREEN_HEIGHT // 2

def rnd_circles(x, y, r):
    color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.circle(screen, color, (xc + x, yc + y), r)
    pygame.draw.circle(screen, color, (xc - x, yc + y), r)
    pygame.draw.circle(screen, color, (xc - x, yc - y), r)
    pygame.draw.circle(screen, color, (xc + x, yc - y), r)
    pygame.draw.circle(screen, color, (xc + y, yc + x), r)
    pygame.draw.circle(screen, color, (xc - y, yc + x), r)
    pygame.draw.circle(screen, color, (xc - y, yc - x), r)
    pygame.draw.circle(screen, color, (xc + y, yc - x), r)
    # outlines
    border_color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.circle(screen, border_color, (xc + x, yc + y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - x, yc + y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - x, yc - y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc + x, yc - y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc + y, yc + x), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - y, yc + x), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - y, yc - x), r, width=1)
    pygame.draw.circle(screen, border_color, (xc + y, yc - x), r, width=1)

def rnd_bars(x, y, r):
    color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.rect(screen, color, (xc + x - r//2, yc + y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - x - r//2, yc + y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - x - r//2, yc - y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc + x - r//2, yc - y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc + y - r//2, yc + x - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - y - r//2, yc + x - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - y - r//2, yc - x - r//2, r, r))
    pygame.draw.rect(screen, color, (xc + y - r//2, yc - x - r//2, r, r))
    # outlines
    border_color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.rect(screen, border_color, (xc + x - r//2, yc + y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - x - r//2, yc + y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - x - r//2, yc - y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc + x - r//2, yc - y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc + y - r//2, yc + x - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - y - r//2, yc + x - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - y - r//2, yc - x - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc + y - r//2, yc - x - r//2, r, r), width=1)

# Run until the user asks to quit.
while True:

    # Update the display.
    pygame.time.wait(10)
    pygame.display.update()

    # Exit on any key press, button click or window close.
    for event in pygame.event.get():
        if event.type == pygame.QUIT or \
           event.type == pygame.TEXTINPUT or \
           (event.type == pygame.KEYDOWN and event.key == pygame.K_RETURN) or \
           event.type == pygame.MOUSEBUTTONDOWN:
            pygame.quit()
            sys.exit(0)

    # Fade existing contents.
    surface = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.SRCALPHA)
    surface.fill((0, 0, 0, 4))
    screen.blit(surface, (0, 0))

    # Define randrange position and radius of a circle.
    x = randrange(xc)
    y = randrange(yc)
    r = 5 + randrange(25)

    # Draw 8 filled circles.
    rnd_circles(x, y, r)

    # Define a randrange box.
    x = randrange(xc)
    y = randrange(yc)
    r = 5 + randrange(25)

    # Draw 8 boxes.
    rnd_bars(x, y, r)
