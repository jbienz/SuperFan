# T'REX #

T'REX is a controller created by DAGU. It can drive two motors up to 30V and a peak of 40A per motor! (Note that 40A is peak amperage for stalled motors and nominal use should not exceed around 18A per motor.) The T'REX includes an integrated Arduino Nano 328, meaning no additional shields are required. The T'REX controller can be purchased [from SparkFun](https://www.sparkfun.com/products/12075) for around $75.

## Bluetooth ##
One of the best features of T'REX is that it can easily be controlled by Bluetooth, which makes it great for PC-based simulations. Several Bluetooth modules work with the board but the sample code provided by DAGU (and preloaded on the board) only works with the DAGU Bluetooth module. Though the DAGU Bluetooth module is a decent one, it's harder to come buy and isn't as flexible as other modules available on the market. Therefore the sketch hosted here has been modified to work with the  [BlueSMiRF Gold](https://www.sparkfun.com/products/12582) module from SparkFun.

## Initialization ##

When the T'REX board is powered on it will attempt to initialize the Bluetooth module. If successful, it will set the advertised Bluetooth name to `SuperFan-####` where `####` is the last four digits of the Bluetooth MAC address. This ensures that more than one SuperFan module can be present and paired if desired.

When initialization has completed the board will 'beep' the motor twice. This indicates that the board is ready to be paired or to receive commands over Bluetooth.
