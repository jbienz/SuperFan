#define CMD_START 0x0F                                         // Indicates the start of a command
#define CMD_LEN 4                                              // Total number of bytes in a command packet
#define RESP_LEN 3                                             // Total number of bytes in a response packet

//============================================================= Bluetooth Control ===================================================
void Bluetooth()
{

  // Place to read commands from bluetooth module
  static char command[CMD_LEN];

  // Make sure there are enough bytes to read a command
  if (Serial.available() >= CMD_LEN)
  {
    // Read command and verify integrity
    // If not a valid command, empty buffer and bail
    if ((Serial.readBytes(command, CMD_LEN) != CMD_LEN) || (command[0] != CMD_START))
    {
      EmptyBuffer();
      return;
    }

    // Break command into components
    char op = command[1];
    char param1 = command[2];
    char param2 = command[3];

    // What operation
    switch (op)
    {
      case '1': // Initialize
        break;
        
      case '0': // Shutdown
        lmspeed = rmspeed = 0;
        break;
        
      case 'S': // Speed
        lmspeed = (byte)param1; // This is for full resolution forward only  (all 255 bits are forward). For half resolution forward and reverse use int(param)-127)*2-1;
        rmspeed = (byte)param2; // This is for full resolution forward only  (all 255 bits are forward). For half resolution forward and reverse use int(param)-127)*2-1;
        // Serial.println(lmspeed, DEC);
        break;
      
      default:
        // Unknown command, ignore
        return;
    }
  }

  // Update motors     
  Motors();
}



void BluetoothConfig()
{
  // This code intended for a SparkFun BlueSMiRF Gold module. It will likely need to be modified to work with other brands.
  
  // Wait 500ms to ensure bluetooth module is ready
  delay(500);
  
  // Place to read responses from bluetooth module
  char response[RESP_LEN];

  // Enable serial at 115.2k
  Serial.begin(115200);
  
  // Wait for bluetooth module to initialize
  delay(1500);
  
  // Enter command mode
  Serial.print("$$$");
  
  // Wait for bluetooth module to respond
  delay(1500);
  
  // See if we have a response
  if (Serial.available() >= RESP_LEN)
  {
    // Read response
    Serial.readBytes(response, RESP_LEN);
    
    // "CMD" indicates we have entered command mode successfully
    if (strcmp(response, "CMD" == 0))
    {
      // Clear buffer
      EmptyBuffer();
      
      // Ensure Bluetooth name is set to "SuperFan-XXXX"
      Serial.print("S-,SuperFan\r");
      
      // Wait for bluetooth module to process
      delay(500);
      
      // Reboot module for name to take effect
      Serial.print("R,1\r");
      
      // Wait for bluetooth module to reboot
      delay(1000);
      
      // Clear buffer
      EmptyBuffer();
      
      // Bluetooth module successfully detected and configured - change to bluetooth mode
      mode=2;
    }
  }

  // If we're still in mode zero then Bluetooth was not detected or could not be initialized
  if (mode == 0)                                                  
  {
    // Clear buffer and close serial
    EmptyBuffer();
    Serial.end();
  }
}

void EmptyBuffer()
{
  byte b;
  while(Serial.available())
  {
    b=Serial.read();
  }
}


/*****************************************************************************
 * Example Packets
 *****************************************************************************
 *
 * // Initialize the interface and prepare to receive speed data
 * 0x0F 0x30 0x00 0x00 - [Flag] | ['0'] Initialize | [00] | [00]  // (Ignored)
 *
 * // Set Left speed to 12% and Right speed to 50%
 * 0x0F 0x53 0x15 0x7F - [Flag] | ['S'] | [21]  | [127]  // (21 / 255 = 12%) (127 / 255 = 50%) 
 *
 * // Shutdown the interface and stop all motors
 * 0x0F 0x30 0x00 0x00 - [Flag] | ['0'] Shutdown | [00] | [00]  // (Ignored)
 *
 ****************************************************************************/

