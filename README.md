# Tron game
Two players game inspired by 'Tron' science-fiction film. Using socket and ncurses libraries in C.
Networking Algorithms class project fall 2022.

## Control keys
Two players on one client:
(first player/second player)
'z'/'i' - cycle up
'q'/'j' - cycle left
's'/'k' - cycle down
'd'/'l' - cycle right
space/'m' - tracing light wall on/off

One player on each client:
'z' - cycle up
'q' - cycle left
's' - cycle down
'd' - cycle right
space - tracing light wall on/off

Player loses if:
(a) collision with a light wall
(b) collision with an edge wall

Players tie if:
(a) collision with opponent's lightcycle
