void modbus_configure(
        unsigned char _slaveID, 
        unsigned int _holdingRegsSize,
        unsigned int* _regs,
        void (*_txmode)(char),
        void (*_txbyte)(char),
        void (*_writecb)(uint16_t,uint16_t));
void modbus_read(char c);
unsigned int modbus_update();
void exceptionResponse(unsigned char exception);
unsigned int calculateCRC(unsigned char bufferSize);
void sendPacket(unsigned char bufferSize);
