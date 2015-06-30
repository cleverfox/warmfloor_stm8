#include "includes.h"
#include "modbus_slave.h"

#define BUFFER_SIZE 129

// frame[] is used to recieve and transmit packages. 
// The maximum serial ring buffer size is 128
unsigned char frame[BUFFER_SIZE+1];
unsigned int framePtr=0;
unsigned int holdingRegsSize; // size of the register array
unsigned int* uregs; // user array address
unsigned char broadcastFlag;
unsigned char slaveID;
unsigned char function;
unsigned int errorCount;
//unsigned int T1_5; // inter character time out
//unsigned int T3_5; // frame delay
void (*txmode)(char);
void (*txbyte)(char);
void (*writecb)(uint16_t,uint16_t);

void exceptionResponse(unsigned char exception);
unsigned int calculateCRC(unsigned char bufferSize); 
void sendPacket(unsigned char bufferSize);

void modbus_configure(
        unsigned char _slaveID, 
        unsigned int _holdingRegsSize,
        unsigned int* _regs,
        void (*_txmode)(char),
        void (*_txbyte)(char),
        void (*_writecb)(uint16_t,uint16_t)
        ) {
    slaveID = _slaveID;
    holdingRegsSize = _holdingRegsSize; 
    uregs = _regs;
    errorCount = 0; // initialize errorCount
    txmode=_txmode;
    txbyte=_txbyte;
    writecb=_writecb;


    // Modbus states that a baud rate higher than 19200 must use a fixed 750 us 
    // for inter character time out and 1.75 ms for a frame delay for baud rates
    // below 19200 the timing is more critical and has to be calculated.
    // E.g. 9600 baud in a 10 bit packet is 960 characters per second
    // In milliseconds this will be 960characters per 1000ms. So for 1 character
    // 1000ms/960characters is 1.04167ms per character and finally modbus states
    // an inter-character must be 1.5T or 1.5 times longer than a character. Thus
    // 1.5T = 1.04167ms * 1.5 = 1.5625ms. A frame delay is 3.5T.
    /*
    if (baud > 19200)
    {
        T1_5 = 750; 
        T3_5 = 1750; 
    }
    else 
    {
        T1_5 = 15000000/baud; // 1T * 1.5 = T1.5
        T3_5 = 35000000/baud; // 1T * 3.5 = T3.5
    }
    */
}   

void modbus_read(char c) {
   frame[framePtr]=c; 
   if(framePtr<BUFFER_SIZE){
       framePtr++;
   }else{
       errorCount=1;
   }
}

static x=0;
unsigned int modbus_update() {
    if(errorCount){
        framePtr=0;
        errorCount=0;
        return errorCount;
    }
    // The minimum request packet is 8 bytes for function 3 & 16
    if (framePtr > 7) {
        unsigned char id = frame[0];

        broadcastFlag = 0;

        if (id == 0)
            broadcastFlag = 1;

        if (id == slaveID || broadcastFlag) // if the recieved ID matches the slaveID or broadcasting id (0), continue
        {
            uint16_t crc = ((frame[framePtr - 2] << 8) | frame[framePtr - 1]); // combine the crc Low & High bytes
            uint16_t crc1 = calculateCRC(framePtr - 2);
            if (crc1 == crc) // if the calculated crc matches the recieved crc continue
            {
                unsigned int startingAddress;
                unsigned int no_of_registers;
                unsigned int maxData;
                unsigned char index;
                unsigned char address;
                unsigned int crc16;


                function = frame[1];
                startingAddress = ((frame[2] << 8) | frame[3]); // combine the starting address bytes
                no_of_registers = ((frame[4] << 8) | frame[5]); // combine the number of register bytes	
                maxData = startingAddress + no_of_registers;

                // broadcasting is not supported for function 3 
                if (!broadcastFlag && (function==3)) {
                    if (startingAddress < holdingRegsSize){ // check exception 2 ILLEGAL DATA ADDRESS
                        if (maxData <= holdingRegsSize){ // check exception 3 ILLEGAL DATA VALUE
                            unsigned char responseFrameSize;
                            unsigned int temp;
                            unsigned char noOfBytes = no_of_registers * 2; 
                            // ID, function, noOfBytes, (dataLo + dataHi)*number of registers,
                            //  crcLo, crcHi
                            responseFrameSize = 5 + noOfBytes; 
                            frame[0] = slaveID;
                            frame[1] = function;
                            frame[2] = noOfBytes;
                            address = 3; // PDU starts at the 4th byte

                            for (index = startingAddress; index < maxData; index++) {
                                temp = uregs[index];
                                frame[address] = temp >> 8; // split the register into 2 bytes
                                address++;
                                frame[address] = temp & 0xFF;
                                address++;
                            }	

                            crc16 = calculateCRC(responseFrameSize - 2);
                            frame[responseFrameSize - 2] = crc16 >> 8; // split crc into 2 bytes
                            frame[responseFrameSize - 1] = crc16 & 0xFF;
                            sendPacket(responseFrameSize);
                        } else	
                            exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                    } else
                        exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                } else if (function == 16) {
                    // Check if the recieved number of bytes matches the calculated bytes 
                    // minus the request bytes.
                    // id + function + (2 * address bytes) + (2 * no of register bytes) + 
                    // byte count + (2 * CRC bytes) = 9 bytes
                    if (frame[6] == (framePtr - 9)) {
                        if (startingAddress < holdingRegsSize) // check exception 2 ILLEGAL DATA ADDRESS
                        {
                            if (maxData <= holdingRegsSize) // check exception 3 ILLEGAL DATA VALUE
                            {
                                address = 7; // start at the 8th byte in the frame

                                for (index = startingAddress; index < maxData; index++) {
                                    uint16_t val=((frame[address] << 8) | frame[address + 1]);
                                    if(writecb)
                                        writecb(index,val);
                                    else 
                                        uregs[index] = val;
                                    address += 2;
                                }	

                                // only the first 6 bytes are used for CRC calculation
                                crc16 = calculateCRC(6); 
                                frame[6] = crc16 >> 8; // split crc into 2 bytes
                                frame[7] = crc16 & 0xFF;

                                // a function 16 response is an echo of the first 6 bytes from 
                                // the request + 2 crc bytes
                                if (!broadcastFlag) // don't respond if it's a broadcast message
                                    sendPacket(8); 
                            } else	exceptionResponse(3); // exception 3 ILLEGAL DATA VALUE
                        } else exceptionResponse(2); // exception 2 ILLEGAL DATA ADDRESS
                    } else errorCount++; // corrupted packet
                } else exceptionResponse(1); // exception 1 ILLEGAL FUNCTION
            } else { // checksum failed
                errorCount++;
                txbyte(crc>>8);
                txbyte(crc&0xff);
                txbyte(crc1>>8);
                txbyte(crc1&0xff);
            }
        } // incorrect id
    } else if (framePtr > 0)
        errorCount++; // corrupted packet
    framePtr=0;
    errorCount=0;
    return errorCount;
}	

void exceptionResponse(unsigned char exception) {
    // each call to exceptionResponse() will increment the errorCount
    errorCount++; 
    if (!broadcastFlag) // don't respond if its a broadcast message
    {
        unsigned int crc16;
        frame[0] = slaveID;
        frame[1] = (function | 0x80); // set MSB bit high, informs the master of an exception
        frame[2] = exception;
        crc16 = calculateCRC(3); // ID, function|0x80, exception code
        frame[3] = crc16 >> 8;
        frame[4] = crc16 & 0xFF;
        // exception response is always 5 bytes 
        // ID, function + 0x80, exception code, 2 bytes crc
        sendPacket(5); 
    }
}

unsigned int calculateCRC(unsigned char bufferSize) {
    unsigned int temp, temp2, flag;
    unsigned char i, j;
    temp = 0xFFFF;
    for (i = 0; i < bufferSize; i++)
    {
        temp = temp ^ frame[i];
        for (j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag)
                temp ^= 0xA001;
        }
    }
    // Reverse byte order. 
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF; 
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp; 
}
/*
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

// Table of CRC values for low-order byte 
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length) {
    uint8_t crc_hi = 0xFF; // high CRC byte initialized 
    uint8_t crc_lo = 0xFF; // low CRC byte initialized 
    unsigned int i; // will index into CRC lookup 

    // pass through message buffer 
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; // calculate the CRC  
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}
unsigned int calculateCRC(unsigned char bufferSize) {
    return crc16(frame,bufferSize);
}
*/



void sendPacket(unsigned char bufferSize) {
    unsigned char i;
    if(txmode) txmode(1);

    for (i = 0; i < bufferSize; i++)
        txbyte(frame[i]);

    // allow a frame delay to indicate end of transmission
    //delayMicroseconds(T3_5); 

//    txbyte(bufferSize);
    if(txmode) txmode(0);
}
