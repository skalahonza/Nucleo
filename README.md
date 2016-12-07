# Nucleo
School project for embedded platform STM Nucleo 401RE. Communication via USB with a desktop app. Nucleo responses as mentioned in a communication protocol (readme). Nucleo is equipped with user LED, user button and extensional board with joystick and display.
# Communication protocol
<table>
<tr><th>Commands</th><th>Actions</th></tr>
<tr><td>"LED\r\n"</td><td>Blink Led</td></tr>
<tr><td>"BTN\r\n"</td><td>Print button status</td></tr>
<tr><td>3</td><td>Joystick - not yet implemented</td></tr>
<tr><td>4</td><td>Display - not yet implemented</td></tr>
<tr><td>"*IDN?"</td><td>Print board ID - in this case - Nucelo 401 RE</td></tr>
</table>
