# T'REX #

T'REX is a controller created by DAGU. It can drive two motors up to 30V and a peak of 40A per motor! (Note that 40A is peak amperage for stalled motors and nominal use should not exceed around 18A per motor.) The T'REX includes an integrated Arduino Nano 328, meaning no additional shields are required. The T'REX controller can be purchased [from SparkFun](https://www.sparkfun.com/products/12075) for around $75.

## Bluetooth ##
One of the best features of T'REX is that it can easily be controlled by Bluetooth, which makes it great for PC-based simulations. Several Bluetooth modules work with the board but the sample code provided by DAGU (and preloaded on the board) only works with the DAGU Bluetooth module. Though the DAGU Bluetooth module is a decent one, it's harder to come buy and isn't as flexible as other modules available on the market. Therefore the sketch hosted here has been modified to work with the  [BlueSMiRF Gold](https://www.sparkfun.com/products/12582) module from SparkFun.

## Initialization ##

When the T'REX board is powered on it will attempt to initialize the Bluetooth module. If successful, it will set the advertised Bluetooth name to `SuperFan-####` where `####` is the last four digits of the Bluetooth MAC address. This ensures that more than one SuperFan module can be present and paired if desired.

When initialization has completed the board will 'beep' the motor twice. This indicates that the board is ready to be paired or to receive commands over Bluetooth.   

## Protocol ##
Our customized sketch for the T'REX expects a connection at 19200/8-N-1 (19200 kbps, 8 data bits, no parity bit, 1 stop bit). The protocol is a simple 4-byte protocol in the following format:


    [Start] [Command] [Param1] [Param2]

 
- `[Start]` - The beginning of a command packet. (0x0F)
- `[Command]` - The command to perform.  
- `[Param1]` - The first parameter for the command.
- `[Param2]` - The second parameter for the command. 


###Commands###

####Startup####

Informs the controller that a session is starting. Currently the controller doesn't do anything in response to this command but that may change. Therefore simulators should send this command when starting a game.
 
- Command: '1' (ASCII 0x31)
- Param1: Ignored
- Param2: Ignored


####Speed####

Sets the two motors to individual absolute speeds.
 
- Command: 'S' (ASCII 0x53)
- Param1: Left motor speed (0-255)
- Param2: Right motor speed (0-255)


####Shutdown####

Informs the controller that a session is ending. Currently the controller responds to this command by setting both motors to speed 0. Simulators should send this command when exiting from the game.
 
- Command: '0' (ASCII 0x30)
- Param1: Ignored
- Param2: Ignored


###Example Packets###

**Signal a new session and prepare to receive speed data**

    // [Start] | [1] Startup | [00] | [00]
    0x0F 0x31 0x00 0x00

 
**Set Left speed to 12% and Right speed to 50%** 

    // [Start] | [S] Speed | [21]  | [127]  :: (21 / 255 = 12%) (127 / 255 = 50%)
    0x0F 0x53 0x15 0x7F 

 
**End the session and stop all motors** 

    // [Start] | [0] Shutdown | [00] | [00]
    0x0F 0x30 0x00 0x00 
