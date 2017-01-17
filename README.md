# Nucleo
School project for embedded platform STM Nucleo 401RE. Communication via USB with a desktop app. Nucleo responses as mentioned in a communication protocol (readme). Nucleo is equipped with user LED, user button and extensional board with joystick and display.
# Communication protocol
<ul>
<li>Baudrate 9600</li>
<li>you can set the baudrate with "bash baudset.sh"</li>
<li>Terminating charracters for commands \r\n</li>
<li>Example LED ON\r\n</li>
</ul>
<table>
<tr><th>Commands</th><th>Actions</th><th>Syntax</th></tr>
<tr><td>"LED ON"</td><td>Turn on Led</td><td>-</td></tr>
<tr><td>"LED OFF"</td><td>Turn Off led</td><td>-</td></tr>
<tr><td>"BUTTON?"</td><td>Print button status (BUTTON 1, BUTTON 0)</td><td>-</td></tr>
<tr><td>"*IDN?"</td><td>Print board ID - in this case - Nucelo 401 RE</td><td>-</td></tr>
<tr><td>"DRAW:SETTEXTCOLOR"</td><td>Sets the color for the drawing</td><td>DRAW:SETTEXTCOLOR 1</td></tr>
<tr><td>"DRAW:CLEAR"</td><td>Fills screen with given color</td><td>DRAW:CLEAR 9</td></tr>
<tr><td>"DRAW:PIXEL"</td><td>Draws pixel on given cords.</td><td>DRAW:PIXEL 30,40,4</td></tr>
<tr><td>"DRAW:LINE"</td><td>Draws line on x1,y1 x2,y2.</td><td>DRAW:LINE 10,10,30,30</td></tr>
<tr><td>"DRAW:CIRCLE"</td><td>Draws circle on x,y with specified r</td><td>DRAW:CIRCLE 64,80,30</td></tr>
<tr><td>"DRAW:SETFONT"</td><td>Sets font for text</td><td>DRAW:SETFONT 20</td></tr>
<tr><td>"DRAW:TEXT"</td><td>Draws text on given cords</td><td>DRAW:TEXT 64,80,Nucleo RULEZ!,1</td></tr>
</table>

# Asynchronous reading of joystick state
Desktop app is able to read joystick state. When the joystick is pressed, Nucelo sends the information about Joystick state to the app, and the information is displayed in a terminal.
<table>
<tr><th>Joystick state</th><th>Message</th></tr>
<tr><td>"UP"</td><td>EVENT:JOY_UP</td></tr>
<tr><td>"DOWN"</td><td>EVENT:JOY_DOWN</td></tr>
<tr><td>LEFT</td><td>EVENT:JOY_LEFT</td></tr>
<tr><td>RIGHT</td><td>EVENT:JOY_RIGHT</td></tr>
<tr><td>"CENTER"</td><td>EVENT:JOY_SEL</td></tr>
<tr><td>"NONE"</td><td>Print board ID - in this case - Nucelo 401 RE</td></tr>
</table>
