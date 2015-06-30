#include "includes.h"

void CLK_PeripheralClockConfig(CLK_Peripheral_TypeDef CLK_Peripheral, FunctionalState NewState)
{

    /* Check the parameters */
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));
    assert_param(IS_CLK_PERIPHERAL_OK(CLK_Peripheral));

    if (((uint8_t)CLK_Peripheral & (uint8_t)0x10) == 0x00)
    {
        if (NewState != DISABLE)
        {
            /* Enable the peripheral Clock */
            CLK->PCKENR1 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
        }
        else
        {
            /* Disable the peripheral Clock */
            CLK->PCKENR1 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
        }
    }
    else
    {
        if (NewState != DISABLE)
        {
            /* Enable the peripheral Clock */
            CLK->PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
        }
        else
        {
            /* Disable the peripheral Clock */
            CLK->PCKENR2 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
        }
    }

}

void CLK_SYSCLKConfig(CLK_Prescaler_TypeDef CLK_Prescaler)
{

    /* check the parameters */
    assert_param(IS_CLK_PRESCALER_OK(CLK_Prescaler));

    if (((uint8_t)CLK_Prescaler & (uint8_t)0x80) == 0x00) /* Bit7 = 0 means HSI divider */
    {
        CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
        CLK->CKDIVR |= (uint8_t)((uint8_t)CLK_Prescaler & (uint8_t)CLK_CKDIVR_HSIDIV);
    }
    else /* Bit7 = 1 means CPU divider */
    {
        CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_CPUDIV);
        CLK->CKDIVR |= (uint8_t)((uint8_t)CLK_Prescaler & (uint8_t)CLK_CKDIVR_CPUDIV);
    }

}

void EXTI_SetExtIntSensitivity(EXTI_Port_TypeDef Port, EXTI_Sensitivity_TypeDef SensitivityValue)
{

    /* Check function parameters */
    assert_param(IS_EXTI_PORT_OK(Port));
    assert_param(IS_EXTI_SENSITIVITY_OK(SensitivityValue));

    /* Set external interrupt sensitivity */
    switch (Port)
    {
    case EXTI_PORT_GPIOA:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PAIS);
        EXTI->CR1 |= (uint8_t)(SensitivityValue);
        break;
    case EXTI_PORT_GPIOB:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PBIS);
        EXTI->CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 2);
        break;
    case EXTI_PORT_GPIOC:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PCIS);
        EXTI->CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 4);
        break;
    case EXTI_PORT_GPIOD:
        EXTI->CR1 &= (uint8_t)(~EXTI_CR1_PDIS);
        EXTI->CR1 |= (uint8_t)((uint8_t)(SensitivityValue) << 6);
        break;
    case EXTI_PORT_GPIOE:
        EXTI->CR2 &= (uint8_t)(~EXTI_CR2_PEIS);
        EXTI->CR2 |= (uint8_t)(SensitivityValue);
        break;
    default:
        break;
    }
}

void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, GPIO_Mode_TypeDef GPIO_Mode)
{
    /*----------------------*/
    /* Check the parameters */
    /*----------------------*/

    assert_param(IS_GPIO_MODE_OK(GPIO_Mode));
    assert_param(IS_GPIO_PIN_OK(GPIO_Pin));
    
  /* Reset corresponding bit to GPIO_Pin in CR2 register */
  GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));

    /*-----------------------------*/
    /* Input/Output mode selection */
    /*-----------------------------*/

    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x80) != (uint8_t)0x00) /* Output mode */
    {
        if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x10) != (uint8_t)0x00) /* High level */
        {
            GPIOx->ODR |= (uint8_t)GPIO_Pin;
        } 
        else /* Low level */
        {
            GPIOx->ODR &= (uint8_t)(~(GPIO_Pin));
        }
        /* Set Output mode */
        GPIOx->DDR |= (uint8_t)GPIO_Pin;
    } 
    else /* Input mode */
    {
        /* Set Input mode */
        GPIOx->DDR &= (uint8_t)(~(GPIO_Pin));
    }

    /*------------------------------------------------------------------------*/
    /* Pull-Up/Float (Input) or Push-Pull/Open-Drain (Output) modes selection */
    /*------------------------------------------------------------------------*/

    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x40) != (uint8_t)0x00) /* Pull-Up or Push-Pull */
    {
        GPIOx->CR1 |= (uint8_t)GPIO_Pin;
    } 
    else /* Float or Open-Drain */
    {
        GPIOx->CR1 &= (uint8_t)(~(GPIO_Pin));
    }

    /*-----------------------------------------------------*/
    /* Interrupt (Input) or Slope (Output) modes selection */
    /*-----------------------------------------------------*/

    if ((((uint8_t)(GPIO_Mode)) & (uint8_t)0x20) != (uint8_t)0x00) /* Interrupt or Slow slope */
    {
        GPIOx->CR2 |= (uint8_t)GPIO_Pin;
    } 
    else /* No external interrupt or No slope control */
    {
        GPIOx->CR2 &= (uint8_t)(~(GPIO_Pin));
    }
}

void GPIO_Write(GPIO_TypeDef* GPIOx, uint8_t PortVal)
{
    GPIOx->ODR = PortVal;
}

void GPIO_WriteHigh(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
    GPIOx->ODR |= (uint8_t)PortPins;
}

void GPIO_WriteLow(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins)
{
    GPIOx->ODR &= (uint8_t)(~PortPins);
}

void TIM1_ClearITPendingBit(TIM1_IT_TypeDef TIM1_IT)
{
    /* Check the parameters */
    assert_param(IS_TIM1_IT_OK(TIM1_IT));

    /* Clear the IT pending Bit */
    TIM1->SR1 = (uint8_t)(~(uint8_t)TIM1_IT);
}

void TIM1_Cmd(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    /* set or Reset the CEN Bit */
    if (NewState != DISABLE)
    {
        TIM1->CR1 |= TIM1_CR1_CEN;
    }
    else
    {
        TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN);
    }
}

ITStatus TIM1_GetITStatus(TIM1_IT_TypeDef TIM1_IT)
{
    ITStatus bitstatus = RESET;
    uint8_t TIM1_itStatus = 0, TIM1_itEnable = 0;

    /* Check the parameters */
    assert_param(IS_TIM1_GET_IT_OK(TIM1_IT));

    TIM1_itStatus = (uint8_t)(TIM1->SR1 & (uint8_t)TIM1_IT);

    TIM1_itEnable = (uint8_t)(TIM1->IER & (uint8_t)TIM1_IT);

    if ((TIM1_itStatus != (uint8_t)RESET ) && (TIM1_itEnable != (uint8_t)RESET ))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return (ITStatus)(bitstatus);
}

void TIM1_SetCompare1(uint16_t Compare1) {
    /* Set the Capture Compare1 Register value */
    TIM1->CCR1H = (uint8_t)(Compare1 >> 8);
    TIM1->CCR1L = (uint8_t)(Compare1);
}

void TIM1_CtrlPWMOutputs(FunctionalState NewState) {
    /* Set or Reset the MOE Bit */
    if (NewState != DISABLE) {
        TIM1->BKR |= TIM1_BKR_MOE;
    } else {
        TIM1->BKR &= (uint8_t)(~TIM1_BKR_MOE);
    }
}


void TIM1_TimeBaseInit(uint16_t TIM1_Prescaler,
                       TIM1_CounterMode_TypeDef TIM1_CounterMode,
                       uint16_t TIM1_Period,
                       uint8_t TIM1_RepetitionCounter)
{

    /* Check parameters */
    assert_param(IS_TIM1_COUNTER_MODE_OK(TIM1_CounterMode));

    /* Set the Autoreload value */
    TIM1->ARRH = (uint8_t)(TIM1_Period >> 8);
    TIM1->ARRL = (uint8_t)(TIM1_Period);

    /* Set the Prescaler value */
    TIM1->PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
    TIM1->PSCRL = (uint8_t)(TIM1_Prescaler);

    /* Select the Counter Mode */
    TIM1->CR1 = (uint8_t)((uint8_t)(TIM1->CR1 & (uint8_t)(~(TIM1_CR1_CMS | TIM1_CR1_DIR)))
                           | (uint8_t)(TIM1_CounterMode));

    /* Set the Repetition Counter value */
    TIM1->RCR = TIM1_RepetitionCounter;

}

void TIM1_OC1Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState)
{
    // Check the parameters 
    assert_param(IS_TIM1_OC_MODE_OK(TIM1_OCMode));
    assert_param(IS_TIM1_OUTPUT_STATE_OK(TIM1_OutputState));
    assert_param(IS_TIM1_OUTPUTN_STATE_OK(TIM1_OutputNState));
    assert_param(IS_TIM1_OC_POLARITY_OK(TIM1_OCPolarity));
    assert_param(IS_TIM1_OCN_POLARITY_OK(TIM1_OCNPolarity));
    assert_param(IS_TIM1_OCIDLE_STATE_OK(TIM1_OCIdleState));
    assert_param(IS_TIM1_OCNIDLE_STATE_OK(TIM1_OCNIdleState));

    // Disable the Channel 1: Reset the CCE Bit, Set the Output State , the Output N State, the Output Polarity & the Output N Polarity
    TIM1->CCER1 &= (uint8_t)(~( TIM1_CCER1_CC1E | TIM1_CCER1_CC1NE 
                               | TIM1_CCER1_CC1P | TIM1_CCER1_CC1NP));
    // Set the Output State & Set the Output N State & Set the Output Polarity & Set the Output N Polarity 
  TIM1->CCER1 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OutputState & TIM1_CCER1_CC1E)
                                     | (uint8_t)(TIM1_OutputNState & TIM1_CCER1_CC1NE))
                           | (uint8_t)( (uint8_t)(TIM1_OCPolarity  & TIM1_CCER1_CC1P)
                                        | (uint8_t)(TIM1_OCNPolarity & TIM1_CCER1_CC1NP)));

    // Reset the Output Compare Bits & Set the Output Compare Mode 
    TIM1->CCMR1 = (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~TIM1_CCMR_OCM)) | 
                            (uint8_t)TIM1_OCMode);

    // Reset the Output Idle state & the Output N Idle state bits 
    TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS1 | TIM1_OISR_OIS1N));
    // Set the Output Idle state & the Output N Idle state configuration 
    TIM1->OISR |= (uint8_t)((uint8_t)( TIM1_OCIdleState & TIM1_OISR_OIS1 ) | 
                            (uint8_t)( TIM1_OCNIdleState & TIM1_OISR_OIS1N ));

    // Set the Pulse value 
    TIM1->CCR1H = (uint8_t)(TIM1_Pulse >> 8);
    TIM1->CCR1L = (uint8_t)(TIM1_Pulse);
}



void FLASH_Unlock(FLASH_MemType_TypeDef FLASH_MemType) {
    if (FLASH_MemType == FLASH_MEMTYPE_PROG) {
        FLASH->PUKR = FLASH_RASS_KEY1;
        FLASH->PUKR = FLASH_RASS_KEY2;
    } else {
        FLASH->DUKR = FLASH_RASS_KEY2;
        FLASH->DUKR = FLASH_RASS_KEY1;
    }
}

void FLASH_Lock(FLASH_MemType_TypeDef FLASH_MemType) {
  FLASH->IAPSR &= (uint8_t)FLASH_MemType;
}

void FLASH_ProgramByte(uint32_t Address, uint8_t Data) {
    *(PointerAttr uint8_t*) (uint16_t)Address = Data;
}

uint8_t FLASH_ReadByte(uint32_t Address) {
    return(*(PointerAttr uint8_t *) (uint16_t)Address); 
}

