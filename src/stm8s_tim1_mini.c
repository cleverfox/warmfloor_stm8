/**
  ******************************************************************************
  * @file    stm8s_tim1.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   This file contains all the functions for the TIM1 peripheral.
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
#include "stm8s_tim1.h"

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void TI1_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);
static void TI2_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);
static void TI3_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);
static void TI4_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);

/**
  * @addtogroup TIM1_Public_Functions
  * @{
  */

/**
  * @brief  Deinitializes the TIM1 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void TIM1_DeInit(void)
{
    TIM1->CR1  = TIM1_CR1_RESET_VALUE;
    TIM1->CR2  = TIM1_CR2_RESET_VALUE;
    TIM1->SMCR = TIM1_SMCR_RESET_VALUE;
    TIM1->ETR  = TIM1_ETR_RESET_VALUE;
    TIM1->IER  = TIM1_IER_RESET_VALUE;
    TIM1->SR2  = TIM1_SR2_RESET_VALUE;
    /* Disable channels */
    TIM1->CCER1 = TIM1_CCER1_RESET_VALUE;
    TIM1->CCER2 = TIM1_CCER2_RESET_VALUE;
    /* Configure channels as inputs: it is necessary if lock level is equal to 2 or 3 */
    TIM1->CCMR1 = 0x01;
    TIM1->CCMR2 = 0x01;
    TIM1->CCMR3 = 0x01;
    TIM1->CCMR4 = 0x01;
    /* Then reset channel registers: it also works if lock level is equal to 2 or 3 */
    TIM1->CCER1 = TIM1_CCER1_RESET_VALUE;
    TIM1->CCER2 = TIM1_CCER2_RESET_VALUE;
    TIM1->CCMR1 = TIM1_CCMR1_RESET_VALUE;
    TIM1->CCMR2 = TIM1_CCMR2_RESET_VALUE;
    TIM1->CCMR3 = TIM1_CCMR3_RESET_VALUE;
    TIM1->CCMR4 = TIM1_CCMR4_RESET_VALUE;
    TIM1->CNTRH = TIM1_CNTRH_RESET_VALUE;
    TIM1->CNTRL = TIM1_CNTRL_RESET_VALUE;
    TIM1->PSCRH = TIM1_PSCRH_RESET_VALUE;
    TIM1->PSCRL = TIM1_PSCRL_RESET_VALUE;
    TIM1->ARRH  = TIM1_ARRH_RESET_VALUE;
    TIM1->ARRL  = TIM1_ARRL_RESET_VALUE;
    TIM1->CCR1H = TIM1_CCR1H_RESET_VALUE;
    TIM1->CCR1L = TIM1_CCR1L_RESET_VALUE;
    TIM1->CCR2H = TIM1_CCR2H_RESET_VALUE;
    TIM1->CCR2L = TIM1_CCR2L_RESET_VALUE;
    TIM1->CCR3H = TIM1_CCR3H_RESET_VALUE;
    TIM1->CCR3L = TIM1_CCR3L_RESET_VALUE;
    TIM1->CCR4H = TIM1_CCR4H_RESET_VALUE;
    TIM1->CCR4L = TIM1_CCR4L_RESET_VALUE;
    TIM1->OISR  = TIM1_OISR_RESET_VALUE;
    TIM1->EGR   = 0x01; /* TIM1_EGR_UG */
    TIM1->DTR   = TIM1_DTR_RESET_VALUE;
    TIM1->BKR   = TIM1_BKR_RESET_VALUE;
    TIM1->RCR   = TIM1_RCR_RESET_VALUE;
    TIM1->SR1   = TIM1_SR1_RESET_VALUE;
}

/**
  * @brief  Initializes the TIM1 Time Base Unit according to the specified parameters.
  * @param  TIM1_Prescaler specifies the Prescaler value.
  * @param  TIM1_CounterMode specifies the counter mode  from @ref TIM1_CounterMode_TypeDef .
  * @param  TIM1_Period specifies the Period value.
  * @param  TIM1_RepetitionCounter specifies the Repetition counter value
  * @retval None
  */
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


/**
  * @brief  Enables or disables the TIM1 peripheral.
  * @param  NewState new state of the TIM1 peripheral.
	*         This parameter can be ENABLE or DISABLE.
  * @retval None
  */
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

/**
  * @brief  Enables or disables the specified TIM1 interrupts.
  * @param  NewState new state of the TIM1 peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @param  TIM1_IT specifies the TIM1 interrupts sources to be enabled or disabled.
  *         This parameter can be any combination of the following values:
  *           - TIM1_IT_UPDATE: TIM1 update Interrupt source
  *           - TIM1_IT_CC1: TIM1 Capture Compare 1 Interrupt source
  *           - TIM1_IT_CC2: TIM1 Capture Compare 2 Interrupt source
  *           - TIM1_IT_CC3: TIM1 Capture Compare 3 Interrupt source
  *           - TIM1_IT_CC4: TIM1 Capture Compare 4 Interrupt source
  *           - TIM1_IT_CCUpdate: TIM1 Capture Compare Update Interrupt source
  *           - TIM1_IT_TRIGGER: TIM1 Trigger Interrupt source
  *           - TIM1_IT_BREAK: TIM1 Break Interrupt source
  * @param  NewState new state of the TIM1 peripheral.
  * @retval None
  */
void TIM1_ITConfig(TIM1_IT_TypeDef  TIM1_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_TIM1_IT_OK(TIM1_IT));
    assert_param(IS_FUNCTIONALSTATE_OK(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the Interrupt sources */
        TIM1->IER |= (uint8_t)TIM1_IT;
    }
    else
    {
        /* Disable the Interrupt sources */
        TIM1->IER &= (uint8_t)(~(uint8_t)TIM1_IT);
    }
}

/**
  * @brief  Configures the TIM1 internal Clock.
  * @param  None
  * @retval None
  */
void TIM1_InternalClockConfig(void)
{
    /* Disable slave mode to clock the prescaler directly with the internal clock */
    TIM1->SMCR &= (uint8_t)(~TIM1_SMCR_SMS);
}

/**
  * @brief  Sets the TIM1 Counter Register value.
  * @param   Counter specifies the Counter register new value.
  * This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCounter(uint16_t Counter)
{
    /* Set the Counter Register value */
    TIM1->CNTRH = (uint8_t)(Counter >> 8);
    TIM1->CNTRL = (uint8_t)(Counter);

}


/**
  * @brief  Sets the TIM1 Capture Compare1 Register value.
  * @param   Compare1 specifies the Capture Compare1 register new value.
  * This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCompare1(uint16_t Compare1)
{
    /* Set the Capture Compare1 Register value */
    TIM1->CCR1H = (uint8_t)(Compare1 >> 8);
    TIM1->CCR1L = (uint8_t)(Compare1);

}


/**
  * @brief  Sets the TIM1 Capture Compare2 Register value.
  * @param   Compare2 specifies the Capture Compare2 register new value.
  * This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCompare2(uint16_t Compare2)
{
    /* Set the Capture Compare2 Register value */
    TIM1->CCR2H = (uint8_t)(Compare2 >> 8);
    TIM1->CCR2L = (uint8_t)(Compare2);

}

/**
  * @brief  Checks whether the specified TIM1 flag is set or not.
  * @param  TIM1_FLAG specifies the flag to check.
  *         This parameter can be one of the following values:
  *                   - TIM1_FLAG_UPDATE: TIM1 update Flag
  *                   - TIM1_FLAG_CC1: TIM1 Capture Compare 1 Flag
  *                   - TIM1_FLAG_CC2: TIM1 Capture Compare 2 Flag
  *                   - TIM1_FLAG_CC3: TIM1 Capture Compare 3 Flag
  *                   - TIM1_FLAG_CC4: TIM1 Capture Compare 4 Flag
  *                   - TIM1_FLAG_COM: TIM1 Commutation Flag
  *                   - TIM1_FLAG_TRIGGER: TIM1 Trigger Flag
  *                   - TIM1_FLAG_BREAK: TIM1 Break Flag
  *                   - TIM1_FLAG_CC1OF: TIM1 Capture Compare 1 overcapture Flag
  *                   - TIM1_FLAG_CC2OF: TIM1 Capture Compare 2 overcapture Flag
  *                   - TIM1_FLAG_CC3OF: TIM1 Capture Compare 3 overcapture Flag
  *                   - TIM1_FLAG_CC4OF: TIM1 Capture Compare 4 overcapture Flag
  * @retval FlagStatus The new state of TIM1_FLAG (SET or RESET).
  */
FlagStatus TIM1_GetFlagStatus(TIM1_FLAG_TypeDef TIM1_FLAG)
{
    FlagStatus bitstatus = RESET;
    uint8_t tim1_flag_l = 0, tim1_flag_h = 0;

    /* Check the parameters */
    assert_param(IS_TIM1_GET_FLAG_OK(TIM1_FLAG));

    tim1_flag_l = (uint8_t)(TIM1->SR1 & (uint8_t)TIM1_FLAG);
    tim1_flag_h = (uint8_t)((uint16_t)TIM1_FLAG >> 8);

    if ((tim1_flag_l | (uint8_t)(TIM1->SR2 & tim1_flag_h)) != 0)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return (FlagStatus)(bitstatus);
}

/**
  * @brief  Clears the TIM1’s pending flags.
  * @param  TIM1_FLAG specifies the flag to clear.
  *         This parameter can be one of the following values:
  *                       - TIM1_FLAG_UPDATE: TIM1 update Flag
  *                       - TIM1_FLAG_CC1: TIM1 Capture Compare 1 Flag
  *                       - TIM1_FLAG_CC2: TIM1 Capture Compare 2 Flag
  *                       - TIM1_FLAG_CC3: TIM1 Capture Compare 3 Flag
  *                       - TIM1_FLAG_CC4: TIM1 Capture Compare 4 Flag
  *                       - TIM1_FLAG_COM: TIM1 Commutation Flag
  *                       - TIM1_FLAG_TRIGGER: TIM1 Trigger Flag
  *                       - TIM1_FLAG_BREAK: TIM1 Break Flag
  *                       - TIM1_FLAG_CC1OF: TIM1 Capture Compare 1 overcapture Flag
  *                       - TIM1_FLAG_CC2OF: TIM1 Capture Compare 2 overcapture Flag
  *                       - TIM1_FLAG_CC3OF: TIM1 Capture Compare 3 overcapture Flag
  *                       - TIM1_FLAG_CC4OF: TIM1 Capture Compare 4 overcapture Flag
  * @retval None.
  */
void TIM1_ClearFlag(TIM1_FLAG_TypeDef TIM1_FLAG)
{
    /* Check the parameters */
    assert_param(IS_TIM1_CLEAR_FLAG_OK(TIM1_FLAG));

    /* Clear the flags (rc_w0) clear this bit by writing 0. Writing ‘1’ has no effect*/
    TIM1->SR1 = (uint8_t)(~(uint8_t)(TIM1_FLAG));
    TIM1->SR2 = (uint8_t)((uint8_t)(~((uint8_t)((uint16_t)TIM1_FLAG >> 8))) & 
                          (uint8_t)0x1E);
}

/**
  * @brief  Checks whether the TIM1 interrupt has occurred or not.
  * @param  TIM1_IT specifies the TIM1 interrupt source to check.
  *         This parameter can be one of the following values:
  *                       - TIM1_IT_UPDATE: TIM1 update Interrupt source
  *                       - TIM1_IT_CC1: TIM1 Capture Compare 1 Interrupt source
  *                       - TIM1_IT_CC2: TIM1 Capture Compare 2 Interrupt source
  *                       - TIM1_IT_CC3: TIM1 Capture Compare 3 Interrupt source
  *                       - TIM1_IT_CC4: TIM1 Capture Compare 4 Interrupt source
  *                       - TIM1_IT_COM: TIM1 Commutation Interrupt source
  *                       - TIM1_IT_TRIGGER: TIM1 Trigger Interrupt source
  *                       - TIM1_IT_BREAK: TIM1 Break Interrupt source
  * @retval ITStatus The new state of the TIM1_IT(SET or RESET).
  */
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

/**
  * @brief  Clears the TIM1's interrupt pending bits.
  * @param  TIM1_IT specifies the pending bit to clear.
  *         This parameter can be one of the following values:
  *                       - TIM1_IT_UPDATE: TIM1 update Interrupt source
  *                       - TIM1_IT_CC1: TIM1 Capture Compare 1 Interrupt source
  *                       - TIM1_IT_CC2: TIM1 Capture Compare 2 Interrupt source
  *                       - TIM1_IT_CC3: TIM1 Capture Compare 3 Interrupt source
  *                       - TIM1_IT_CC4: TIM1 Capture Compare 4 Interrupt source
  *                       - TIM1_IT_COM: TIM1 Commutation Interrupt source
  *                       - TIM1_IT_TRIGGER: TIM1 Trigger Interrupt source
  *                       - TIM1_IT_BREAK: TIM1 Break Interrupt source
  * @retval None.
  */
void TIM1_ClearITPendingBit(TIM1_IT_TypeDef TIM1_IT)
{
    /* Check the parameters */
    assert_param(IS_TIM1_IT_OK(TIM1_IT));

    /* Clear the IT pending Bit */
    TIM1->SR1 = (uint8_t)(~(uint8_t)TIM1_IT);
}

/**
  * @brief  Gets the TIM1 Counter value.
  * @param  None
  * @retval Counter Register value.
  */
uint16_t TIM1_GetCounter(void)
{
  uint16_t tmpcntr = 0;
  
  tmpcntr = ((uint16_t)TIM1->CNTRH << 8);
   
  /* Get the Counter Register value */
    return (uint16_t)(tmpcntr | (uint16_t)(TIM1->CNTRL));
}


