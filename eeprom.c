#include "eeprom.h"

FlagStatus I2C_GetFlagStatus(I2C_Flag_TypeDef I2C_Flag) {
  u8 tempreg = 0;
  u8 regindex = 0;
  FlagStatus bitstatus = RESET;

  /* Read flag register index */
  regindex = (u8)((u16)I2C_Flag >> 8);
  /* Check SRx index */
  switch (regindex)
  {
      /* Returns whether the status register to check is SR1 */
    case 0x01:
      tempreg = (u8)I2C->SR1;
      break;

      /* Returns whether the status register to check is SR2 */
    case 0x02:
      tempreg = (u8)I2C->SR2;
      break;

      /* Returns whether the status register to check is SR3 */
    case 0x03:
      tempreg = (u8)I2C->SR3;
      break;

    default:
      break;
  }
  
   /* Check the status of the specified I2C flag */
  if ((tempreg & (u8)I2C_Flag ) != 0)
  {
    /* Flag is set */
    bitstatus = SET;
  }
  else
  {
    /* Flag is reset */
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}

void I2C_GenerateSTART(FunctionalState NewState) {
  if (NewState != DISABLE)
  {
    /* Generate a START condition */
    I2C->CR2 |= I2C_CR2_START;
  }
  else /* NewState == DISABLE */
  {
    /* Disable the START condition generation */
    I2C->CR2 &= (u8)(~I2C_CR2_START);
  }
}

ErrorStatus I2C_CheckEvent(I2C_Event_TypeDef I2C_Event) {
  __IO u16 lastevent = 0x00;
  u8 flag1 = 0x00 ;
  u8 flag2 = 0x00;
  ErrorStatus status = ERROR;

  if (I2C_Event == I2C_EVENT_SLAVE_ACK_FAILURE)
    lastevent = I2C->SR2 & I2C_SR2_AF;
  else
  {
    flag1 = I2C->SR1;
    flag2 = I2C->SR3;
    lastevent = ((u16)((u16)flag2 << (u16)8) | (u16)flag1);
  }
  /* Check whether the last event is equal to I2C_EVENT */
  if (((u16)lastevent & (u16)I2C_Event) == (u16)I2C_Event)
    status = SUCCESS;
  else
    status = ERROR;

  /* Return status */
  return status;
}

void I2C_Send7bitAddress(u8 Address, I2C_Direction_TypeDef Direction) {
  /* Clear bit0 (direction) just in case */
  Address &= (u8)0xFE;

  /* Send the Address + Direction */
  I2C->DR = (u8)(Address | (u8)Direction);
}

void I2C_SendData(u8 Data) {
  I2C->DR = Data;
}

u8 I2C_ReceiveData(void) {
  return ((u8)I2C->DR);
}

void I2C_GenerateSTOP(FunctionalState NewState) {
  if (NewState != DISABLE)
    I2C->CR2 |= I2C_CR2_STOP;   /* Generate a STOP condition */
  else
    I2C->CR2 &= (u8)(~I2C_CR2_STOP);       /* Disable the STOP condition generation */
}

void I2C_AcknowledgeConfig(I2C_Ack_TypeDef Ack) {
  if (Ack == I2C_ACK_NONE)
    I2C->CR2 &= (u8)(~I2C_CR2_ACK);     /* Disable the acknowledgement */
  else
  {
    I2C->CR2 |= I2C_CR2_ACK;    /* Enable the acknowledgement */
    if (Ack == I2C_ACK_CURR)
      I2C->CR2 &= (u8)(~I2C_CR2_POS);
    else
      I2C->CR2 |= I2C_CR2_POS;
  }
}

void I2C_Cmd(FunctionalState NewState) {
  if (NewState != DISABLE)
  {
    /* Enable I2C peripheral */
    I2C->CR1 |= I2C_CR1_PE;
  }
  else /* NewState == DISABLE */
  {
    /* Disable I2C peripheral */
    I2C->CR1 &= (u8)(~I2C_CR1_PE);
  }
}

void I2C_DeInit(void) {
  I2C->CR1 = I2C_CR1_RESET_VALUE;
  I2C->CR2 = I2C_CR2_RESET_VALUE;
  I2C->FREQR = I2C_FREQR_RESET_VALUE;
  I2C->OARL = I2C_OARL_RESET_VALUE;
  I2C->OARH = I2C_OARH_RESET_VALUE;
  I2C->ITR = I2C_ITR_RESET_VALUE;
  I2C->CCRL = I2C_CCRL_RESET_VALUE;
  I2C->CCRH = I2C_CCRH_RESET_VALUE;
  I2C->TRISER = I2C_TRISER_RESET_VALUE;
}

void I2C_Init(u32 OutputClockFrequencyHz, u16 OwnAddress, 
              I2C_DutyCycle_TypeDef I2C_DutyCycle, I2C_Ack_TypeDef Ack, 
              I2C_AddMode_TypeDef AddMode, u8 InputClockFrequencyMHz ) {
  u16 result = 0x0004;
  u16 tmpval = 0;
  u8 tmpccrh = 0;

  /*------------------------- I2C FREQ Configuration ------------------------*/
  /* Clear frequency bits */
  I2C->FREQR &= (u8)(~I2C_FREQR_FREQ);
  /* Write new value */
  I2C->FREQR |= InputClockFrequencyMHz;

  /*--------------------------- I2C CCR Configuration ------------------------*/
  /* Disable I2C to configure TRISER */
  I2C->CR1 &= (u8)(~I2C_CR1_PE);

  /* Clear CCRH & CCRL */
  I2C->CCRH &= (u8)(~(I2C_CCRH_FS | I2C_CCRH_DUTY | I2C_CCRH_CCR));
  I2C->CCRL &= (u8)(~I2C_CCRL_CCR);

  /* Detect Fast or Standard mode depending on the Output clock frequency selected */
  if (OutputClockFrequencyHz > I2C_MAX_STANDARD_FREQ) /* FAST MODE */
  {
    /* Set F/S bit for fast mode */
    tmpccrh = I2C_CCRH_FS;

    if (I2C_DutyCycle == I2C_DUTYCYCLE_2)
    {
      /* Fast mode speed calculate: Tlow/Thigh = 2 */
#ifdef MATH
      result = (u16) ((InputClockFrequencyMHz * 1000000) / (OutputClockFrequencyHz * 3));
#else
      result=53;
#endif
    }
    else /* I2C_DUTYCYCLE_16_9 */
    {
      /* Fast mode speed calculate: Tlow/Thigh = 16/9 */
#ifdef MATH
      result = (u16) ((InputClockFrequencyMHz * 1000000) / (OutputClockFrequencyHz * 25));
#else
      result=6;
#endif
      /* Set DUTY bit */
      tmpccrh |= I2C_CCRH_DUTY;
    }

    /* Verify and correct CCR value if below minimum value */
    if (result < (u16)0x01)
    {
      /* Set the minimum allowed value */
      result = (u16)0x0001;
    }

    /* Set Maximum Rise Time: 300ns max in Fast Mode
    = [300ns/(1/InputClockFrequencyMHz.10e6)]+1
    = [(InputClockFrequencyMHz * 3)/10]+1 */
#ifdef MATH
    tmpval = ((InputClockFrequencyMHz * 3) / 10) + 1;
#else
    tmpval = 0x01;
#endif
    I2C->TRISER = (u8)tmpval;
  }
  else /* STANDARD MODE */
  {

    /* Calculate standard mode speed */
#ifdef MATH
    result = (u16)((InputClockFrequencyMHz * 1000000) / (OutputClockFrequencyHz << (u8)1));
#else
    result = 80;
#endif

    /* Verify and correct CCR value if below minimum value */
    if (result < (u16)0x0004)
    {
      /* Set the minimum allowed value */
      result = (u16)0x0004;
    }

    /* Set Maximum Rise Time: 1000ns max in Standard Mode
    = [1000ns/(1/InputClockFrequencyMHz.10e6)]+1
    = InputClockFrequencyMHz+1 */
    I2C->TRISER = (u8)(InputClockFrequencyMHz + (u8)1);

  }

  /* Write CCR with new calculated value */
  I2C->CCRL = (u8)result;
  I2C->CCRH = (u8)((u8)((u8)(result >> 8) & I2C_CCRH_CCR) | tmpccrh);

  /* Enable I2C */
  I2C->CR1 |= I2C_CR1_PE;

  /* Configure I2C acknowledgement */
  I2C_AcknowledgeConfig(Ack);

  /*--------------------------- I2C OAR Configuration ------------------------*/
  I2C->OARL = (u8)(OwnAddress);
  I2C->OARH = (u8)((u8)(AddMode | I2C_OARH_ADDCONF) |
                   (u8)((OwnAddress & (u16)0x0300) >> (u8)7));
}

void I2C_init(void) {
  I2C_DeInit();
  I2C_Cmd(ENABLE);
  I2C_Init(I2C_SPEED, 0xB0, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
  (void)I2C->SR1;
  (void)I2C->SR3;
}

#ifdef EEPROM
void EE_Write(u8* pData, u16 start, u16 length, u16 address) {
    int i;
    u8 tmp;
    u16 v_ind = start;

    while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));

    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress((u8)EE_ADDRESS, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData((u8)((address & 0xFF00) >> 8));
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData((u8)(address & 0x00FF));
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    while( v_ind < start+length )
    {
        tmp = pData[v_ind];
        I2C_SendData(tmp);
        while (I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET);
        v_ind++;
    }

    I2C_GenerateSTOP(ENABLE);
    (void)I2C->SR1;
    (void)I2C->SR3;
    for(i = 0; i < 0xFFFF; ++i);
}

u8 EE_Read(u16 address) {
  u8 sym;
  
  while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
  
  I2C_GenerateSTART(ENABLE);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  
  I2C_Send7bitAddress((u8)EE_ADDRESS, I2C_DIRECTION_TX);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  I2C_SendData((u8)((address & 0xFF00) >> 8));
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_SendData((u8)(address & 0x00FF));
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTART(ENABLE);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  
  I2C_Send7bitAddress((u8)EE_ADDRESS, I2C_DIRECTION_RX);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
        
  while (I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET);
  sym = I2C_ReceiveData();
  
  I2C_AcknowledgeConfig(I2C_ACK_NONE);
  
  I2C_GenerateSTOP(ENABLE);
  (void)I2C->SR1;
  (void)I2C->SR3;
  return sym;
}
#endif

u16 LM75_Read() {
  u8 sym1;
  u8 sym2;
  u16 trys;
  
  while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
  
  I2C_GenerateSTART(ENABLE);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  
  I2C_Send7bitAddress((u8)LM_ADDRESS, I2C_DIRECTION_TX);
  trys=0xff;
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
      trys--;
      if(!trys){
          I2C_GenerateSTOP(ENABLE);
          return 0xffff;
      }
  }
  
  I2C_SendData(0);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTART(ENABLE);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  
  I2C_Send7bitAddress((u8)LM_ADDRESS, I2C_DIRECTION_RX);
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
        
  while (I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET);
  sym1 = I2C_ReceiveData();
  while (I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET);
  sym2 = I2C_ReceiveData();
  
  I2C_AcknowledgeConfig(I2C_ACK_NONE);
  
  I2C_GenerateSTOP(ENABLE);
  (void)I2C->SR1;
  (void)I2C->SR3;
  //return (sym1<<3)|sym2>>5;
  return ((sym1<<8)|sym2);
}

