# TRON
===============

TRON is a simple game based on the 1982 TRON movie.  
In this game, 2 bikes ride on a grid, leaving an uncrossable trail behind them.  
The last player still riding is the winner.

---

## how to play

This game can be played on keyboard only.
current keymaps:

MainMenu:
 - z move cursor up 
 - s move cursor down
 - e select an option (enter)
 - a abort

Game:
 - z move up
 - d move right
 - q move left
 - s move down

## Project specification

### Interactive CLI

This game runs **only** in the terminal, using ASCII characters to draw the grid.  
All graphics operations are done on a separate thread to always keep a fluid display.  
When starting the program, the size of the grid is computed based on the terminal size.  
/!\ Resizing the terminal will not resize the grid /!\.

### Input handling

As the game can run without any user input, the game loop is detached from the main thread.  
However, since the main loop and input loop may access the same variables at the same time,  
**mutexes** are used to avoid any data races.  

Also, the terminal must be set to **raw mode**.  
Usually, user input is buffered and processed only when a `\n` (Enter) is received.  
As our game needs to be responsive (125ms per frame), we don't want the user to press Enter between each move.  
Raw mode allows inputs to be processed one by one, unbuffered.  

At the end of the program, the terminal will return to its previous state, even if the program is interrupted by a signal.

### AI

Work in progress.

for now multiples ways are considered.

---

## ROADMAP

Here is a list of features I have considered and explored.
All features on this list are awaiting integration into this project.

+ multiplayer (local server)

+ AI implémentation 

## possibles futures features

This unordered list gathers all the simple ideas that came to me for this project.
These aren't in-depth reflections, but simply interesting features that I think are worth noting.
None of these features are planned for implementation in the near future.

+ pause 

+ bindable controls

+ multiplayer local (2 user on same keyboard) / Tmux intergation

+ score board


---

## Contributing

The main purpose of this project is to discover new aspects of the C language through a fun and complete project.  
This is a personal project, so no code contributions will be accepted. However, any advice is welcome.

