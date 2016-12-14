# Nucleo
School project for embedded platform STM Nucleo 401RE. Communication via USB with a desktop app. Nucleo responses as mentioned in a communication protocol (readme). Nucleo is equipped with user LED, user button and extensional board with joystick and display.
# Communication protocol
<table>
<tr><th>Commands</th><th>Actions</th></tr>
<tr><td>"LED\r\n"</td><td>Blink Led</td></tr>
<tr><td>"BTN\r\n"</td><td>Print button status</td></tr>
<tr><td>3</td><td>Joystick - not yet implemented</td></tr>
<tr><td>4</td><td>Display - not yet implemented</td></tr>
<tr><td>"*IDN?\r\n"</td><td>Print board ID - in this case - Nucelo 401 RE</td></tr>
</table>

# Asynchronous reading of joystick state
Desktop app is able to read joystick state. When the joystick is pressed, Nucelo sends the information about Joystick state to the app, and the information is displayed in a terminal.
