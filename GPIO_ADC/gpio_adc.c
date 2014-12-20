#include "gpio_adc.h"
#include "stm32f4_discovery.h"


uint32_t ADC_not_configured=1;
uint8_t led5_status_var=0;
uint8_t GPIO_not_configured=1;
uint8_t out1_status=0;
uint8_t out2_status=0;


GPIO_InitTypeDef GPIO_InitDef;


void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable ADC1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Configure ADC Channel 7 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ADC Common Init */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Configuration ------------------------------------------------------*/
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 Regular Channel Config */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_56Cycles);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* ADC1 regular Software Start Conv */
	ADC_SoftwareStartConv(ADC1);
}


void ADC_Handler(char *pcInsert)
{
	/* We have only one SSI handler iIndex = 0 */

	char Digit1=0, Digit2=0, Digit3=0, Digit4=0;
    uint32_t ADCVal = 0;

    if (ADC_not_configured ==1) {
    	ADC_Configuration();
    	ADC_not_configured=0;
    }
	/* get ADC conversion value */
    ADCVal = ADC_GetConversionValue(ADC1);

    /* convert to Voltage,  step = 0.8 mV */
    ADCVal = (uint32_t)(ADCVal * 0.8);

    /* get digits to display */

    Digit1= ADCVal/1000;
    Digit2= (ADCVal-(Digit1*1000))/100 ;
    Digit3= (ADCVal-((Digit1*1000)+(Digit2*100)))/10;
    Digit4= ADCVal -((Digit1*1000)+(Digit2*100)+ (Digit3*10));

    /* prepare data to be inserted in html */
    *pcInsert       = (char)(Digit1+0x30);
    *(pcInsert + 1) = (char)(Digit2+0x30);
    *(pcInsert + 2) = (char)(Digit3+0x30);
    *(pcInsert + 3) = (char)(Digit4+0x30);
    *(pcInsert + 4) = 0;


}

void LED5_off(void) {
	STM_EVAL_LEDOff(LED5);
	led5_status_var=0;
}

void LED5_on(void) {
	STM_EVAL_LEDOn(LED5);
	led5_status_var=1;
}

uint8_t LED5_status(void) {
	return led5_status_var;
}

void GPIO_init()
{
	RCC_AHB1PeriphClockCmd(OUT1_RCC, ENABLE);
	GPIO_InitDef.GPIO_Pin = OUT1_PIN;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(OUT1_PORT, &GPIO_InitDef);

	RCC_AHB1PeriphClockCmd(OUT2_RCC, ENABLE);
	GPIO_InitDef.GPIO_Pin = OUT2_PIN;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(OUT2_PORT, &GPIO_InitDef);

	RCC_AHB1PeriphClockCmd(IN1_RCC, ENABLE);
	GPIO_InitDef.GPIO_Pin = IN1_PIN;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IN1_PORT, &GPIO_InitDef);

	GPIO_not_configured=0;
}

void OUT1_off(void) {
	// Konfiguracja
	if(GPIO_not_configured) GPIO_init();
	OUT1_PORT->BSRRH = OUT1_PIN;
	out1_status=0;
}

void OUT1_on(void) {
	// Konfiguracja
	if(GPIO_not_configured) GPIO_init();
	OUT1_PORT->BSRRL = OUT1_PIN;
	out1_status=1;
}

uint8_t OUT1_status(void) {
	// Konfiguracja
	if(GPIO_not_configured) GPIO_init();
	return out1_status;
}

void OUT2_off(void) {
	// Konfiguracja
	if(GPIO_not_configured) GPIO_init();
	OUT2_PORT->BSRRH = OUT2_PIN;
	out2_status=0;
}

void OUT2_on(void) {
	// Konfiguracja
	if(GPIO_not_configured) GPIO_init();
	OUT2_PORT->BSRRL = OUT2_PIN;
	out2_status=1;
}

uint8_t OUT2_status(void) {
	// Konfiguracja
	if(GPIO_not_configured) GPIO_init();
	return out2_status;
}

uint8_t IN1_status(void) {
	// Konfiguracja
	if(GPIO_not_configured) GPIO_init();
	return GPIO_ReadInputDataBit(IN1_PORT, IN1_PIN);
}
