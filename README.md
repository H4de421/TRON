# TRON

TRON is a simple game based on the 1982 TRON movie.  
In this game, 2 bikes ride on a grid, leaving an uncrossable trail behind them.  
The last player still riding is the winner.

---

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

---

## Contributing

The main purpose of this project is to discover new aspects of the C language through a fun and complete project.  
This is a personal project, so no code contributions will be accepted. However, any advice is welcome.

