/*
 * Data: 07/2021
 * Instituicao de ensino: Universidade Federal de Minas Gerais
 * Autor: Victor Freitas Borges
 * Versao: 1.0
 * Arquivos da API: servo.c, servo.h
 * Hardware: Essa API foi desenvolvida com o hardware NUCLEO-H743 da familia STM32;
 *
 * Esta API foi desenvolvida como trabalho da disciplina de Programação de Sistemas Embarcados da UFMG - Prof. Ricardo de Oliveira Duarte - Departamento de Engenharia Eletrônica.
 */

#include "servo.h"
#include "main.h"

int check_mode(void)
{
	GPIO_PinState status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
	return((int) status);
}

void transmit_status(int status)
{
	if(status == 1)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
	}
}

int check_position(int position)
{
	if(position == 0)
	{
		position = 1;
	}
	else if (position == 1)
	{
		position = 2;
	}
	else
	{
		position = 0;
	}
}

uint16_t map_reference(int ADC_Value)
{
	uint16_t reference = (uint16_t) (1.68*ADC_Value);
	return (reference);
}

uint16_t reference_define(int status, ADC_HandleTypeDef *hadc1, int position)
{
	uint16_t reference = 0;
	if(status == 1)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1);
		int ADC_Value = HAL_ADC_GetValue(&hadc1);
		reference = map_reference(ADC_Value);
		return(reference);
	}
	else
	{
		if(position == 0)
		{
			reference = 2100; //posicao minima
			HAL_Delay(1000);
			return(reference);
		}
		else if(position == 1)
		{
			reference = 4500; //posicao neutra
			HAL_Delay(1000);
			return(reference);
		}
		else
		{
			reference = 6900; //posicao maxima
			HAL_Delay(1000);
			return(reference);
		}
	}
}

void set_position(TIM_HandleTypeDef timer, uint32_t channel, uint16_t pulse)
{
		 HAL_TIM_PWM_Stop(&timer, channel); // encerra o PWM
		 TIM_OC_InitTypeDef sConfigOC;
		 timer.Init.Period = 60000; // configura o periodo
		 HAL_TIM_PWM_Init(&timer); // reinicializa o sinal
		 sConfigOC.OCMode = TIM_OCMODE_PWM1;
		 sConfigOC.Pulse = pulse; // define a largura do pulso
		 sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		 sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		 HAL_TIM_PWM_ConfigChannel(&timer, &sConfigOC, channel);
		 HAL_TIM_PWM_Start(&timer, channel); // envia o sinal de saida no pino configurado
}


