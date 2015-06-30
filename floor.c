#include "includes.h"
#include "eeprom.h"
#include "modbus_slave.h"

//registers
// 0 - address
// 1 - do not do anything timeout (sec)
// 2,3 - uptime in sec
// 4 - DS18b20 temperature (set to 0xffff to disable sensor)
// 5 - LM75 temperature (set to 0xffff to disable sensor)
// //1880, 1920; 1860, 18e0; 18d0, 1b80;
// //1630,15e0 - cold; 1660, 1760 - avg; 16c0, 1880; 16e0, 18e0 - ok; 
// 1700, 1920 - good
// 17a0, 1940
// 6   - Off temperature (high)
// 7   - On  temperature (low)
// 8   - auto action 
//       0 - do nothing
//       4 - based on register 4 (DS18B20)
//       5 - based on register 5 (LM75)
//       10 - based on register 10  (external)
// 9    - current status
// 10   - external temperature (put here temp from ext. sensor)
// 11   - LED PWM value
// 14   - set 0xf00f - system reset
// 15   - write here my address to save config to eeprom
volatile char wakeup=0;
int16_t GetTemperature(void);
uint16_t regs[16];
uint8_t modbus_ready=0;

void applyOut(void){
    if(regs[9])
        GPIO_WriteLow(GPIOD,GPIO_PIN_0);
    else
        GPIO_WriteHigh(GPIOD,GPIO_PIN_0);
}

#define _nop_() __asm__("nop")
void x_delay(unsigned int i){ 
      while (--i) _nop_();
} 
#define delay_1us()   _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
#define delay_15us()  x_delay(42)
#define delay_45us()  x_delay(127)
#define delay_60us()  x_delay(170)
#define delay_480us() x_delay(1360)

int16_t on=25;
int16_t off=40;
#define TXTDEBUG

void cout(const char c) {
    while(!(UART1->SR & UART1_SR_TXE)) {}
    UART1->CR2 &= (uint8_t)(~UART1_CR2_REN);  
    UART1_SendData8(c);
    while(!(UART1->SR & UART1_SR_TXE)) {}
    delay_480us();
    UART1->CR2 |= (uint8_t)UART1_CR2_REN;  
}

#ifdef TXTDEBUG
void UART_Send(const char *s) {
    char c;
    while ( (c = *s++) ) {
        cout(c);
    }
}

void xcout(uint16_t i){
    int c;
    for(c=4;c>0;c--){
	uint16_t q=(i&0xF000)>>12;
	if(q<10){
	    cout('0'+q);
	}else{
	    cout('A'+q-10);
	}
	i<<=4;
    }
    cout('\r');
    cout('\n');

}
#endif


/*
void PORTA_EXTI(void) __interrupt(3) {
#ifdef TXTDEBUG
    UART_Send("A_EXTI\r\n");
#endif
}

void TIM1CC_vector(void) __interrupt(12) { //capture
//    cout('t');
//    xcout(rxs[4]);
    if(TIM1_GetITStatus(TIM1_IT_UPDATE)==SET){
        TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
    }
#ifdef TXTDEBUG
    UART_Send("TIM1CC\r\n");
#endif
}
*/

void UART1_RX_vector(void) __interrupt(18) {
    while(UART1_GetITStatus(UART1_IT_RXNE)){
        char c=UART1_ReceiveData8();
        modbus_read(c);
        modbus_ready=2;
        /*
        if(c=='R'){
            cout('O');
            cout('K');
            cout('\r');
            cout('\n');
        }else if((c>='0' && c<='9')){
                buf<<=4;
                buf|=c-'0';
                xcout(buf);
        }else if((c>='a' && c<='f')){
                buf<<=4;
                buf|=c-'a'+10;
                xcout(buf);
        }else if((c>='A' && c<='F')){
                buf<<=4;
                buf|=c-'A'+10;
                xcout(buf);
        }
        */
    }
}


void TIM2OVF(void) __interrupt(13) { //overflow
    static sec=224;
    if(TIM2_GetITStatus(TIM2_IT_UPDATE)==SET){
        TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
    }

#ifdef TXTDEBUG
#ifdef DEBUG
    if(!GPIO_ReadInputPin(GPIOD, GPIO_PIN_2)){
        UART_Send("1");
    }
    if(!GPIO_ReadInputPin(GPIOC, GPIO_PIN_7)){
        UART_Send("2\r\n");
    }
#endif
#endif

    sec--;
    if(!sec){
        sec=448;
        wakeup=1;
    }

    if(modbus_ready){
        modbus_ready--;
        if(!modbus_ready){
            uint16_t r=modbus_update();
            //delay_480us();
            //cout(r>>8);
            //cout(r&255);
//            UART_Send("MB: ");
//            xcout(modbus_update());
        }
    }
}


void readee(){
    /*
    if(FLASH_ReadByte(0x4000)==0x33){
//        cmode=FLASH_ReadByte(0x4001);
        UART_Send("OK\r\n");
    }else{
//        eesave=1;
        UART_Send("BEE\r\n");
    }
    */
}

void write_register(uint16_t index, uint16_t value){
    regs[index]=value;
    if(index==0){
        modbus_configure(regs[0],16,regs,NULL,cout,write_register);
    }else if(index==14 && value==0xf00f){
        __asm__("LDW X,  SP ");
        __asm__("LD  A,  255");
        __asm__("LD  XL, A  ");
        __asm__("LDW SP, X  ");
        __asm__ ("jpf 0x8000"); //go to start
    }else if(index==9){
        applyOut();
    }else if(index==11){
        TIM1_SetCompare1(regs[11]);
    }else if(index==15){
        regs[15]=0xFF00;
        if(value==regs[0]){
            int i=0;
            FLASH_Unlock(FLASH_MEMTYPE_DATA);
            regs[15]=0x3333;
            for(;i<=15;i++){
                uint16_t b=0x4000+(i<<1);
                FLASH_ProgramByte(b, regs[i]>>8);
                FLASH_ProgramByte(b+1, regs[i]&0xff);
            }
            FLASH_Lock(FLASH_MEMTYPE_DATA);
            regs[15]=0x100;
        }
    }
}

void mainloop();
int main(void) {
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

    UART1_DeInit();
    UART1_Init(8, 10, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,     
            UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    UART1_Cmd(ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR,ENABLE);
    //UART_Send("Preved\r\n");

    GPIO_Init(GPIOD, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST); //Heat
    GPIO_WriteHigh(GPIOD,GPIO_PIN_0);

    GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST); //Led

    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);

    TIM2_TimeBaseInit(TIM2_PRESCALER_64, 512);//16384);
    TIM2_Cmd(ENABLE);
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);

    TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 65535, 0);
    TIM1_Cmd(ENABLE);
    TIM1_OC1Init(TIM1_OCMODE_PWM2,TIM1_OUTPUTSTATE_ENABLE,TIM1_OUTPUTNSTATE_DISABLE,1024,
            TIM1_OCPOLARITY_LOW, 0, TIM1_OCIDLESTATE_SET, 0);
    TIM1_CtrlPWMOutputs(ENABLE);

    /*
       GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST); 
       GPIO_WriteHigh(GPIOC,GPIO_PIN_1);
       void TIM1_OC1Init(TIM1_OCMode_TypeDef TIM1_OCMode, 
       TIM1_OutputState_TypeDef TIM1_OutputState, 
       TIM1_OutputNState_TypeDef TIM1_OutputNState, 
       uint16_t TIM1_Pulse, TIM1_OCPolarity_TypeDef TIM1_OCPolarity, 
       TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity, 
       TIM1_OCIdleState_TypeDef TIM1_OCIdleState, 
       TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);

       void TIM1_ITConfig(TIM1_IT_TypeDef TIM1_IT, FunctionalState NewState);
       void TIM1_SetCompare1(uint16_t Compare1);
       */
    //TIM1_SetCompare1(0x1000);
    /*    TIM1_ClearFlag(TIM1_FLAG_CC1);
          TIM1_ITConfig(TIM1_IT_CC1, ENABLE);
          */
    if(FLASH_ReadByte(0x401f)==0x33){
        int i=0;
        for(;i<=15;i++){
            uint16_t b,val;
            b=0x4000+(i<<1);
            val=FLASH_ReadByte(b)<<8;
            val|=FLASH_ReadByte(b+1);
            regs[i]=val;
        }
    }else{
        regs[0]=0x10; //default address
    }
    regs[1]=30; //do not run within 30 sec.
    regs[2]=regs[3]=0; //flush uptime
    regs[9]=0; //disable output
    applyOut();
    //TIM1_SetCompare1(regs[11]);
    modbus_configure(regs[0],16,regs,NULL,cout,write_register);
    I2C_init();

    readee();
    enableInterrupts();

    while(1){
        mainloop();
    }
}

void mainloop(){
    int16_t temp;
    if(!wakeup){
        __asm__("wfi");
    }

    if(regs[3]==0xffff){
        regs[2]++;
        regs[3]=0;
    }else{
        regs[3]++;
    }

    if(regs[4]<0xffff){
        temp=GetTemperature();
        regs[4]=temp;
    }
    //            xcout(temp);

    //            GPIO_WriteHigh(GPIOD,GPIO_PIN_2);
    if(regs[5]<0xffff){
        temp=LM75_Read();
        regs[5]=temp;
    }

    if(regs[1]){
        regs[1]--;
    }else{
        if(regs[8]==4 || regs[8]==5){
            temp=regs[regs[8]];
            if(temp<regs[7]){
                regs[9] = 1;
            } else if(temp>regs[6]) {
                regs[9] = 0;
            }
            applyOut();
        }
    }
    /*
       if(temp<0){
       UART_Send("-");
       temp=-temp;
       }else{
       if((temp>>8)<on)
       GPIO_WriteLow(GPIOD,GPIO_PIN_0);
       else if((temp>>8)>off)
       GPIO_WriteHigh(GPIOD,GPIO_PIN_0);
       }
       xcout(temp>>8);

       UART_Send(".");
       xcout((temp&0xff)>>5);
       UART_Send("\r\n");
       */
    wakeup=0;
}



#define THERM_PORT GPIOD
#define THERM_PIN  GPIO_PIN_3
 
#define THERM_INPUT_MODE()  THERM_PORT->DDR &= ~THERM_PIN
#define THERM_OUTPUT_MODE() THERM_PORT->DDR |=  THERM_PIN
#define THERM_LOW()         THERM_PORT->ODR &= (u8)(~THERM_PIN)
#define THERM_HIGH()        THERM_PORT->ODR |= (u8)THERM_PIN
#define THERM_READ()        (THERM_PORT->IDR & (u8)THERM_PIN)

// Команды
#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xBE
#define THERM_CMD_WSCRATCHPAD 0x4E
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xB8
#define THERM_CMD_RPWRSUPPLY 0xB4
#define THERM_CMD_SEARCHROM 0xF0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xCC
#define THERM_CMD_ALARMSEARCH 0xEC

#define THERM_DECIMAL_STEPS_12BIT 625 //.0625

// Режимы работы. По-умолчанию 12 бит.
typedef enum {
    THERM_MODE_9BIT  = 0x1F,
    THERM_MODE_10BIT = 0x3F,
    THERM_MODE_11BIT = 0x5F,
    THERM_MODE_12BIT = 0x7F
} THERM_MODE;


/**
 * @brief Сброс термодатчика
 * @par
 * Параметры отсутствуют
 * @retval
 * false - Неисправность; true - Норма
 */
bool therm_reset(void) {
    unsigned char i = 0xFF;
    THERM_OUTPUT_MODE();
    THERM_LOW();
    delay_480us();
    THERM_INPUT_MODE();
    delay_60us();
    i = THERM_READ();
    delay_480us();
    // 0 означает правильный сброс, 1 - ошибка
    return (0 == i) ? 1 : 0;
}

/**
 * @brief Запись бита
 * @param[in] bBit бит
 * @retval
 * Возвращаемое значение отсутствует
 */
void therm_write_bit(bool bBit) {
    THERM_OUTPUT_MODE();
    THERM_LOW();
    delay_1us();
    if (bBit) 
        THERM_INPUT_MODE();
    delay_60us();
    THERM_INPUT_MODE();
}

/**
 * @brief Чтение бита
 * @par
 * Параметры отсутствуют
 * @retval
 * Значение бита.
 */
bool therm_read_bit(void) {
    bool bBit = 0;

    THERM_OUTPUT_MODE();
    THERM_LOW(); 
    delay_1us();
    THERM_INPUT_MODE();
    delay_15us();
    if (THERM_READ()) 
        bBit = 1;
    delay_45us();
    return bBit;
}

/**
 * @brief Чтение байта
 * @par
 * Параметры отсутствуют
 * @retval
 * Значение байта.
 */
static unsigned char therm_read_byte(void)
{
    unsigned char i = 8;
    unsigned char n = 0;
    while (i--)
    {    
        // Сдвинуть на одну позицию вправо и сохранить значение бита
        n >>= 1;
        n |= (therm_read_bit() << 7);
    }
    return n;
}

/**
 * @brief Запись байта
 * @param[in] byte байт
 * @retval
 * Возвращаемое значение отсутствует
 */
void therm_write_byte(unsigned char byte) {
    unsigned char i = 8;

    while (i--)
    {
        // Записать текущий бит и сдвинуть на 1 позицию вправо
        // для доступа к следующему биту
        therm_write_bit(byte & 1);
        byte >>= 1;
    }  
}

/**
 * @brief Установить режим работы термодатчика
 * @param[in] mode Режим работы
 * @retval
 * Возвращаемое значение отсутствует
 */
void therm_init_mode(THERM_MODE mode) {
    therm_reset();
    therm_write_byte(THERM_CMD_SKIPROM);
    therm_write_byte(THERM_CMD_WSCRATCHPAD);
    therm_write_byte(0);
    therm_write_byte(0);
    therm_write_byte(mode);
}


/**
 * @brief Чтение температуры.
 * @par
 * Параметры отсутствуют
 * @retval
 * Температура.
 */
int16_t GetTemperature(void)
{
    char iResult = 0;
    unsigned char temperature[2] = {0, 0};
    unsigned char digit;
    short iReadLimit;

    // Запрет прерываний во время опроса датчика
    disableInterrupts();  

    // Сброс и сразу переход к преобразованию температуры
    iResult = therm_reset();
    therm_write_byte(THERM_CMD_SKIPROM);
    therm_write_byte(THERM_CMD_CONVERTTEMP);

    // Ожидание завершения преобразования
    iReadLimit = 10;
    while (!therm_read_bit() && (--iReadLimit > 0)) { ; }

    // Сброс и чтение байт температуры
    therm_reset();
    therm_write_byte(THERM_CMD_SKIPROM);
    therm_write_byte(THERM_CMD_RSCRATCHPAD);  
    temperature[0] = therm_read_byte();
    temperature[1] = therm_read_byte();

    // Разрешить прерывания
    enableInterrupts();  
    return ((temperature[1]&0x0f)<<12)|(temperature[0]<<4);
    /*

       digit = 0;

       digit  = (  temperature[0]         >> 4 ) & 0x0F;  
       digit |= ( (temperature[1] & 0x0F) << 4 ) & 0xF0;  

    // Отрицательная температура - инвертировать и прибавить 1
    if (temperature[1] & 0x80)
    {
    iResult = -(~digit); 
    iResult--;
    } else
    {
    iResult = digit; 
    }

    return iResult;
    */
}
