# STM32-SERVO-API-pt

# Projeto: 

Esse projeto foi desenvolvido no contexto da disciplina “Programação de Sistemas Embarcados” ofertada para o curso de Engenharia Elétrica da UFMG. O objetivo desse projeto é desenvolver uma API para controlar um servo motor.

## Hardware: 

O servo motor selecionado é o “FS5106B” da fabricante FEETECH. Esse é um motor de topologia Brush que opera na faixa de tensão de 4.8V até 6V e possui liberdade para girar até 200°. 

![image](https://user-images.githubusercontent.com/60392063/126249093-1200486c-fb60-4a64-a13b-edd284227e6e.png)

Uma importante característica sobre esse atuador é o fato de que sua posição é controlada com base na variação da largura de pulso de um sinal PWM, de modo que diferentes larguras de pulso correspondem a diferentes posições do eixo do rotor. 

É conhecido com base no datasheet que a frequência de trabalho do servo é de 50Hz (periodo de 20ms) e que para posicionarmos o eixo na posição de um dos extremos temos que a largura do período é de 0.7ms e para o outro extremo a largura do período é de 2.3ms. 

Todas as informações usadas para o desenvolvimento desse projeto foram retiradas do datasheet disponibilizado pelo fabricante do componente. 



O microcontrolador utilizado nessa aplicação é o STM32H743ZIT6U que está presente na placa de desenvolvimento NUCLEO-H743ZI. O modelo pode ser visualizado na sequinte imagem

![image](https://user-images.githubusercontent.com/60392063/126248265-d7cf42ed-005d-430a-baad-bc41f601ade3.png)


## Consideração importante

É importante mencionar que o microcontrolador opera na tensão de 3.3V, logo é necessário um circuito que faça a interface entre o microcontrolador e o servo motor. Não entraremos na explicação sobre o funcionamento elétrico desse circuito nesse texto, trataremos apenas da implementação da API. 

## Periféricos empregados

Para essa aplicação usaremos a saída de PWM que permite controlar a posição do eixo do servo motor. Além disso usaremos os LEDs disponíveis para gerar um feedback visual sobre o modo de operação do controlador. 

A saída de PWM foi configurada para variar com pulsos de largura de 

# API

A API desenvolvida permite controlar o modo de operação do servo motor. O modo de operação pode variar de uma aplicação controlada por um potenciômetro ou controlada por um timer. 

## Variáveis: 
Variável responsável por armazenar o status de funcioanmento do servo motor
```
int status = 0;
```
Variável responsável por armazenar o valor da referencia de posição usada para definir a posição do servo motor
```
uint16_t reference = 0;
```
Variável responsável por armazenar a posição do eixo do servo motor para o modo de funcionamento com timer
```
int position = 0;
```

## Funções

Função responsável por verificar e retornar o status do pino que define o modo de controle do servo motor.
```
int check_mode(void)
{
	GPIO_PinState status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);// Verifica o nivel logico da porta de controle
	return((int) status);
}
```

Função responsável por retornar um feedback visual sobre qual o modo de operação do servo motor. 
```
void transmit_status(int status) 
{
	if(status == 1)//caso o modo de operacao seja o com base no valor de ADC acende o LED vermelho
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
	}
	else//caso o modo de operacao seja o com base no valor de ADC acende o LED verde
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
	}
}
```

Função responsável por verificar a posição atual do eixo do motor para o modo de operação com base no tempo
```
int check_position(int position) //verifica qual a posicao atual e atualiza com a nova posicao
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
```

Função responsável por fazer o mapeamento do valor resultado do ADC conectado ao potenciometro com o valor de referencia para posicionar o eixo do sevo motor
```
uint16_t map_reference(int ADC_Value) 
{
	uint16_t reference = (uint16_t) (1.68*ADC_Value); //aplica um ganho sobre o valor lido no ADC para mapear o valor na faixa de controle do potenciometro
	return (reference);
}
```

Função responsável por gerar o valor de referencia a ser enviado para definir a posição do eixo do servo motor
```
uint16_t reference_define(int status, ADC_HandleTypeDef *hadc1, int position)
{
	uint16_t reference = 0;
	if(status == 1) // caso o modo de operacao seja o com referencia no potenciometro, a entrada analogica é lida e o valor mapeado em uma faixa para controle do servo motor
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1);
		int ADC_Value = HAL_ADC_GetValue(&hadc1);
		reference = map_reference(ADC_Value);
		return(reference);
	}
	else // caso o modo de operacao seja o com base no timer o verifica-se qual a posicao atual e com base nisso define a posicao do eixo do servo motor 
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
```

Função responsável por definir o sinal de PWM que vai controlar o servo motor
```
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
```

## Código exemplo 
Para exemplificar o uso dessa aplicação foi desenvolvido um código exemplo que segue o seguinte fluxo:

![image](https://user-images.githubusercontent.com/60392063/126248060-efb1b393-a129-4ae3-a53c-f0126c9d0df9.png)

O código tem a rotina de implementar o controle da posição do eixo do servo motor. Sendo que esse controle pode ser feito de duas maneiras, de acordo com o nível lógico do pino de controle. Os modos de funcionamento são: acompanhar o valor analógico (no caso pode ser representado por um potenciômetro), ou alternar entre as posições de valor mínimo, médio e máximo com variação de 1 segundo. 

O projeto foi gerado usando o CubeIDE versão 1.5.0 e está disponível na pasta projeto. 
