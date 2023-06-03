/*
Code for functions to simplify using the external flash on the Nicla Sense
*/

#include "niclaflash.h"

/*
For reference about external flash on Nicla Sense:
Model Number: MX25R1635FZUIH0 
Type: NOR
Size: 2 MB
Read Size: 1 B
Program Size: 1 B
Erase Size: 4 kB

A FAT32 allocation for 2 MB will require 4 kB at the beginning of the memory for 
the file table
*/

NiclaFlash::NiclaFlash():writeaddress{FLASH_START_ADDRESS},readaddress{FLASH_START_ADDRESS},
  currentsector{0},extflash{},blockdevice{&extflash, 1}
{
//the default values in the constructor for extflash work
//Uses the Serial Flash Discoverable Parameters feature
  extflash.init();
  // Allocate a partition
  mbed::MBRBlockDevice::partition(&extflash, 1, PARTITION_TYPE, 1024*4, DEVICE_SIZE);
  blockdevice.init(); //initialize the device
}

NiclaFlash::~NiclaFlash()
{
  extflash.deinit();  
  blockdevice.deinit();
}

u_int32_t NiclaFlash::get_writeaddress(void)
{
  return writeaddress;
}
u_int32_t NiclaFlash::set_writeaddress(const u_int32_t newaddress)
{
  u_int32_t oldaddress=writeaddress;
  writeaddress=newaddress;
  return oldaddress;
}
u_int32_t NiclaFlash::get_readaddress(void)
{
  return readaddress;
}
u_int32_t NiclaFlash::set_readaddress(const u_int32_t newaddress)
{
  u_int32_t oldaddress=readaddress;
  readaddress=newaddress;
  return oldaddress;
}
u_int32_t NiclaFlash::get_currentsector(void)
{
  return currentsector;
}





mbed::MBRBlockDevice NiclaFlash::get_blockdevice(void){
  return blockdevice;
}



/*
This function attempts to determine if the message size will push into a new sector
It erases the sector if so.
*/
int NiclaFlash::preparesectorforwrite(const u_int32_t messagesize){

  if(floor(float(writeaddress+messagesize)/float(ERASE_MIN_SIZE))>=currentsector){
    currentsector=u_int32_t(floor(float(writeaddress+messagesize)/float(ERASE_MIN_SIZE)));
    return blockdevice.erase(currentsector*ERASE_MIN_SIZE, 1*ERASE_MIN_SIZE);
  }
  return 0;
}




/*Limited to max 4kB (4k char) in 1 write
This is due to the prepare sector function currently only
being able to handle one sector ahead
*/
int NiclaFlash::writestring(const String stringtowrite)
{
  //This function is a public wrapper to feed the writestring function
  //the current write address from the flash object
  return writestring(stringtowrite, writeaddress);
}
/*
This is the private function that performs the write string operation
*/
int NiclaFlash::writestring(const String stringtowrite, const u_int32_t address)
{
  const auto messagesize = stringtowrite.length() + 1; //One extra byte for NULL termination of c string

  preparesectorforwrite(messagesize);

  blockdevice.program(stringtowrite.c_str(), address, messagesize);
  writeaddress+=messagesize;//increase the write address pointer
  return 0;
}

int NiclaFlash::readchunktoserial(const u_int32_t sizetoread)
{
  //This function is a public wrapper to feed the readchunktoserial function
  //the current read address from the flash object
  return readchunktoserial(sizetoread, readaddress);
}

int NiclaFlash::readchunktoserial(const u_int32_t sizetoread, const u_int32_t address)
{
  char buffer[sizetoread] {}; 
  blockdevice.read(buffer, address, sizetoread);
  Serial.print(buffer);
  readaddress+=sizetoread;//increase the read address pointer
  return 0;
}

/*
This read function will read in from the flash
until a \0 character is hit and then print the buffer to the serial port.
The \0 will be included in the output
*/
int NiclaFlash::readstringtoserial(void)
{
  char buffer[MAX_FLASH_READ_BUFFER_SIZE+1] {};
  char holder[1] {};
  uint16_t hitmaxflag=1;//set to 0 if max is not hit
  //the final result will need to have a \0 appended
  //to have it be a string if the max read is exceed.
  // As such, need the buffer to be one larger
  //in case the max read is reached
  for(u_int32_t i=0; i<MAX_FLASH_READ_BUFFER_SIZE; i++)
  {
    blockdevice.read(holder, readaddress, 1);
    buffer[i]=holder[0];
    readaddress+=1;//increase the read address pointer
    if(buffer[i]=='\0')
    {
      hitmaxflag=0;
      break;
    }
  }

  if(hitmaxflag==0)
  {
    Serial.print(buffer);
  }
  else
  {
    buffer[MAX_FLASH_READ_BUFFER_SIZE]='\0';
    Serial.print(buffer);
  }
  return 0;
}


/*
This read function will read in from the flash
until a \n or \r character is hit and then print the buffer to the serial port
The output will include the \n or \r
*/
int NiclaFlash::readlinetoserial(void)
{
  char buffer[MAX_FLASH_READ_BUFFER_SIZE+1] {};
  char holder[1] {};
  uint16_t hitmaxflag=1;//set to 0 if max is not hit
  //the final result will need to have a \0 appended
  //to have it be a string. As such, need the buffer to be one larger
  //in case the max read is hit
  for(u_int32_t i=0; i<MAX_FLASH_READ_BUFFER_SIZE; i++)
  {
    blockdevice.read(holder, readaddress, 1);
    buffer[i]=holder[0];    
    readaddress+=1;//increase the read address pointer

    if((buffer[i]=='\n')|| (buffer[i]=='\r'))
    {
      hitmaxflag=0;
      buffer[i+1]='\0';
      break;
    } else if(buffer[i]=='\0')//handle if multiple strings form one line
    {
      buffer[i]=' ';
      //i-=1;
    }
  }

  if(hitmaxflag==0)
  {
    //Serial.print("print buffer\n");
    Serial.print(buffer);
  }
  else
  {
    buffer[MAX_FLASH_READ_BUFFER_SIZE]='\0';
    Serial.print(buffer);
  }
  return 0;
}










int NiclaFlash::flashtest(void){


  Serial.println("Nicla Sense SPI Flash Test");
  Serial.println("------------------------"); 

  randomSeed(analogRead(A0)); // seed generator for later random value
  
  const auto eraseblocksize = blockdevice.get_erase_size();
  const auto programblocksize = blockdevice.get_program_size();
  Serial.println("Flash size: " + String((unsigned int) extflash.size() / 1024) + " KB");
  Serial.println("Block device size: " + String((unsigned int) blockdevice.size() / 1024) + " KB");  
  Serial.println("Readable block size: " + String((unsigned int) blockdevice.get_read_size())  + " bytes");
  Serial.println("Programmable block size: " + String((unsigned int) programblocksize) + " bytes");
  Serial.println("Erasable block size: " + String((unsigned int) eraseblocksize / 1024) + " KB");

 //Random number to demonstrate that the value read back and written will be unique and
 //not just the same value read over and over with the new one failing to program
  String newmessage = "Random number: " + String(random(1024));
  Serial.println("Test Message=[" + newmessage+"]");  
  // Calculate the amount of bytes needed to store the message
  // This has to be a multiple of the program block size
  const auto messagesize = newmessage.length() + 1; // C String takes 1 byte for NULL termination
  const unsigned int requirederaseblocks = ceil(messagesize / (float)  eraseblocksize);
  const unsigned int requiredblocks = ceil(messagesize / (float)  programblocksize);
  const auto datasize = requiredblocks * programblocksize;  
  char buffer[datasize] {};  

  Serial.println("Message Size: " + String((unsigned int) messagesize) + " B");
  Serial.println("Data Size: " + String((unsigned int) datasize) + " B");
  Serial.println("Required Erase Blocks: " + String((unsigned int) requirederaseblocks));
  Serial.println("Required Data Blocks: " + String((unsigned int) requiredblocks));


  //Note that, because how how big an area is being formatted and the allocation table being made,
  // there are bytes at the beginning that are erased as part of the partioning. Seems to be roughly 4 KB
  // So we have to read from an address further down than just 0


  // Read back what was stored at previous execution  
  Serial.println("Reading previous message...");
  blockdevice.read(buffer, 1024*8, datasize);
  Serial.println(buffer);

  // Erase a block starting at the offset 0 relative
  // to the block device start address
  blockdevice.erase(1024*8, requirederaseblocks * eraseblocksize);

  // Write an updated message to the first block
  Serial.println("Writing new message...");
  Serial.println(newmessage);  
  blockdevice.program(newmessage.c_str(), 1024*8, datasize);
  

  Serial.println("Done.");


  return 0;
}

/*
Function to read the whole flash to the serial port.
Starts from the intial read address
Currently not implemented
*/
int NiclaFlash::readalltoserial(void)
{
  return 0;
}



