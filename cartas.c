
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <driverlib/timer.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "declaracoes.h"
#include <configuracoes.h>
#include "nokia.h"
#include "cartas.h"
#include "cartas_bmp.h"

int aux_X, aux_Y;   //coordenadas onde serao printadas as cartas do jogador
int k;  //variavel faz-tudo


//------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE CARTA
//------------------------------------------------------------------------------------------------------------------------
int embaralha(int pilha[52]){   //recebe o vetor baralho e embaralha seus termos
  int aux1,aux2;
  srand(time(NULL));  //seed randomica
  for(k = 0;k < 52;k++){
    pilha[k] = k + 1; //comeca em 1
  }
  for(k = 0;k < 52;k++){
    aux1 = rand() % 52; //entre 1 e 52
    aux2 = pilha[k];
    pilha[k] = pilha[aux1];
    pilha[aux1] = aux2;
  }
  return 0;
}

const uint8_t *converte_carta(int k){   //rece os inteiros do baralho e retorna a carta correspondente
    switch(k){

        //OUROS

        case 1:     return as_ouros;
        case 2:     return dois_ouros;
        case 3:     return tres_ouros;
        case 4:     return quatro_ouros;
        case 5:     return cinco_ouros;
        case 6:     return seis_ouros;
        case 7:     return sete_ouros;
        case 8:     return oito_ouros;
        case 9:     return nove_ouros;
        case 10:    return dez_ouros;
        case 11:    return valete_ouros;
        case 12:    return dama_ouros;
        case 13:    return rei_ouros;

        //ESPADAS

        case 14:    return as_espadas;
        case 15:    return dois_espadas;
        case 16:    return tres_espadas;
        case 17:    return quatro_espadas;
        case 18:    return cinco_espadas;
        case 19:    return seis_espadas;
        case 20:    return sete_espadas;
        case 21:    return oito_espadas;
        case 22:    return nove_espadas;
        case 23:    return dez_espadas;
        case 24:    return valete_espadas;
        case 25:    return dama_espadas;
        case 26:    return rei_espadas;

            //COPAS

        case 27:    return as_copas;
        case 28:    return dois_copas;
        case 29:    return tres_copas;
        case 30:    return quatro_copas;
        case 31:    return cinco_copas;
        case 32:    return seis_copas;
        case 33:    return sete_copas;
        case 34:    return oito_copas;
        case 35:    return nove_copas;
        case 36:    return dez_copas;
        case 37:    return valete_copas;
        case 38:    return dama_copas;
        case 39:    return rei_copas;

            //PAUS

        case 40:    return as_paus;
        case 41:    return dois_paus;
        case 42:    return tres_paus;
        case 43:    return quatro_paus;
        case 44:    return cinco_paus;
        case 45:    return seis_paus;
        case 46:    return sete_paus;
        case 47:    return oito_paus;
        case 48:    return nove_paus;
        case 49:    return dez_paus;
        case 50:    return valete_paus;
        case 51:    return dama_paus;
        case 52:    return rei_paus;
    }
    return 0;
}

void posicao_carta(int c){  //passa as posicoes de cada carta para as variaveis globais
    switch(c){

        case 0:
            aux_X = 13;
            aux_Y = 1;
            break;
        case 1:
            aux_X = 22;
            aux_Y = 1;
            break;
        case 2:
            aux_X = 31;
            aux_Y = 1;
            break;
        case 3:
            aux_X = 40;
            aux_Y = 1;
            break;
        case 4:
            aux_X = 49;
            aux_Y = 1;
            break;
        case 5:
            aux_X = 58;
            aux_Y = 1;
            break;
        case 6:
            aux_X = 67;
            aux_Y = 1;
            break;
        case 7:
            aux_X = 1;
            aux_Y = 18;
            break;
        case 8:
            aux_X = 10;
            aux_Y = 18;
            break;
        case 9:
            aux_X = 19;
            aux_Y = 18;
            break;
        case 10:
            aux_X = 28;
            aux_Y = 18;
            break;
    }
}

void escreve_cartas_jogador(int mao[11]){   //printa as cartas do jogador nos espacos predestinados
    int c = 0;
    while(mao[c] != 0){
        posicao_carta(c);
        k = mao[c];
        Nokia5110_PrintBMP2(aux_X, aux_Y,converte_carta(k), 9, 15);
        Nokia5110_DisplayBuffer();
        c++;
    }
}

int nova_carta(int mao[11], int baralho[52]){   //retira a carta de cima do baralho e passa para a mao do jogador
  int aux;
  for(k = 0;k < 52;k++){
    if(baralho[k] != 0){
      aux = baralho[k];
      baralho[k] = 0;
      break;
    }
  }
  for(k = 0;k < 11;k++){
    if(mao[k] == 0){
      mao[k] = aux;
      break;
    }
  }
  return 0;
}

