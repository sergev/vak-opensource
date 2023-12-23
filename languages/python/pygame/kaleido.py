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

# Draw eight circles of random color.
def draw_circles(x, y, r):
    color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.circle(screen, color, (xc + x, yc + y), r)
    pygame.draw.circle(screen, color, (xc - x, yc + y), r)
    pygame.draw.circle(screen, color, (xc - x, yc - y), r)
    pygame.draw.circle(screen, color, (xc + x, yc - y), r)
    pygame.draw.circle(screen, color, (xc + y, yc + x), r)
    pygame.draw.circle(screen, color, (xc - y, yc + x), r)
    pygame.draw.circle(screen, color, (xc - y, yc - x), r)
    pygame.draw.circle(screen, color, (xc + y, yc - x), r)

    border_color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.circle(screen, border_color, (xc + x, yc + y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - x, yc + y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - x, yc - y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc + x, yc - y), r, width=1)
    pygame.draw.circle(screen, border_color, (xc + y, yc + x), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - y, yc + x), r, width=1)
    pygame.draw.circle(screen, border_color, (xc - y, yc - x), r, width=1)
    pygame.draw.circle(screen, border_color, (xc + y, yc - x), r, width=1)

# Draw eight boxes of random color.
def draw_boxes(x, y, r):
    color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.rect(screen, color, (xc + x - r//2, yc + y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - x - r//2, yc + y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - x - r//2, yc - y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc + x - r//2, yc - y - r//2, r, r))
    pygame.draw.rect(screen, color, (xc + y - r//2, yc + x - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - y - r//2, yc + x - r//2, r, r))
    pygame.draw.rect(screen, color, (xc - y - r//2, yc - x - r//2, r, r))
    pygame.draw.rect(screen, color, (xc + y - r//2, yc - x - r//2, r, r))

    border_color = (randrange(256), randrange(256), randrange(256))
    pygame.draw.rect(screen, border_color, (xc + x - r//2, yc + y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - x - r//2, yc + y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - x - r//2, yc - y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc + x - r//2, yc - y - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc + y - r//2, yc + x - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - y - r//2, yc + x - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc - y - r//2, yc - x - r//2, r, r), width=1)
    pygame.draw.rect(screen, border_color, (xc + y - r//2, yc - x - r//2, r, r), width=1)

# Quit on window close, any button click or key press.
def is_quit(event):
    if event.type == pygame.QUIT:
        return True
    if event.type == pygame.MOUSEBUTTONDOWN:
        return True
    if event.type == pygame.TEXTINPUT:
        return True
    if event.type == pygame.KEYDOWN:
        if event.key == pygame.K_RETURN:
            return True
        if event.key == pygame.K_ESCAPE:
            return True
    return False

# Main loop.
while True:

    # Update the display.
    pygame.display.update()
    pygame.time.wait(10)

    # Process user input.
    for event in pygame.event.get():
        if is_quit(event):
            pygame.quit()
            sys.exit(0)

    # Fade existing contents by 3%.
    surface = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.SRCALPHA)
    surface.fill((0, 0, 0, 3))
    screen.blit(surface, (0, 0))

    # Draw eight random circles.
    x = randrange(xc)
    y = randrange(yc)
    r = 5 + randrange(25)
    draw_circles(x, y, r)

    # Draw eight random boxes.
    x = randrange(xc)
    y = randrange(yc)
    r = 5 + randrange(25)
    draw_boxes(x, y, r)
