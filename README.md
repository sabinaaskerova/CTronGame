# Tron game
Two players game inspired by 'Tron' science-fiction film. Using socket and ncurses libraries in C.<br />
Networking Algorithms class project fall 2022.

## Control keys
Two players on one client:<br />
(first player/second player)<br />
'z'/'i' - cycle up<br />
'q'/'j' - cycle left<br />
's'/'k' - cycle down<br />
'd'/'l' - cycle right<br />
space/'m' - tracing light wall on/off<br />
##
One player on each client:<br />
'z' - cycle up<br />
'q' - cycle left<br />
's' - cycle down<br />
'd' - cycle right<br />
space - tracing light wall on/off<br />

##
Player loses if:<br />
(a) collision with a light wall<br />
(b) collision with an edge wall<br />

Players tie if:<br />
(a) collision with opponent's lightcycle<br />
