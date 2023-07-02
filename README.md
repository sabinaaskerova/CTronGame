# Tron game
Two players game inspired by 'Tron' science-fiction film. Using socket and ncurses libraries in C.<br />
Networking Algorithms class project fall 2022.
<img width="1268" alt="Screenshot 2023-07-02 at 21 29 57" src="https://github.com/sabinaaskerova/CTronGame/assets/91430159/3cc66b2f-1530-4be2-ac20-a07f7cbf875d">

## Control keys
Two players on one client:<br />
(first player/second player)<br />
'z'/'i' - cycle up<br />
'q'/'j' - cycle left<br />
's'/'k' - cycle down<br />
'd'/'l' - cycle right<br />
space/'m' - tracing light wall on/off<br />
<img width="709" alt="Screenshot 2023-07-02 at 21 30 31" src="https://github.com/sabinaaskerova/CTronGame/assets/91430159/d72d35ba-07c2-4f50-865d-96a6b360c2dc">

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
