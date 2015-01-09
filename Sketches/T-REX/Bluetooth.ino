#define CMD_START 0x0F                                         // Indicates the start of a command
#define CMD_LEN 6                                              // Total number of bytes in a command packet
#define RESP_LEN 3                                             // Total number of bytes in a response packet
#define LOG_NONE 0                                             // Log messages are disabled
#define LOG_ERROR 1                                            // Log level for error messages
#define LOG_WARN 2                                             // Log level for warning messages
#define LOG_INFO 3                                             // Log level for info messages
#define LOG_DEBUG 4                                            // Log level for debug messages

// Log level
static int loglevel = LOG_NONE;

//============================================================= Bluetooth Control ===================================================
void Bluetooth()
{
  // Number of bytes read from bluetooth module
  static int bytesRead = 0;

  // Placeholder for the currently read byte
  static byte currentByte = 0;

  // The components of a command packet
  static char op = 0;
  static char param1 = 0;
  static char param2 = 0;
  static char param3 = 0;
  static char param4 = 0;

  // While there are bytes to read, read them
  while (Serial.available() > 0)
  {
    // Read current byte
    currentByte = Serial.read();
    
    // If the current byte is a start of command, reset byte count and continue the loop
    if (currentByte == CMD_START)
    {
      if (loglevel >= LOG_DEBUG)
      {
        Serial.println("Start packet found.");
      }
      bytesRead = 0;
      continue; 
    }
    
    // Not start of command so increase count
    bytesRead++;

    if (loglevel >= LOG_DEBUG)
    {
      Serial.print("Bytes read: ");
      Serial.println(bytesRead, DEC);
    }
    
    // If we have read more bytes than are in a packet, ignore all bytes until a start byte is found
    if (bytesRead >= CMD_LEN)
    {
      if (loglevel >= LOG_ERROR)
      {
        Serial.print("Unexpected bytes after packet. Current byte count: ");
        Serial.println(bytesRead);
      }
      continue; 
    }
    
    // Where we store this byte depends on how many bytes have already been read
    switch (bytesRead)
    {
      case 1:
        op = currentByte;
        continue;
      case 2:
        param1 = currentByte;
        continue;
      case 3:
        param2 = currentByte;
        continue;
      case 4:
        param3 = currentByte;
        continue;
      case 5:
        param4 = currentByte;
        break;
    }

    // Enough bytes. What operation?
    switch (op)
    {
      case '1': // Initialize
        lmspeed = rmspeed = 0;
        if (loglevel >= LOG_INFO)
        {
          Serial.println("Initialized.");
        }
        break;
        
      case '0': // Shutdown
        lmspeed = rmspeed = 0;
        if (loglevel >= LOG_INFO)
        {
          Serial.println("Shut down.");
        }
        break;
        
      case 'L': // Logging
        loglevel = param1;
        if (loglevel >= LOG_ERROR)
        {
          Serial.print("Logging enabled level ");
          Serial.println(loglevel, DEC);
        }
        break;

      case 'S': // Speed
        if (loglevel >= LOG_INFO)
        {
          Serial.print("Set speed: ");
          Serial.print((byte)param1, DEC);
          Serial.print(" ");
          Serial.print((byte)param2, DEC);
          Serial.print(" ");
          Serial.print((byte)param3, DEC);
          Serial.print(" ");
          Serial.println((byte)param4, DEC);
        }

        lmspeed = (byte)param1; // This is for full resolution forward only  (all 255 bits are forward). For half resolution forward and reverse use int(param)-127)*2-1;
        rmspeed = (byte)param2; // This is for full resolution forward only  (all 255 bits are forward). For half resolution forward and reverse use int(param)-127)*2-1;
        // param3 and param4 are not used by this controller
        // Serial.println(lmspeed, DEC);
        break;
      
      default:
        if (loglevel >= LOG_ERROR)
        {
          Serial.print("Unknown operation code ");
          Serial.println(op, DEC);
        }
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
      
      /*
      if (loglevel >= LOG_INFO)
      {
        Serial.println("Bluetooth Ready");
      }
      */
      
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
 * 0x0F 0x30 0x00 0x00 0x00 0x00 - [Start] | ['0'] Initialize | [00] | [00] | [00] | [00]  // (Ignored)
 *
 * // Enable logging (DEBUG level)
 * 0x0F 0x4C 0x04 0x00 0x00 0x00 - [Start] | ['L'] | [4]  | [0] | [0] | [0] // 4 = DEBUG
 *
 * // Set motor 1 speed to 12%, motor 2 speed to 50%, motor 3 speed to 80% and motor 4 speed to 25%
 * 0x0F 0x53 0x15 0x7F 0xCC 0x40 - [Start] | ['S'] | [21]  | [127] | [204] | [64]  // (21 / 255 = 12%) (127 / 255 = 50%) (204 / 255 = 80%) (64 / 255 = 25%) 

 * // Shutdown the interface and stop all motors
 * 0x0F 0x30 0x00 0x00 0x00 0x00 - [Start] | ['0'] Shutdown | [00] | [00] | [00] | [00]  // (Ignored)
 *
 ****************************************************************************/

