/**
  ******************************************************************************
  * @file    stm8s_clk.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   This file contains all the functions for the CLK peripheral.
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

#include "stm8s_clk.h"
#include "stm8s_tim4.h"
#include "stm8s_uart2.h"
#include "stm8s_itc.h"
#include "stm8s_flash.h"

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_CLEAR_BYTE ((uint8_t)0x00)
#define FLASH_SET_BYTE  ((uint8_t)0xFF)
#define OPERATION_TIMEOUT  ((uint32_t)0xFFFFF)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private Constants ---------------------------------------------------------*/

/**
  * @addtogroup CLK_Private_Constants
  * @{
  */

CONST uint8_t HSIDivFactor[4] = {1, 2, 4, 8}; /*!< Holds the different HSI Divider factors */
CONST uint8_t CLKPrescTable[8] = {1, 2, 4, 8, 10, 16, 20, 40}; /*!< Holds the different CLK prescaler values */



/**
  * @brief  This function returns the frequencies of different on chip clocks.
  * @param  None
  * @retval the master clock frequency
  */
uint32_t CLK_GetClockFreq(void)
{

    uint32_t clockfrequency = 0;
    CLK_Source_TypeDef clocksource = CLK_SOURCE_HSI;
    uint8_t tmp = 0, presc = 0;

    /* Get CLK source. */
    clocksource = (CLK_Source_TypeDef)CLK->CMSR;

    if (clocksource == CLK_SOURCE_HSI)
    {
        tmp = (uint8_t)(CLK->CKDIVR & CLK_CKDIVR_HSIDIV);
        tmp = (uint8_t)(tmp >> 3);
        presc = HSIDivFactor[tmp];
        clockfrequency = HSI_VALUE / presc;
    }
    else if ( clocksource == CLK_SOURCE_LSI)
    {
        clockfrequency = LSI_VALUE;
    }
    else
    {
        clockfrequency = HSE_VALUE;
    }

    return((uint32_t)clockfrequency);

}




/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/**
  * @addtogroup TIM4_Public_Functions
  * @{
  */

/**
  * @brief  Deinitializes the TIM4 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void TIM4_DeInit(void)
{
    TIM4->CR1 = TIM4_CR1_RESET_VALUE;
    TIM4->IER = TIM4_IER_RESET_VALUE;
    TIM4->CNTR = TIM4_CNTR_RESET_VALUE;
    TIM4->PSCR = TIM4_PSCR_RESET_VALUE;
    TIM4->ARR = TIM4_ARR_RESET_VALUE;
    TIM4->SR1 = TIM4_SR1_RESET_VALUE;
}

/**
  * @brief  Initializes the TIM4 Time Base Unit according to the specified parameters.
  * @param    TIM4_Prescaler specifies the Prescaler from TIM4_Prescaler_TypeDef.
  * @param    TIM4_Period specifies the Period value.
  * @retval None
  */
void TIM4_TimeBaseInit(TIM4_Prescaler_TypeDef TIM4_Prescaler, uint8_t TIM4_Period)
{
    /* Check TIM4 prescaler value */
    assert_param(IS_TIM4_PRESCALER_OK(TIM4_Prescaler));
    /* Set the Prescaler value */
    TIM4->PSCR = (uint8_t)(TIM4_Prescaler);
    /* Set the Autoreload value */
    TIM4->ARR = (uint8_t)(TIM4_Period);
}



/**
  * @brief  Enables or disables the TIM4 peripheral.
  * @param   NewState new state of the TIM4 peripheral. This parameter can
  * be ENABLE or DISABLE.
  * @retval None
  */
void TIM4_Cmd(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    /* set or Reset the CEN Bit */
    if (NewState != DISABLE)
    {
        TIM4->CR1 |= TIM4_CR1_CEN;
    }
    else
    {
        TIM4->CR1 &= (uint8_t)(~TIM4_CR1_CEN);
    }
}

/**
  * @brief  Enables or disables the specified TIM4 interrupts.
  * @param   NewState new state of the TIM4 peripheral.
  * This parameter can be: ENABLE or DISABLE.
  * @param   TIM4_IT specifies the TIM4 interrupts sources to be enabled or disabled.
  * This parameter can be any combination of the following values:
  * - TIM4_IT_UPDATE: TIM4 update Interrupt source
  * @param   NewState new state of the TIM4 peripheral.
  * @retval None
  */
void TIM4_ITConfig(TIM4_IT_TypeDef TIM4_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_TIM4_IT_OK(TIM4_IT));
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the Interrupt sources */
        TIM4->IER |= (uint8_t)TIM4_IT;
    }
    else
    {
        /* Disable the Interrupt sources */
        TIM4->IER &= (uint8_t)(~TIM4_IT);
    }
}

/**
  * @brief  Clears the TIM4's interrupt pending bits.
  * @param   TIM4_IT specifies the pending bit to clear.
  * This parameter can be one of the following values:
  *                       - TIM4_IT_UPDATE: TIM4 update Interrupt source
  * @retval None.
  */
void TIM4_ClearITPendingBit(TIM4_IT_TypeDef TIM4_IT)
{
    /* Check the parameters */
    assert_param(IS_TIM4_IT_OK(TIM4_IT));

    /* Clear the IT pending Bit */
    TIM4->SR1 = (uint8_t)(~TIM4_IT);
}



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
  * @addtogroup UART2_Public_Functions
  * @{
  */


/**
  * @brief  Initializes the UART2 according to the specified parameters.
  * @param  BaudRate: The baudrate.
  * @param  WordLength : This parameter can be any of the 
  *         @ref UART2_WordLength_TypeDef enumeration.
  * @param  StopBits: This parameter can be any of the 
  *         @ref UART2_StopBits_TypeDef enumeration.
  * @param  Parity: This parameter can be any of the 
  *         @ref UART2_Parity_TypeDef enumeration.
  * @param  SyncMode: This parameter can be any of the 
  *         @ref UART2_SyncMode_TypeDef values.
  * @param  Mode: This parameter can be any of the @ref UART2_Mode_TypeDef values
  * @retval None
  */
void UART2_Init(uint32_t BaudRate, UART2_WordLength_TypeDef WordLength, UART2_StopBits_TypeDef StopBits, UART2_Parity_TypeDef Parity, UART2_SyncMode_TypeDef SyncMode, UART2_Mode_TypeDef Mode)
{
    uint8_t BRR2_1 = 0, BRR2_2 = 0;
    uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0;

    /* Check the parameters */
    assert_param(IS_UART2_BAUDRATE_OK(BaudRate));
    assert_param(IS_UART2_WORDLENGTH_OK(WordLength));
    assert_param(IS_UART2_STOPBITS_OK(StopBits));
    assert_param(IS_UART2_PARITY_OK(Parity));
    assert_param(IS_UART2_MODE_OK((uint8_t)Mode));
    assert_param(IS_UART2_SYNCMODE_OK((uint8_t)SyncMode));

     /* Clear the word length bit */
    UART2->CR1 &= (uint8_t)(~UART2_CR1_M);
    /* Set the word length bit according to UART2_WordLength value */
    UART2->CR1 |= (uint8_t)WordLength; 

    /* Clear the STOP bits */
    UART2->CR3 &= (uint8_t)(~UART2_CR3_STOP);
    /* Set the STOP bits number according to UART2_StopBits value  */
    UART2->CR3 |= (uint8_t)StopBits; 

    /* Clear the Parity Control bit */
    UART2->CR1 &= (uint8_t)(~(UART2_CR1_PCEN | UART2_CR1_PS  ));
    /* Set the Parity Control bit to UART2_Parity value */
    UART2->CR1 |= (uint8_t)Parity;

    /* Clear the LSB mantissa of UART2DIV  */
    UART2->BRR1 &= (uint8_t)(~UART2_BRR1_DIVM);
    /* Clear the MSB mantissa of UART2DIV  */
    UART2->BRR2 &= (uint8_t)(~UART2_BRR2_DIVM);
    /* Clear the Fraction bits of UART2DIV */
    UART2->BRR2 &= (uint8_t)(~UART2_BRR2_DIVF);

    /* Set the UART2 BaudRates in BRR1 and BRR2 registers according to UART2_BaudRate value */
    BaudRate_Mantissa    = ((uint32_t)CLK_GetClockFreq() / (BaudRate << 4));
    BaudRate_Mantissa100 = (((uint32_t)CLK_GetClockFreq() * 100) / (BaudRate << 4));
    
    /* The fraction and MSB mantissa should be loaded in one step in the BRR2 register*/
    /* Set the fraction of UARTDIV  */
    BRR2_1 = (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100))
                        << 4) / 100) & (uint8_t)0x0F); 
    BRR2_2 = (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0);

    UART2->BRR2 = (uint8_t)(BRR2_1 | BRR2_2);
    /* Set the LSB mantissa of UARTDIV  */
    UART2->BRR1 = (uint8_t)BaudRate_Mantissa;           

    /* Disable the Transmitter and Receiver before seting the LBCL, CPOL and CPHA bits */
    UART2->CR2 &= (uint8_t)~(UART2_CR2_TEN | UART2_CR2_REN);
    /* Clear the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART2->CR3 &= (uint8_t)~(UART2_CR3_CPOL | UART2_CR3_CPHA | UART2_CR3_LBCL);
    /* Set the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART2->CR3 |= (uint8_t)((uint8_t)SyncMode & (uint8_t)(UART2_CR3_CPOL | \
                                              UART2_CR3_CPHA | UART2_CR3_LBCL));

    if ((uint8_t)(Mode & UART2_MODE_TX_ENABLE))
    {
        /* Set the Transmitter Enable bit */
        UART2->CR2 |= (uint8_t)UART2_CR2_TEN;
    }
    else
    {
        /* Clear the Transmitter Disable bit */
        UART2->CR2 &= (uint8_t)(~UART2_CR2_TEN);
    }
    if ((uint8_t)(Mode & UART2_MODE_RX_ENABLE))
    {
        /* Set the Receiver Enable bit */
        UART2->CR2 |= (uint8_t)UART2_CR2_REN;
    }
    else
    {
        /* Clear the Receiver Disable bit */
        UART2->CR2 &= (uint8_t)(~UART2_CR2_REN);
    }
    /* Set the Clock Enable bit, lock Polarity, lock Phase and Last Bit Clock 
           pulse bits according to UART2_Mode value */
    if ((uint8_t)(SyncMode & UART2_SYNCMODE_CLOCK_DISABLE))
    {
        /* Clear the Clock Enable bit */
        UART2->CR3 &= (uint8_t)(~UART2_CR3_CKEN); 
    }
    else
    {
        UART2->CR3 |= (uint8_t)((uint8_t)SyncMode & UART2_CR3_CKEN);
    }
}

/**
  * @brief  Enable the UART2 peripheral.
  * @param  NewState : The new state of the UART Communication.
  *         This parameter can be any of the @ref FunctionalState enumeration.
  * @retval None
  */
/*
void UART2_Cmd(FunctionalState NewState)
{

    if (NewState != DISABLE)
    {
        // UART2 Enable 
        UART2->CR1 &= (uint8_t)(~UART2_CR1_UARTD);
    }
    else
    {
        // UART2 Disable 
        UART2->CR1 |= UART2_CR1_UARTD; 
    }
}
*/

/**
  * @brief  Enables or disables the specified UART2 interrupts.
  * @param  UART2_IT specifies the UART2 interrupt sources to be enabled or disabled.
  *         This parameter can be one of the following values:
  *         - UART2_IT_LBDF:  LIN Break detection interrupt
  *         - UART2_IT_LHDF:  LIN Break detection interrupt
  *         - UART2_IT_TXE:  Tansmit Data Register empty interrupt
  *         - UART2_IT_TC:   Transmission complete interrupt
  *         - UART2_IT_RXNE_OR: Receive Data register not empty/Over run error interrupt
  *         - UART2_IT_IDLE: Idle line detection interrupt
  *         - UART2_IT_PE:   Parity Error interrupt
  * @param  NewState new state of the specified UART2 interrupts.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void UART2_ITConfig(UART2_IT_TypeDef UART2_IT, FunctionalState NewState)
{
    uint8_t uartreg = 0, itpos = 0x00;
    
    /* Check the parameters */
    assert_param(IS_UART2_CONFIG_IT_OK(UART2_IT));
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    /* Get the UART2 register index */
    uartreg = (uint8_t)((uint16_t)UART2_IT >> 0x08);

    /* Get the UART2 IT index */
    itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)UART2_IT & (uint8_t)0x0F));


    if (NewState != DISABLE)
    {
        /* Enable the Interrupt bits according to UART2_IT mask */
        if (uartreg == 0x01)
        {
            UART2->CR1 |= itpos;
        }
        else if (uartreg == 0x02)
        {
            UART2->CR2 |= itpos;
        }
        else if (uartreg == 0x03)
        {
            UART2->CR4 |= itpos;
        }
        else
        {
            UART2->CR6 |= itpos;
        }
    }
    else
    {
        /* Disable the interrupt bits according to UART2_IT mask */
        if (uartreg == 0x01)
        {
            UART2->CR1 &= (uint8_t)(~itpos);
        }
        else if (uartreg == 0x02)
        {
            UART2->CR2 &= (uint8_t)(~itpos);
        }
        else if (uartreg == 0x03)
        {
            UART2->CR4 &= (uint8_t)(~itpos);
        }
        else
        {
            UART2->CR6 &= (uint8_t)(~itpos);
        }
    }
}



/**
  * @brief  Transmits 8 bit data through the UART2 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void UART2_SendData8(uint8_t Data)
{
    /* Transmit Data */
    UART2->DR = Data;
}





/**
  * @brief  Sets the software priority of the specified interrupt source.
  * @note   - The modification of the software priority is only possible when
  *         the interrupts are disabled.
  *         - The normal behavior is to disable the interrupt before calling
  *         this function, and re-enable it after.
  *         - The priority level 0 cannot be set (see product specification
  *         for more details).
  * @param  IrqNum : Specifies the peripheral interrupt source.
  * @param  PriorityValue : Specifies the software priority value to set,
  *         can be a value of @ref  ITC_PriorityLevel_TypeDef .
  * @retval None
*/
void ITC_SetSoftwarePriority(ITC_Irq_TypeDef IrqNum, ITC_PriorityLevel_TypeDef PriorityValue)
{

    uint8_t Mask = 0;
    uint8_t NewPriority = 0;

    /* Check function parameters */
    assert_param(IS_ITC_IRQ_OK((uint8_t)IrqNum));
    assert_param(IS_ITC_PRIORITY_OK(PriorityValue));

    /* Check if interrupts are disabled */
    assert_param(IS_ITC_INTERRUPTS_DISABLED);

    /* Define the mask corresponding to the bits position in the SPR register */
    /* The mask is reversed in order to clear the 2 bits after more easily */
    Mask = (uint8_t)(~(uint8_t)(0x03U << (((uint8_t)IrqNum % 4U) * 2U)));

    /* Define the new priority to write */
    NewPriority = (uint8_t)((uint8_t)(PriorityValue) << (((uint8_t)IrqNum % 4U) * 2U));

    switch (IrqNum)
    {

    case ITC_IRQ_TLI: /* TLI software priority can be written but has no meaning */
    case ITC_IRQ_AWU:
    case ITC_IRQ_CLK:
    case ITC_IRQ_PORTA:
        ITC->ISPR1 &= Mask;
        ITC->ISPR1 |= NewPriority;
        break;

    case ITC_IRQ_PORTB:
    case ITC_IRQ_PORTC:
    case ITC_IRQ_PORTD:
    case ITC_IRQ_PORTE:
        ITC->ISPR2 &= Mask;
        ITC->ISPR2 |= NewPriority;
        break;

#if defined(STM8S208) || defined(STM8AF52Ax)
    case ITC_IRQ_CAN_RX:
    case ITC_IRQ_CAN_TX:
#endif /*STM8S208 or STM8AF52Ax */

#ifdef STM8S903
    case ITC_IRQ_PORTF:
#endif /*STM8S903*/
    case ITC_IRQ_SPI:
    case ITC_IRQ_TIM1_OVF:
        ITC->ISPR3 &= Mask;
        ITC->ISPR3 |= NewPriority;
        break;

    case ITC_IRQ_TIM1_CAPCOM:
#ifdef STM8S903
    case ITC_IRQ_TIM5_OVFTRI:
    case ITC_IRQ_TIM5_CAPCOM:
#else
    case ITC_IRQ_TIM2_OVF:
    case ITC_IRQ_TIM2_CAPCOM:
#endif /*STM8S903*/

    case ITC_IRQ_TIM3_OVF:
        ITC->ISPR4 &= Mask;
        ITC->ISPR4 |= NewPriority;
        break;

    case ITC_IRQ_TIM3_CAPCOM:
    case ITC_IRQ_UART1_TX:
    case ITC_IRQ_UART1_RX:
    case ITC_IRQ_I2C:
        ITC->ISPR5 &= Mask;
        ITC->ISPR5 |= NewPriority;
        break;

#if defined(STM8S105) || defined(STM8S005) || defined(STM8AF626x)
    case ITC_IRQ_UART2_TX:
    case ITC_IRQ_UART2_RX:
#endif /*STM8S105 or STM8AF626x */

#if defined(STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8AF52Ax) || \
    defined(STM8AF62Ax)
    case ITC_IRQ_UART3_TX:
    case ITC_IRQ_UART3_RX:
    case ITC_IRQ_ADC2:
#endif /*STM8S208 or STM8S207 or STM8AF52Ax or STM8AF62Ax */

#if defined(STM8S105) || defined(STM8S005) || defined(STM8S103) || defined(STM8S003) || \
    defined(STM8S903) || defined(STM8AF626x)
    case ITC_IRQ_ADC1:
#endif /*STM8S105, STM8S103 or STM8S905 or STM8AF626x */

#ifdef STM8S903
    case ITC_IRQ_TIM6_OVFTRI:
#else
    case ITC_IRQ_TIM4_OVF:
#endif /*STM8S903*/
        ITC->ISPR6 &= Mask;
        ITC->ISPR6 |= NewPriority;
        break;

    case ITC_IRQ_EEPROM_EEC:
        ITC->ISPR7 &= Mask;
        ITC->ISPR7 |= NewPriority;
        break;

    default:
        break;

    }

}



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private Constants ---------------------------------------------------------*/

/** @addtogroup FLASH_Public_functions
  * @{
  */

/**
  * @brief   Unlocks the program or data EEPROM memory
  * @param  FLASH_MemType : Memory type to unlock
  *         This parameter can be a value of @ref FLASH_MemType_TypeDef
  * @retval None
  */
void FLASH_Unlock(FLASH_MemType_TypeDef FLASH_MemType)
{
    /* Check parameter */
    assert_param(IS_MEMORY_TYPE_OK(FLASH_MemType));

    /* Unlock program memory */
    if (FLASH_MemType == FLASH_MEMTYPE_PROG)
    {
        FLASH->PUKR = FLASH_RASS_KEY1;
        FLASH->PUKR = FLASH_RASS_KEY2;
    }
    /* Unlock data memory */
    else
    {
        FLASH->DUKR = FLASH_RASS_KEY2; /* Warning: keys are reversed on data memory !!! */
        FLASH->DUKR = FLASH_RASS_KEY1;
    }
}





/**
  * @brief   Erases one byte in the program or data EEPROM memory
  * @note   PointerAttr define is declared in the stm8s.h file to select if 
  *         the pointer will be declared as near (2 bytes) or far (3 bytes).
  * @param  Address : Address of the byte to erase
  * @retval None
  */
void FLASH_EraseByte(uint32_t Address)
{
    /* Check parameter */
    assert_param(IS_FLASH_ADDRESS_OK(Address));
    
    /* Erase byte */
   *(PointerAttr uint8_t*) (uint16_t)Address = FLASH_CLEAR_BYTE; 

}

/**
  * @brief   Programs one byte in program or data EEPROM memory
  * @note   PointerAttr define is declared in the stm8s.h file to select if 
  *         the pointer will be declared as near (2 bytes) or far (3 bytes).
  * @param  Address : Address where the byte will be programmed
  * @param  Data : Value to be programmed
  * @retval None
  */
void FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
    /* Check parameters */
    assert_param(IS_FLASH_ADDRESS_OK(Address));
    *(PointerAttr uint8_t*) (uint16_t)Address = Data;
}

/**
  * @brief   Reads any byte from flash memory
  * @note   PointerAttr define is declared in the stm8s.h file to select if 
  *         the pointer will be declared as near (2 bytes) or far (3 bytes).
  * @param  Address : Address to read
  * @retval Value of the byte
  */
uint8_t FLASH_ReadByte(uint32_t Address)
{
    /* Check parameter */
    assert_param(IS_FLASH_ADDRESS_OK(Address));
    
    /* Read byte */
    return(*(PointerAttr uint8_t *) (uint16_t)Address); 

}

/**
  * @brief   Sets the fixed programming time
  * @param  FLASH_ProgTime Indicates the programming time to be fixed
  *         This parameter can be any of the @ref FLASH_ProgramTime_TypeDef values.
  * @retval None
  */
void FLASH_SetProgrammingTime(FLASH_ProgramTime_TypeDef FLASH_ProgTime)
{
    /* Check parameter */
    assert_param(IS_FLASH_PROGRAM_TIME_OK(FLASH_ProgTime));

    FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);
    FLASH->CR1 |= (uint8_t)FLASH_ProgTime;
}


