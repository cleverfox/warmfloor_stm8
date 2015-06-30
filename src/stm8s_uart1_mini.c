/**
  ********************************************************************************
  * @file    stm8s_uart1.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   This file contains all the functions for the UART1 peripheral.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s_uart1.h"

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

/** @}
  * @addtogroup UART1_Public_Functions
  * @{
  */

/**
  * @brief  Deinitializes the UART peripheral.
  * @param  None
  * @retval None
	*/
void UART1_DeInit(void)
{
    /* Clear the Idle Line Detected bit in the status rerister by a read
       to the UART1_SR register followed by a Read to the UART1_DR register */
    (void)UART1->SR;
    (void)UART1->DR;

    UART1->BRR2 = UART1_BRR2_RESET_VALUE;  /* Set UART1_BRR2 to reset value 0x00 */
    UART1->BRR1 = UART1_BRR1_RESET_VALUE;  /* Set UART1_BRR1 to reset value 0x00 */

    UART1->CR1 = UART1_CR1_RESET_VALUE;  /* Set UART1_CR1 to reset value 0x00 */
    UART1->CR2 = UART1_CR2_RESET_VALUE;  /* Set UART1_CR2 to reset value 0x00 */
    UART1->CR3 = UART1_CR3_RESET_VALUE;  /* Set UART1_CR3 to reset value 0x00 */
    UART1->CR4 = UART1_CR4_RESET_VALUE;  /* Set UART1_CR4 to reset value 0x00 */
    UART1->CR5 = UART1_CR5_RESET_VALUE;  /* Set UART1_CR5 to reset value 0x00 */

    UART1->GTR = UART1_GTR_RESET_VALUE;
    UART1->PSCR = UART1_PSCR_RESET_VALUE;
}

/**
  * @brief  Initializes the UART1 according to the specified parameters.
  * @note   Configure in Push Pull or Open Drain mode the Tx pin by setting the
  *         correct I/O Port register according the product package and line
  *         configuration
  * @param  BaudRate: The baudrate.
  * @param  WordLength : This parameter can be any of the 
  *         @ref UART1_WordLength_TypeDef enumeration.
  * @param  StopBits: This parameter can be any of the 
  *         @ref UART1_StopBits_TypeDef enumeration.
  * @param  Parity: This parameter can be any of the 
  *         @ref UART1_Parity_TypeDef enumeration.
  * @param  SyncMode: This parameter can be any of the 
  *         @ref UART1_SyncMode_TypeDef values.
  * @param  Mode: This parameter can be any of the @ref UART1_Mode_TypeDef values
  * @retval None
  */
void UART1_Init(uint8_t BRR1, uint8_t BRR2, UART1_WordLength_TypeDef WordLength, 
                UART1_StopBits_TypeDef StopBits, UART1_Parity_TypeDef Parity, 
                UART1_SyncMode_TypeDef SyncMode, UART1_Mode_TypeDef Mode)
{
//    uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0;

    /* Check the parameters */
    assert_param(IS_UART1_BAUDRATE_OK(BaudRate));
    assert_param(IS_UART1_WORDLENGTH_OK(WordLength));
    assert_param(IS_UART1_STOPBITS_OK(StopBits));
    assert_param(IS_UART1_PARITY_OK(Parity));
    assert_param(IS_UART1_MODE_OK((uint8_t)Mode));
    assert_param(IS_UART1_SYNCMODE_OK((uint8_t)SyncMode));

    /* Clear the word length bit */
    UART1->CR1 &= (uint8_t)(~UART1_CR1_M);  
    
     /* Set the word length bit according to UART1_WordLength value */
    UART1->CR1 |= (uint8_t)WordLength;

    /* Clear the STOP bits */
    UART1->CR3 &= (uint8_t)(~UART1_CR3_STOP);  
    /* Set the STOP bits number according to UART1_StopBits value  */
    UART1->CR3 |= (uint8_t)StopBits;  

    /* Clear the Parity Control bit */
    UART1->CR1 &= (uint8_t)(~(UART1_CR1_PCEN | UART1_CR1_PS  ));  
    /* Set the Parity Control bit to UART1_Parity value */
    UART1->CR1 |= (uint8_t)Parity;  

    /* Clear the LSB mantissa of UART1DIV  */
    UART1->BRR1 &= (uint8_t)(~UART1_BRR1_DIVM);  
    /* Clear the MSB mantissa of UART1DIV  */
    UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVM);  
    /* Clear the Fraction bits of UART1DIV */
    UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVF);  

    /* Set the UART1 BaudRates in BRR1 and BRR2 registers according to UART1_BaudRate value */
//    BaudRate_Mantissa    = ((uint32_t)CLK_GetClockFreq() / (BaudRate << 4));
//    BaudRate_Mantissa100 = (((uint32_t)CLK_GetClockFreq() * 100) / (BaudRate << 4));
    /* Set the fraction of UART1DIV  */
//    UART1->BRR2 |= (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100)) << 4) / 100) & (uint8_t)0x0F); 
    /* Set the MSB mantissa of UART1DIV  */
//    UART1->BRR2 |= (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0); 
    /* Set the LSB mantissa of UART1DIV  */
//    UART1->BRR1 |= (uint8_t)BaudRate_Mantissa;           
	UART1->BRR2|=BRR2;
	UART1->BRR1|=BRR1;

    /* Disable the Transmitter and Receiver before seting the LBCL, CPOL and CPHA bits */
    UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); 
    /* Clear the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART1->CR3 &= (uint8_t)~(UART1_CR3_CPOL | UART1_CR3_CPHA | UART1_CR3_LBCL); 
    /* Set the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART1->CR3 |= (uint8_t)((uint8_t)SyncMode & (uint8_t)(UART1_CR3_CPOL | 
                                              UART1_CR3_CPHA | UART1_CR3_LBCL));  

    if ((uint8_t)(Mode & UART1_MODE_TX_ENABLE))
    {
        /* Set the Transmitter Enable bit */
        UART1->CR2 |= (uint8_t)UART1_CR2_TEN;  
    }
    else
    {
        /* Clear the Transmitter Disable bit */
        UART1->CR2 &= (uint8_t)(~UART1_CR2_TEN);  
    }
    if ((uint8_t)(Mode & UART1_MODE_RX_ENABLE))
    {
        /* Set the Receiver Enable bit */
        UART1->CR2 |= (uint8_t)UART1_CR2_REN;  
    }
    else
    {
        /* Clear the Receiver Disable bit */
        UART1->CR2 &= (uint8_t)(~UART1_CR2_REN);  
    }
    /* Set the Clock Enable bit, lock Polarity, lock Phase and Last Bit Clock 
           pulse bits according to UART1_Mode value */
    if ((uint8_t)(SyncMode & UART1_SYNCMODE_CLOCK_DISABLE))
    {
        /* Clear the Clock Enable bit */
        UART1->CR3 &= (uint8_t)(~UART1_CR3_CKEN); 
    }
    else
    {
        UART1->CR3 |= (uint8_t)((uint8_t)SyncMode & UART1_CR3_CKEN);
    }
}

/**
  * @brief  Enable the UART1 peripheral.
  * @param  NewState : The new state of the UART Communication.
  *         This parameter can be any of the @ref FunctionalState enumeration.
  * @retval None
  */
void UART1_Cmd(FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        /* UART1 Enable */
        UART1->CR1 &= (uint8_t)(~UART1_CR1_UARTD); 
    }
    else
    {
        /* UART Disable */
        UART1->CR1 |= UART1_CR1_UARTD;  
    }
}

/**
  * @brief  Enables or disables the specified USART interrupts.
  * @param  UART1_IT specifies the USART interrupt sources to be enabled or disabled.
  *         This parameter can be one of the following values:
  *         - UART1_IT_TXE:  Tansmit Data Register empty interrupt
  *         - UART1_IT_TC:   Transmission complete interrupt
  *         - UART1_IT_RXNE: Receive Data register not empty interrupt
  *         - UART1_IT_OR: Overrun error interrupt
  *         - UART1_IT_IDLE: Idle line detection interrupt
  *         - USRT1_IT_ERR:  Error interrupt
  * @param  NewState new state of the specified USART interrupts.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void UART1_ITConfig(UART1_IT_TypeDef UART1_IT, FunctionalState NewState)
{
    uint8_t uartreg = 0, itpos = 0x00;
    
    /* Check the parameters */
    assert_param(IS_UART1_CONFIG_IT_OK(UART1_IT));
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    /* Get the UART1 register index */
    uartreg = (uint8_t)((uint16_t)UART1_IT >> 0x08);
    /* Get the UART1 IT index */
    itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)UART1_IT & (uint8_t)0x0F));

    if (NewState != DISABLE)
    {
        /**< Enable the Interrupt bits according to UART1_IT mask */
        if (uartreg == 0x01)
        {
            UART1->CR1 |= itpos;
        }
        else if (uartreg == 0x02)
        {
            UART1->CR2 |= itpos;
        }
        else
        {
            UART1->CR4 |= itpos;
        }
    }
    else
    {
        /**< Disable the interrupt bits according to UART1_IT mask */
        if (uartreg == 0x01)
        {
            UART1->CR1 &= (uint8_t)(~itpos);
        }
        else if (uartreg == 0x02)
        {
            UART1->CR2 &= (uint8_t)(~itpos);
        }
        else
        {
            UART1->CR4 &= (uint8_t)(~itpos);
        }
    }

}

/**
  * @brief  Returns the most recent received data by the UART1 peripheral.
  * @param  None
  * @retval The received data.
  */
uint8_t UART1_ReceiveData8(void)
{
    return ((uint8_t)UART1->DR);
}

/**
  * @brief  Transmits 8 bit data through the UART1 peripheral.
  * @param  Data: The data to transmit.
  * @retval None
  */
void UART1_SendData8(uint8_t Data)
{
    /* Transmit Data */
    UART1->DR = Data;
}

/**
  * @brief  Transmits break characters.
  * @param  None
  * @retval None
  */
void UART1_SendBreak(void)
{
    UART1->CR2 |= UART1_CR2_SBK;
}

/**
  * @brief  Checks whether the specified UART1 flag is set or not.
  * @param  UART1_FLAG specifies the flag to check.
  *         This parameter can be any of the @ref UART1_Flag_TypeDef enumeration.
  * @retval FlagStatus (SET or RESET)
  */
FlagStatus UART1_GetFlagStatus(UART1_Flag_TypeDef UART1_FLAG)
{
    FlagStatus status = RESET;

    /* Check parameters */
    assert_param(IS_UART1_FLAG_OK(UART1_FLAG));


    /* Check the status of the specified UART1 flag*/
    if (UART1_FLAG == UART1_FLAG_LBDF)
    {
        if ((UART1->CR4 & (uint8_t)UART1_FLAG) != (uint8_t)0x00)
        {
            /* UART1_FLAG is set*/
            status = SET;
        }
        else
        {
            /* UART1_FLAG is reset*/
            status = RESET;
        }
    }
    else if (UART1_FLAG == UART1_FLAG_SBK)
    {
        if ((UART1->CR2 & (uint8_t)UART1_FLAG) != (uint8_t)0x00)
        {
            /* UART1_FLAG is set*/
            status = SET;
        }
        else
        {
            /* UART1_FLAG is reset*/
            status = RESET;
        }
    }
    else
    {
        if ((UART1->SR & (uint8_t)UART1_FLAG) != (uint8_t)0x00)
        {
            /* UART1_FLAG is set*/
            status = SET;
        }
        else
        {
            /* UART1_FLAG is reset*/
            status = RESET;
        }
    }
    /* Return the UART1_FLAG status*/
    return status;
}

/**
  * @brief  Clears the UART1 flags.
  * @param  UART1_FLAG specifies the flag to clear
  *         This parameter can be any combination of the following values:
  *         - UART1_FLAG_LBDF: LIN Break detection flag.
  *         - UART1_FLAG_RXNE: Receive data register not empty flag.
  * @note
  *         - PE (Parity error), FE (Framing error), NE (Noise error), 
  *         OR (OverRun error) and IDLE (Idle line detected) flags are 
  *         cleared by software sequence: a read operation to UART1_SR register
  *         (UART1_GetFlagStatus())followed by a read operation to UART1_DR 
  *         register(UART1_ReceiveData8() or UART1_ReceiveData9()).
  *           
  *         - RXNE flag can be also cleared by a read to the UART1_DR register
  *         (UART1_ReceiveData8()or UART1_ReceiveData9()).
  *           
  *         - TC flag can be also cleared by software sequence: a read operation
  *         to UART1_SR register (UART1_GetFlagStatus()) followed by a write 
  *         operation to UART1_DR register (UART1_SendData8() or UART1_SendData9()).
  *           
  *         - TXE flag is cleared only by a write to the UART1_DR register 
  *         (UART1_SendData8() or UART1_SendData9()).
  *           
  *         - SBK flag is cleared during the stop bit of break.
  * @retval None
  */

void UART1_ClearFlag(UART1_Flag_TypeDef UART1_FLAG)
{
    assert_param(IS_UART1_CLEAR_FLAG_OK(UART1_FLAG));

    /* Clear the Receive Register Not Empty flag */
    if (UART1_FLAG == UART1_FLAG_RXNE)
    {
        UART1->SR = (uint8_t)~(UART1_SR_RXNE);
    }
    /* Clear the LIN Break Detection flag */
    else
    {
        UART1->CR4 &= (uint8_t)~(UART1_CR4_LBDF);
    }
}

/**
  * @brief  Checks whether the specified UART1 interrupt has occurred or not.
  * @param  UART1_IT: Specifies the UART1 interrupt pending bit to check.
  *         This parameter can be one of the following values:
  *         - UART1_IT_LBDF:  LIN Break detection interrupt
  *         - UART1_IT_TXE:  Tansmit Data Register empty interrupt
  *         - UART1_IT_TC:   Transmission complete interrupt
  *         - UART1_IT_RXNE: Receive Data register not empty interrupt
  *         - UART1_IT_IDLE: Idle line detection interrupt
  *         - UART1_IT_OR:  OverRun Error interrupt
  *         - UART1_IT_PE:   Parity Error interrupt
  * @retval The new state of UART1_IT (SET or RESET).
  */
ITStatus UART1_GetITStatus(UART1_IT_TypeDef UART1_IT)
{
    ITStatus pendingbitstatus = RESET;
    uint8_t itpos = 0;
    uint8_t itmask1 = 0;
    uint8_t itmask2 = 0;
    uint8_t enablestatus = 0;

    /* Check parameters */
    assert_param(IS_UART1_GET_IT_OK(UART1_IT));

    /* Get the UART1 IT index */
    itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)UART1_IT & (uint8_t)0x0F));
    /* Get the UART1 IT index */
    itmask1 = (uint8_t)((uint8_t)UART1_IT >> (uint8_t)4);
    /* Set the IT mask*/
    itmask2 = (uint8_t)((uint8_t)1 << itmask1);


    /* Check the status of the specified UART1 pending bit*/
    if (UART1_IT == UART1_IT_PE)
    {
        /* Get the UART1_IT enable bit status*/
        enablestatus = (uint8_t)((uint8_t)UART1->CR1 & itmask2);
        /* Check the status of the specified UART1 interrupt*/

        if (((UART1->SR & itpos) != (uint8_t)0x00) && enablestatus)
        {
            /* Interrupt occurred*/
            pendingbitstatus = SET;
        }
        else
        {
            /* Interrupt not occurred*/
            pendingbitstatus = RESET;
        }
    }

    else if (UART1_IT == UART1_IT_LBDF)
    {
        /* Get the UART1_IT enable bit status*/
        enablestatus = (uint8_t)((uint8_t)UART1->CR4 & itmask2);
        /* Check the status of the specified UART1 interrupt*/
        if (((UART1->CR4 & itpos) != (uint8_t)0x00) && enablestatus)
        {
            /* Interrupt occurred*/
            pendingbitstatus = SET;
        }
        else
        {
            /* Interrupt not occurred*/
            pendingbitstatus = RESET;
        }
    }
    else
    {
        /* Get the UART1_IT enable bit status*/
        enablestatus = (uint8_t)((uint8_t)UART1->CR2 & itmask2);
        /* Check the status of the specified UART1 interrupt*/
        if (((UART1->SR & itpos) != (uint8_t)0x00) && enablestatus)
        {
            /* Interrupt occurred*/
            pendingbitstatus = SET;
        }
        else
        {
            /* Interrupt not occurred*/
            pendingbitstatus = RESET;
        }
    }

    /* Return the UART1_IT status*/
    return  pendingbitstatus;
}

/**
  * @brief  Clears the UART1 pending flags.
  * @param  UART1_IT specifies the pending bit to clear
  *         This parameter can be one of the following values:
  *         - UART1_IT_LBDF:  LIN Break detection interrupt
  *         - UART1_IT_RXNE: Receive Data register not empty interrupt.
  * @note
  *         - PE (Parity error), FE (Framing error), NE (Noise error), 
  *           OR (OverRun error) and IDLE (Idle line detected) pending bits are 
  *           cleared by software sequence: a read operation to UART1_SR register
  *           (UART1_GetITStatus()) followed by a read operation to UART1_DR register
  *           (UART1_ReceiveData8() or UART1_ReceiveData9()).
  *   
  *         - RXNE pending bit can be also cleared by a read to the UART1_DR register
  *           (UART1_ReceiveData8() or UART1_ReceiveData9()).
  * 
  *         - TC (Transmit complete) pending bit can be cleared by software 
  *           sequence: a read operation to UART1_SR register (UART1_GetITStatus())
  *           followed by a write operation to UART1_DR register (UART1_SendData8()
  *           or UART1_SendData9()).
  *             
  *         - TXE pending bit is cleared only by a write to the UART1_DR register
  *           (UART1_SendData8() or UART1_SendData9()).
  * @retval None
  */
void UART1_ClearITPendingBit(UART1_IT_TypeDef UART1_IT)
{
    assert_param(IS_UART1_CLEAR_IT_OK(UART1_IT));

    /* Clear the Receive Register Not Empty pending bit */
    if (UART1_IT == UART1_IT_RXNE)
    {
        UART1->SR = (uint8_t)~(UART1_SR_RXNE);
    }
    /* Clear the LIN Break Detection pending bit */
    else
    {
        UART1->CR4 &= (uint8_t)~(UART1_CR4_LBDF);
    }
}

/**
  * @}
  */
  
/**
  * @}
  */
  
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
