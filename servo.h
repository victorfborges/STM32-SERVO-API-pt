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
#include "main.h"

int check_mode(void);

void transmit_status(int);

int check_position();

uint16_t map_reference(int);

uint16_t reference_define(int, ADC_HandleTypeDef*, int);

void set_position(TIM_HandleTypeDef, uint32_t, uint16_t);
