#include <stm32f4xx.h>
/******************************************* registers map:
**********************/
// RCC Memory direction for Reset and Clock Control
#define RCC 0x40023800
#define RCC_AHB1ENR_OFFSET 0x30 //Register for enable port D and C
#define RCC_APB1ENR_OFFSET 0x40 // Register for enable TIM2
// GPIO
#define GPIO_PORT_D 0x40020C00
#define GPIO_PORT_C 0x40020800
#define GPIO_ODR_OFFSET 0x14 // GPIO output
#define GPIO_IDR_OFFSET 0x10 // GPIO input
#define GPIO_MODER_OFFSET 0x0 // set the pins to input or output
//TIM2 Timer
#define TIM2_REG 0x40000000
#define TIM_SR_OFFSET 0x10
#define TIM_CNT_OFFSET 0x24
#define TIM_ARR_OFFSET 0x2c // cuando el contador alcanza este valor se
puede generar una interrupcion
#define TIM_PSC_OFFSET 0x28 // preescalar la frecuencia sysclk/psc
#define TIM_DIER_OFFSET 0x0c
#define TIM_CR1_OFFSET 0x00 // controla si la cuenta es ascendente,
descendente, permite interrupcion, etc
#define TIM_CCMR1_OFFSET 0x18 // capturar los canales de caputra7comparacion
del temporizador
#define TIM_CCER_OFFSET 0x20
/********************************* end registers map
*****************************/
/*************************************** basic bit ops
***************************************************/
#define SET_BIT(REG, BIT) ((*(uint32_t*)REG) |= (BIT))
#define CLEAR_BIT(REG, BIT) ((*(uint32_t*)REG) &= ~(BIT))
#define READ_BIT(REG, BIT) ((*(uint32_t*)REG) & (BIT))
#define CLEAR_REG(REG) ((*(uint32_t*)REG) = (0x0))
#define WRITE_REG(REG, VAL) ((*(uint32_t*)REG) = (VAL))
#define READ_REG(REG) ((*(uint32_t*)REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) WRITE_REG((REG),
(((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
void initTim2_GPIO(void);
void delay1Sec(void);
void toggleLED(int);
int result=0;
int time = 0;
_Bool incr = 1;
_Bool decr = 1;
int main (void)
{
initTim2_GPIO();
// Initialize TIM2
/***************************************/
unsigned int currentCntVal = 0;
unsigned int prevCntVal = 0;
unsigned int elapsed = 0;
while(1)
{
// if the button is pressed
if(!(*(uint32_t*)(GPIO_PORT_C+GPIO_IDR_OFFSET) & (1<<15))) {
// initialize the counter from the TIM2
*(uint32_t*)(TIM2_REG+TIM_CNT_OFFSET) = 0;
// enable tim2
SET_BIT((TIM2_REG + TIM_CR1_OFFSET), (1 << 0));
// read the current value from TIM2
prevCntVal = *(uint32_t*)(TIM2_REG+TIM_CNT_OFFSET);
// wait while the button is pressed
while(!(*(uint32_t*)(GPIO_PORT_C+GPIO_IDR_OFFSET) & (1<<15))){}
// read the current value from TIM2
currentCntVal = *(uint32_t*)(TIM2_REG+TIM_CNT_OFFSET);
// calculate the elapsed time
elapsed = currentCntVal - prevCntVal;
// disable tim2
CLEAR_BIT((TIM2_REG + TIM_CR1_OFFSET), (1 << 0));
if(elapsed < 500)
{
result += 1;
check(result);
}
else{
result -= 1;
check(result);
}
}
}
}
void initTim2_GPIO(void)
{
// enable clock for TIM2
SET_BIT((RCC + RCC_APB1ENR_OFFSET), (1 << 0));
// enable GPIOD clock
SET_BIT((RCC + RCC_AHB1ENR_OFFSET), (1 << 3));
// enable GPIOC clock
SET_BIT((RCC + RCC_AHB1ENR_OFFSET), (1 << 2));
//set pins 13, 14 y 15 from GPIOD gpio output mode
*(uint32_t*)(GPIO_PORT_D + GPIO_MODER_OFFSET) |= (1 << 26) | (1 << 28)
| (1 << 30);
// counter prescaler
*(uint32_t*)(TIM2_REG+TIM_PSC_OFFSET) = 16000-1; // psc =
16mhz/1khz - 1 =
// configure as input capture mode
*(uint32_t*)(TIM2_REG+TIM_CCMR1_OFFSET) = 0x1;
// configuracion como entrada - captura
*(uint32_t*)(TIM2_REG+TIM_CCER_OFFSET) = 0x1;
}
// we check which operation we need to do, subtraction or addition
void check(int result){
if(result >=0) addNumber(result);
else subsNumber(result);
}
// in this function we declare the logic for the addition of the numbers,
and the animation of the leds
void addNumber(int result){
switch (result)
{
case 0:
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) &= ~ (1 << 14);
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) &= ~(1 << 13);
decr = 1;
break;
case 1:
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) |= (1 << 14);
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) &= ~(1 << 13);
break;
case 2:
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) &= ~(1 << 14);
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) |= (1 << 13);
break;
case 3:
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) |= (1 << 14);
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) |= (1 << 13);
incr = 1;
break;
case 4:
if(incr){toggleLED(0); incr = 0;}
default:
toggleGreen(result-3);
}
}
// in this function we declare the logic for the substraction of the
numbers, and the animation of the leds
void subsNumber(int result){
switch (result)
{
case -1:
if(decr){toggleLED(1); decr = 0;}
toggleGreen(-1*result);
break;
default:
toggleGreen(-1*result);
}
}
// in this function we declare an animation, both leds start to toggle
void toggleLED(int neg)
{
for(int i=0; i<= 6 + neg; i++){
//INVERT THE STATE OF THE LED
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) ^= (1 << 13);
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) ^= (1 << 14);
/*GPIOA->ODR ^= (1 << 6);
GPIOA->ODR ^= (1 << 7);*/
//WAIT FOR 1 SECOND
Delay_ms (200);
}
}
// in this function we toggle the green led
void toggleGreen(int times)
{
for(int i=0; i< 2 * times; i++){
//INVERT THE STATE OF THE LED
//GPIOA->ODR ^= (1 << 7);
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) ^= (1 << 13);
//WAIT FOR 1 SECOND
Delay_ms (200);
}
}
// in this function we toggle the red led
void toggleLED2(void)
{
for(int i=0; i<=3; i++){
//INVERT THE STATE OF THE LED
*(uint32_t*)(GPIO_PORT_D + GPIO_ODR_OFFSET) ^= (1 << 14);
//WAIT FOR 1 SECOND
Delay_ms (200);
}
}
void Delay_us (uint16_t us)
{
*(uint32_t*)(TIM2_REG+TIM_CNT_OFFSET) = 0;
while (*(uint32_t*)(TIM2_REG+TIM_CNT_OFFSET) < us);
}
void Delay_ms (uint16_t ms)
{
for (uint16_t i=0; i<ms; i++)
{
Delay_us (1); // delay of 1 ms
}
}
