# Sketches #

This section includes the Arduino sketches that control actual fan hardware. Currently there is only one driver and it's built on the T'Rex controller board.

## Protocol ##
The recommended protocol for SuperFan controllers starts with a connection at 19200/8-N-1 (19200 kbps, 8 data bits, no parity bit, 1 stop bit). The protocol is a simple 6-byte protocol in the following format:


    [Start] [Command] [Param1] [Param2] [Param3] [Param4]

 
- `[Start]` - The beginning of a command packet. (0x0F)
- `[Command]` - The command to perform.  
- `[Param1]` - The first parameter for the command.
- `[Param2]` - The second parameter for the command. 
- `[Param3]` - The first parameter for the command.
- `[Param4]` - The second parameter for the command. 


###Commands###

####Startup####

Informs the controller that a session is starting. Currently the controller doesn't do anything in response to this command but that may change. Therefore simulators should send this command when starting a game.
 
- Command: '1' (ASCII 0x31)
- Param1: Ignored
- Param2: Ignored
- Param3: Ignored
- Param4: Ignored


####Speed####

Sets the two motors to individual absolute speeds.
 
- Command: 'S' (ASCII 0x53)
- Param1: Motor 1 speed (0-255)
- Param2: Motor 2 speed (0-255)
- Param3: Motor 3 speed (0-255)
- Param4: Motor 4 speed (0-255)


####Logging (Optional)####

Sets the log level of the controller (for diagnostic purposes)
 
- Command: 'L' (ASCII 0x4C)
- Param1: Log level (0 = None, 1 = Error, 2 = Warn, 3 = Info, 4 = Debug)
- Param2: Ignored
- Param3: Ignored
- Param4: Ignored


####Shutdown####

Informs the controller that a session is ending. Currently the controller responds to this command by setting both motors to speed 0. Simulators should send this command when exiting from the game.
 
- Command: '0' (ASCII 0x30)
- Param1: Ignored
- Param2: Ignored
- Param3: Ignored
- Param4: Ignored


###Example Packets###

**Signal a new session and prepare to receive speed data**

    // [Start] | [1] Startup | [00] | [00] | [00] | [00]
    0x0F 0x31 0x00 0x00 0x00 0x00     	// Realterm
    <15><49><0><0><0><0>          		// SimTools

 
**Set motor 1 speed to 12%, motor 2 speed to 50%, motor 3 speed to 80% and motor 4 speed to 25%** 

    // [Start] | ['S'] | [21]  | [127] | [204] | [64]  // (21 / 255 = 12%) (127 / 255 = 50%) (204 / 255 = 80%) (64 / 255 = 25%) 
    0x0F 0x53 0x15 0x7F 0xCC 0x40     	// Realterm
    <15><83><21><127><204><64>       	// SimTools

**Enable logging (DEBUG level)** 

    [Start] | ['L'] | [4]  | [0] | [0] | [0] 
    0x0F 0x4C 0x04 0x00 0x00 0x00     	// Realterm
    <15><76><4><0><0><0>       			// SimTools
 
**End the session and stop all motors** 

    // [Start] | ['0'] Shutdown | [00] | [00] | [00] | [00]
    0x0F 0x30 0x00 0x00 0x00 0x00     	// Realterm
    <15><48><0><0><0><0>          		// SimTools