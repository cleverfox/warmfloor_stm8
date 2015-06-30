#include "includes.h"

#define I2C_SPEED       100000
#define EE_ADDRESS      0xA0
#define LM_ADDRESS      0x9F

//#define I2C_MAX_STANDARD_FREQ ((u32)100000)
//#define I2C_MAX_FAST_FREQ     ((u32)400000)

#if 0
typedef enum
{
  /* SR1 register flags */
  I2C_FLAG_TXEMPTY             = (u16)0x0180,  /*!< Transmit Data Register Empty flag */
  I2C_FLAG_RXNOTEMPTY          = (u16)0x0140,  /*!< Read Data Register Not Empty flag */
  I2C_FLAG_STOPDETECTION       = (u16)0x0110,  /*!< Stop detected flag */
  I2C_FLAG_HEADERSENT          = (u16)0x0108,  /*!< 10-bit Header sent flag */
  I2C_FLAG_TRANSFERFINISHED    = (u16)0x0104,  /*!< Data Byte Transfer Finished flag */
  I2C_FLAG_ADDRESSSENTMATCHED  = (u16)0x0102,  /*!< Address Sent/Matched (master/slave) flag */
  I2C_FLAG_STARTDETECTION      = (u16)0x0101,  /*!< Start bit sent flag */

  /* SR2 register flags */
  I2C_FLAG_WAKEUPFROMHALT      = (u16)0x0220,  /*!< Wake Up From Halt Flag */
  I2C_FLAG_OVERRUNUNDERRUN     = (u16)0x0208,  /*!< Overrun/Underrun flag */
  I2C_FLAG_ACKNOWLEDGEFAILURE  = (u16)0x0204,  /*!< Acknowledge Failure Flag */
  I2C_FLAG_ARBITRATIONLOSS     = (u16)0x0202,  /*!< Arbitration Loss Flag */
  I2C_FLAG_BUSERROR            = (u16)0x0201,  /*!< Misplaced Start or Stop condition */

  /* SR3 register flags */
  I2C_FLAG_GENERALCALL         = (u16)0x0310,  /*!< General Call header received Flag */
  I2C_FLAG_TRANSMITTERRECEIVER = (u16)0x0304,  /*!< Transmitter Receiver Flag */
  I2C_FLAG_BUSBUSY             = (u16)0x0302,  /*!< Bus Busy Flag */
  I2C_FLAG_MASTERSLAVE         = (u16)0x0301   /*!< Master Slave Flag */
} I2C_Flag_TypeDef;

typedef enum
{
  I2C_EVENT_MASTER_MODE_SELECT               = (u16)0x0301,  /*!< BUSY, MSL and SB flag */
  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED = (u16)0x0782,  /*!< BUSY, MSL, ADDR, TXE and TRA flags */
  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED    = (u16)0x0302,  /*!< BUSY, MSL and ADDR flags */
  I2C_EVENT_MASTER_MODE_ADDRESS10            = (u16)0x0308,  /*!< BUSY, MSL and ADD10 flags */
  I2C_EVENT_MASTER_BYTE_RECEIVED             = (u16)0x0340,  /*!< BUSY, MSL and RXNE flags */
  I2C_EVENT_MASTER_BYTE_TRANSMITTING         = (u16)0x0780,  /*!< TRA, BUSY, MSL, TXE flags */
  I2C_EVENT_MASTER_BYTE_TRANSMITTED          = (u16)0x0784,  /*!< EV8_2: TRA, BUSY, MSL, TXE and BTF flags */
  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED    = (u16)0x0202,  /*!< BUSY and ADDR flags */
  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED = (u16)0x0682,  /*!< TRA, BUSY, TXE and ADDR flags */
  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED  = (u16)0x1200,  /*!< EV2: GENCALL and BUSY flags */
  I2C_EVENT_SLAVE_BYTE_RECEIVED              = (u16)0x0240,  /*!< BUSY and RXNE flags */
  I2C_EVENT_SLAVE_STOP_DETECTED              = (u16)0x0010,  /*!< STOPF flag */
  I2C_EVENT_SLAVE_BYTE_TRANSMITTED           = (u16)0x0684,  /*!< TRA, BUSY, TXE and BTF flags */
  I2C_EVENT_SLAVE_BYTE_TRANSMITTING          = (u16)0x0680,  /*!< TRA, BUSY and TXE flags */
  I2C_EVENT_SLAVE_ACK_FAILURE                = (u16)0x0004  /*!< AF flag */
} I2C_Event_TypeDef;

typedef enum
{
  I2C_DIRECTION_TX = (u8)0x00,  /*!< Transmission direction */
  I2C_DIRECTION_RX = (u8)0x01   /*!< Reception direction */
} I2C_Direction_TypeDef;

typedef enum
{
  I2C_ACK_NONE = (u8)0x00,  /*!< No acknowledge */
  I2C_ACK_CURR = (u8)0x01,  /*!< Acknowledge on the current byte */
  I2C_ACK_NEXT = (u8)0x02   /*!< Acknowledge on the next byte */
} I2C_Ack_TypeDef;

typedef enum
{
  I2C_ADDMODE_7BIT  = (u8)0x00,  /*!< 7-bit slave address (10-bit address not acknowledged) */
  I2C_ADDMODE_10BIT = (u8)0x80   /*!< 10-bit slave address (7-bit address not acknowledged) */
} I2C_AddMode_TypeDef;

typedef enum
{
  I2C_DUTYCYCLE_2    = (u8)0x00,  /*!< Fast mode Tlow/THigh = 2 */
  I2C_DUTYCYCLE_16_9 = (u8)0x40   /*!< Fast mode Tlow/Thigh = 16/9 */
} I2C_DutyCycle_TypeDef;
#endif

FlagStatus I2C_GetFlagStatus(I2C_Flag_TypeDef I2C_Flag);
void I2C_GenerateSTART(FunctionalState NewState);
ErrorStatus I2C_CheckEvent(I2C_Event_TypeDef I2C_Event);
void I2C_Send7bitAddress(u8 Address, I2C_Direction_TypeDef Direction);
void I2C_SendData(u8 Data);
void I2C_GenerateSTOP(FunctionalState NewState);
void I2C_init(void);
void EE_Write(u8* pData, u16 start, u16 length, u16 address);
u8 EE_Read(u16 address);
u16 LM75_Read();

