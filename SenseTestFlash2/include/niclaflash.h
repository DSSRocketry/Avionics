/*
Header for functions to simplify using the external flash on the Nicla Sense
Currently only works for a single partition sized to the whole flash
*/
#ifndef NICLA_FLASH_H
#define NICLA_FLASH_H

#include "Arduino.h"
#include "Nicla_System.h"
#include "SPIFBlockDevice.h"
#include "MBRBlockDevice.h"

#include <stdint.h>
#include <string>
#include <cmath>

#define PARTITION_TYPE 0x0B // FAT 32
#define DEVICE_SIZE 1024 * 1024 * 2 // 2 MB
#define FLASH_START_ADDRESS 1024 * 4 // start 4 kB in to avoid the table
#define READ_MIN_SIZE 1 // 1 B
#define PROG_MIN_SIZE 1 // 1 B
#define ERASE_MIN_SIZE 1024 * 4 // 4 kB

#define MAX_FLASH_READ_BUFFER_SIZE 1024 //number of bytes to make the flash read buffers
/*
Class to simplify basic use of the external flash on the 
Nicla Sense
*/
class NiclaFlash
{
//This class should be made into a singleton class
private:
    u_int32_t writeaddress;
    u_int32_t readaddress;
    u_int32_t currentsector;
    SPIFBlockDevice extflash;
    mbed::MBRBlockDevice blockdevice;
    int writestring(const String stringtowrite, const u_int32_t address);
    int readchunktoserial(const u_int32_t sizetoread, const u_int32_t address);
    int preparesectorforwrite(const u_int32_t messagesize);
public:
    NiclaFlash();
    ~NiclaFlash();

    u_int32_t get_writeaddress(void);
    u_int32_t set_writeaddress(const u_int32_t newaddress);
    u_int32_t get_readaddress(void);
    u_int32_t set_readaddress(const u_int32_t newaddress);
    u_int32_t get_currentsector(void);
    mbed::MBRBlockDevice get_blockdevice(void);


    int writestring(const String stringtowrite);
    int readchunktoserial(const u_int32_t sizetoread);
    int readstringtoserial(void);
    int readlinetoserial(void);
    int flashtest(void);
    int readalltoserial(void);

};










#endif