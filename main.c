
#include <stdio.h>
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
#include "configuracoes.h"
#include "bitmaps.h"
#include "nokia.h"
#include "cartas.h"

int parte_do_jogo = 0;  //dividido em 3 setores: menu - 0 | jogo - 1 | regras - 2
int parte_das_regras = 0;

int aux_con;    //garante que os valores reais das variaveis sejam preservados
int botao_pressionado = 0;  //armazena o estado do botao
int bet;    //armazena os valores apostados pelos jogador
int stack_dealer = 1000;    //total de fichas do jogador
int stack_jogador = 1000;   //total de fichas do Dealer
uint16_t vet[4] = {0};  //vetor utilizado para auxiliar a escrita dos numeros em

int i,m,j;//    i = a variavel faz-tudo | m = limita o numero de casas ocupadas pelos numeros | j = posicao em x dos dados printados

int baralho[52]; //52 cartas do baralho
int mao_jogador[11] = {0}; //mao  do jogador
int mao_dealer[11] = {0}; //mao do Dealer
//variaveis abaixo fazem a movimentacao do cursor nos menus e afins
int posicao_menu = 1; // 1 | 2
int posicao_ganhou = 1; // 1 | 2
int posicao_perdeu = 1; // 1 | 2
int posicao_um_ou_onze = 1; // 1 | 2
int posicao_aposta = 1; // 1 | 2 | 3 | 4 | 5
int posicao_acoes = 1; // 1 | 2 | 3


//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE VALOR
//-------------------------------------------------------------------------------------------------------------------------
int valor(int carta){   //recebe a carta e retorna seu valor:  A - 1 | 10, J, Q, K - 10 | o restante das cartas vale o seu proprio numero
  if( carta == 11 || carta == 12 || carta == 13 || carta == 24 || carta == 25 || carta == 26 || carta == 37 || carta == 38 || carta == 39 || carta == 50 || carta == 51 || carta == 52){
    return 10;
  }else if(carta == 1 || carta == 14 || carta == 27 || carta == 40){    return 1;   }
  if(carta <= 10){  return carta;   }
  else if(carta <= 26){ return carta - 13;  }
  else if(carta <= 39){ return carta - 26;  }
  return carta - 39;
}

int valor_mao(int mao[11]){ //recebe a mao e retorna seu valor
  int soma = 0;
  for(i=0;i<11;i++){
    soma += valor(mao[i]);
  }
  return soma;
}

void dados_tela_padrao(void){   //printa na tela os seguintes dados : valor apostado | stacks dos jogadores | valores das maos dos jogadores
    //o quanto foi apostado
    aux_con = bet;
    converte_valores(aux_con);
    if(aux_con < 100){
        if(aux_con < 10){   m = 1;  }
        else{  m = 2;   }
    }else{  m = 3;}

    j = 45;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 26, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }

    //Stack do jogador
    aux_con = stack_jogador;
    converte_valores(aux_con);
    if(aux_con < 1000){
        if(aux_con < 100){
            if(aux_con < 10){   m = 1;  }
            else{  m = 2;   }
        }else{  m = 3;  }
    }else{  m = 4;  }

    j = 69;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 39, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }
    //Stack do Dealer
    aux_con = stack_dealer;
    converte_valores(aux_con);
    if(aux_con < 1000){
        if(aux_con < 100){
            if(aux_con < 10){   m = 1;  }
            else{  m = 2;   }
        }else{  m = 3;  }
    }else{  m = 4;  }

    j = 69;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 25, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }

    //Valor da mao do jogador
    aux_con = valor_mao(mao_jogador);
    converte_valores(aux_con);
    if(aux_con < 10){   m = 1;  }
    else{  m = 2;   }

    j = 58;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 39, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }

    //Valor da mao do Dealer
    aux_con = valor(mao_dealer[0]);
    converte_valores(aux_con);
    if(aux_con < 10){   m = 1;  }
    else{  m = 2;  }

    j = 58;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 25, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }

    Nokia5110_DisplayBuffer();
}
//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE ACAO
//-------------------------------------------------------------------------------------------------------------------------
void acoes(void){   //jogador escolhe se: para com as cartas que tem e encerra o turno - P | pede mais uma - N | dobra a aposta, pede mais uma e encerra o turno
    while(1){
        botao_pressionado = estado_do_botao();


        if(mao_jogador[2] == 0){//duas cartas cada
            // 1-STAND | 2-HIT | 3-DOUBLE
            if(botao_pressionado == 2){
                Nokia5110_PrintBMP2(1, 46, limpa_barra_acoes, 53, 1);
                Nokia5110_DisplayBuffer();
                if(posicao_acoes == 3){ SysCtlDelay(delay_jogo + 1000);    posicao_acoes = 1;  }
                else{   SysCtlDelay(delay_jogo + 1000);    posicao_acoes++;    }
            }
            SysCtlDelay(delay_jogo);
            if(posicao_acoes == 1){  //cursor em P
                Nokia5110_PrintBMP2(1, 46, barra_P, 9, 1);
                Nokia5110_DisplayBuffer();
                dados_tela_padrao();
                if(botao_pressionado == 1){ break;  }
            }
            SysCtlDelay(delay_jogo);
            if(posicao_acoes == 2){  //cursor em N
                Nokia5110_PrintBMP2(16, 46, barra_N, 10, 1);
                Nokia5110_DisplayBuffer();
                if(botao_pressionado == 1){
                    nova_carta(mao_jogador, baralho);
                    escreve_cartas_jogador(mao_jogador);
                    dados_tela_padrao();
                    if(valor_mao(mao_jogador) >= 21){  break;  }
                }
            }
            SysCtlDelay(delay_jogo);
            if(posicao_acoes == 3){  //cursor em D
                Nokia5110_PrintBMP2(32, 46, barra_D, 10, 1);
                Nokia5110_DisplayBuffer();
                if(botao_pressionado == 1){
                    bet = dobra();
                    nova_carta(mao_jogador, baralho);
                    escreve_cartas_jogador(mao_jogador);
                    dados_tela_padrao();
                    break;
                }
            }
        }
        else{// 1-STAND | 2-HIT

            if(botao_pressionado == 2){
                Nokia5110_PrintBMP2(1, 46, limpa_barra_acoes, 53, 1);
                Nokia5110_DisplayBuffer();
                if(posicao_acoes == 2){ SysCtlDelay(delay_jogo + 1000);    posicao_acoes = 1;  }
                else{   SysCtlDelay(delay_jogo + 1000);    posicao_acoes++;    }
            }
            SysCtlDelay(delay_jogo);
            if(posicao_acoes == 1){  //cursor em P
                Nokia5110_PrintBMP2(1, 46, barra_P, 9, 1);
                Nokia5110_DisplayBuffer();
                dados_tela_padrao();
                if(botao_pressionado == 1){ break;   }
            }
            SysCtlDelay(delay_jogo);
            if(posicao_acoes == 2){  //cursor em N
                Nokia5110_PrintBMP2(16, 46, barra_N, 10, 1);
                Nokia5110_DisplayBuffer();
                if(botao_pressionado == 1){
                    nova_carta(mao_jogador, baralho);
                    escreve_cartas_jogador(mao_jogador);
                    dados_tela_padrao();
                    if(valor_mao(mao_jogador) >= 21){  break;  }
                }
            }
        }
    }
}

int dobra(void){    //o jogador dobra a aposta inicial, pede uma ultima carta e encerra o turno. So pode fazer esta acao se so tem duas cartas
    stack_jogador -= bet;
    stack_dealer -= bet;
    return 2*bet;
}
//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE STACK
//-------------------------------------------------------------------------------------------------------------------------

void converte_valores(uint16_t quantidade){ //recebe uma variavel e passa seus numerais para posicoes de um vetor auxiliar
    memset(&vet, 0, sizeof(vet));   //garante que nao haja "sujeira" no vetor auxiliar

    int aux,aux2 = 1;
    aux = quantidade;
    while(aux){
        aux /= 10;
        aux2 *= 10;
    }

    i = 0;
    while(aux2 > 1){
        aux2 /= 10;
        vet[i] = quantidade / aux2;
        quantidade %= aux2;
        i++;
    }
}

int aposta(void){// 1-5 | 2-10 | 3-25 | 4-50 | 5-100    jogador escolher quanto sera apostado no turno
    while(1){
        botao_pressionado = estado_do_botao();

        Nokia5110_ClearBuffer();
        Nokia5110_PrintBMP2(0, 0, aposte_geral, 84, 48);

        aux_con = stack_jogador;
        converte_valores(aux_con);

        if(aux_con < 1000){
            if(aux_con < 100){
                if(aux_con < 10){   m = 1;  }
                else{  m = 2;  }
            }else{  m = 3;  }
        }else{  m = 4;  }

        j = 65;
        for(i = 0; i < m; i++){
            Nokia5110_PrintBMP2(j, 6, vetor_de_numeros[vet[i]], 4, 5);
            j += 4;
        }

        aux_con = stack_dealer;
        converte_valores(aux_con);

        if(aux_con < 1000){
            if(aux_con < 100){
                if(aux_con < 10){   m = 1;  }
                else{  m = 2;   }
            }else{  m = 3;  }
        }else{  m = 4;  }

        j = 65;
        for(i = 0; i < m; i++){
            Nokia5110_PrintBMP2(j, 13, vetor_de_numeros[vet[i]], 4, 5);
            j += 4;
        }
        Nokia5110_DisplayBuffer();

        if(botao_pressionado == 2){
            if(posicao_aposta == 5){    SysCtlDelay(delay_jogo + 1000);    posicao_aposta = 1; }
            else{       SysCtlDelay(delay_jogo + 1000);    posicao_aposta++;   }
        }
        SysCtlDelay(delay_jogo);
        if(posicao_aposta == 1){  //cursor em CINCO
            Nokia5110_PrintBMP2(3, 34, barra_cinco, 6, 2);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado == 1){
                stack_jogador -= 5;
                stack_dealer -= 5;
                bet = 5;
                return bet;
            }
        }
        SysCtlDelay(delay_jogo);
        if(posicao_aposta == 2){  //cursor em DEZ
            Nokia5110_PrintBMP2(13, 34, barra_dez, 11, 2);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado == 1){
                stack_jogador -= 10;
                stack_dealer -= 10;
                bet = 10;
                return bet;
            }
        }
        SysCtlDelay(delay_jogo);
        if(posicao_aposta == 3){  //cursor em VINTE E CINCO
            Nokia5110_PrintBMP2(28, 34, barra_vinte_cinco, 11, 2);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado == 1){
                stack_jogador -= 25;
                stack_dealer -= 25;
                bet = 25;
                return bet;
            }
        }
        SysCtlDelay(delay_jogo);
        if(posicao_aposta == 4){  //cursor em CINQUENTA
            Nokia5110_PrintBMP2(43, 34, barra_cinquenta, 11, 2);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado == 1){
                stack_jogador -= 50;
                stack_dealer -= 50;
                bet = 50;
                return bet;
            }
        }
        SysCtlDelay(delay_jogo);
        if(posicao_aposta == 5){  //cursor em CEM
            Nokia5110_PrintBMP2(58, 34, barra_cem, 16, 2);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado == 1){
                stack_jogador -= 100;
                stack_dealer -= 100;
                bet = 100;
                return bet;
            }
        }
    }
}

void economia(void){    //verifica se algum jogador atingiu o triplo de fichas do adversario, ou seja, ganhou o jogo
    if(stack_dealer >= (3*stack_jogador)){
        stack_jogador = 1000;  stack_dealer = 1000;
        perdeu();
    }
    else if(stack_jogador >= (3*stack_dealer)){
        stack_jogador = 1000;  stack_dealer = 1000;
        ganhou();
    }
}
//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE TURNO
//-------------------------------------------------------------------------------------------------------------------------
int novo_turno(void){   //rodada onde os jogadores executam suas acoes
    Nokia5110_ClearBuffer();
    nova_carta(mao_jogador, baralho);   nova_carta(mao_jogador, baralho);
    Nokia5110_PrintBMP2(0, 0, tela_padrao, 84, 48);
    escreve_cartas_jogador(mao_jogador);
    dados_tela_padrao();
    Nokia5110_DisplayBuffer();
    return 0;
}

void encerra_turno(void){   //analisa quem ganhou, faz a distribuicao das fichas apostadas e mostra uma mensagem de incentivo ao jogador
    Nokia5110_ClearBuffer();

    if(valor_mao(mao_jogador) > 21){
        if(valor_mao(mao_dealer) <= 21){

            stack_dealer += 2*bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, perdendo, 59, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }else{//empate

            stack_dealer += bet;
            stack_jogador += bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, empatando, 82, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }
    }
    else if(valor_mao(mao_dealer) > 21){
        if(valor_mao(mao_jogador) <= 21){

            stack_jogador += 2*bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, ganhando, 69, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }else{//empate

            stack_dealer += bet;
            stack_jogador += bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, empatando, 82, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }
    }
    else if(valor_mao(mao_jogador) == 21){
        if(valor_mao(mao_dealer) != 21){

            stack_jogador += 2*bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, ganhando, 69, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }else{//empate

            stack_dealer += bet;
            stack_jogador += bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, empatando, 82, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }
    }
    else if(valor_mao(mao_dealer) == 21){
        if(valor_mao(mao_jogador) != 21){

            stack_dealer += 2*bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, perdendo, 59, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }else{//empate

            stack_dealer += bet;
            stack_jogador += bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, empatando, 82, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }
    }
    else if(valor_mao(mao_jogador) < 21){
        if(valor_mao(mao_jogador) > valor_mao(mao_dealer)){

            stack_jogador += 2*bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, ganhando, 69, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }

        }else if(valor_mao(mao_jogador) < valor_mao(mao_dealer)){

            stack_dealer += 2*bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, perdendo, 59, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }else{//empate
            stack_dealer += bet;
            stack_jogador += bet;
            while(1){
                botao_pressionado = estado_do_botao();

                Nokia5110_PrintBMP2(1, 1, empatando, 82, 5);
                dados_encerra_turno();
                Nokia5110_DisplayBuffer();
                if(botao_pressionado != 0){ break;  }
            }
        }
    }
    memset(&mao_jogador, 0, sizeof(mao_jogador));   //limpa a mao do jogador
    memset(&mao_dealer, 0, sizeof(mao_dealer)); //limpa a mao do Dealer
}

void dados_encerra_turno(void){ //mostra os valores finais das maos dos jogadores e os stacks atualizados
    Nokia5110_PrintBMP2(1, 23, valores_maos, 16, 17);
    Nokia5110_PrintBMP2(41, 23, stacks, 16, 17);

    //VALOR DA MAO DO JOGADOR

    aux_con = valor_mao(mao_jogador);
    converte_valores(aux_con);

    if(aux_con < 10){   m = 1;  }
    else{  m = 2;   }

    j = 19;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 23, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }

    //VALOR DA MAO DO DEALER
    aux_con = valor_mao(mao_dealer);
    converte_valores(aux_con);

    if(aux_con < 10){   m = 1;  }
    else{  m = 2;   }

    j = 19;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 35, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }
    //STACK_JOGADOR
    aux_con = stack_jogador;
    converte_valores(aux_con);

    if(aux_con < 1000){
        if(aux_con < 100){
            if(aux_con < 10){   m = 1;  }
            else{  m = 2;   }
        }else{  m = 3;  }
    }else{  m = 4;  }

    j = 59;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 23, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }
    //STACK_DEALER
    aux_con = stack_dealer;
    converte_valores(aux_con);

    if(aux_con < 1000){
        if(aux_con < 100){
            if(aux_con < 10){   m = 1;  }
            else{  m = 2;   }
        }else{  m = 3;  }
    }else{  m = 4;  }

    j = 59;
    for(i = 0; i < m; i++){
        Nokia5110_PrintBMP2(j, 35, vetor_de_numeros[vet[i]], 4, 5);
        j += 4;
    }
}
//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE DEALER
//-------------------------------------------------------------------------------------------------------------------------
int IA_dealer(void){    //Dealer pede cartas ate 5 vezes ou ate que sua mao seja maior que 16
    i = 0;
    while(valor_mao(mao_dealer) < 17){
        if(i == 5){ return 0;  }
        else{
            nova_carta(mao_dealer, baralho);
            i++;
        }
    }
    return 0;
}
//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE MENU
//-------------------------------------------------------------------------------------------------------------------------
void menu(void){    //jogador escolhe se quer jogar ou ver as regras
    while(1){
        botao_pressionado = estado_do_botao();

        if(botao_pressionado == 2){
            if(posicao_menu == 2){    SysCtlDelay(delay_jogo);  posicao_menu = 1; }
            else{   SysCtlDelay(delay_jogo);    posicao_menu++;   }
        }
        if(posicao_menu == 1){  //cursor em jogar
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, menu_jogar, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado == 1){
                parte_do_jogo = 1;
                break;
            }
        }
        if(posicao_menu == 2){  //cursor em regras
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, menu_regras, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado == 1){
                parte_do_jogo = 2;
                break;
            }
        }
    }
}

void jogo(void){    //embaralha as cartas - distribui - jogadores apostam - jogam - verifica as fichas - ciclo se repete ate haver um campeao
    embaralha(baralho);
    IA_dealer();
    SysCtlDelay(delay_jogo);
    aposta();
    SysCtlDelay(delay_jogo);
    novo_turno();
    SysCtlDelay(delay_jogo);
    acoes();
    SysCtlDelay(delay_jogo);
    encerra_turno();
    SysCtlDelay(delay_jogo);
    economia();
    SysCtlDelay(delay_jogo);
}

void regras(void){  //explica o jogo e se algum botao for pressionado retorna ao menu
    while(1){
        botao_pressionado = estado_do_botao();
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 0){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_zero, 79, 29);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 1;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 1){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_um, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 2;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 2){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_dois, 84, 35);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 3;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 3){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_tres, 76, 23);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 4;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 4){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_quatro, 79, 41);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 5;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 5){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_cinco, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 6;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 6){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_seis, 79, 23);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 7;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 7){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_sete, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 8;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 8){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_oito, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 9;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 9){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_nove, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 10;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 10){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_dez, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 11;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 11){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_onze, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 12;
                break;
            }
        }
        SysCtlDelay(delay_jogo);
        if(parte_das_regras == 12){
            Nokia5110_ClearBuffer();
            Nokia5110_PrintBMP2(0, 0, regras_doze, 84, 48);
            Nokia5110_DisplayBuffer();
            if(botao_pressionado != 0){
                parte_das_regras = 0;
                parte_do_jogo = 0;
                SysCtlDelay(delay_jogo);
                break;
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE RESULTADO
//-------------------------------------------------------------------------------------------------------------------------
void ganhou(void){  //parabeniza o jogador vencedor e se algum botao for pressionado retorna ao menu
    while(1){
        botao_pressionado = estado_do_botao();

        Nokia5110_ClearBuffer();
        Nokia5110_PrintBMP2(0, 0, tela_ganhou, 84, 48);
        Nokia5110_DisplayBuffer();

        if(botao_pressionado == 1 ){
            parte_do_jogo = 0;
            break;
        }
    }
}

void perdeu(void){  //incentiva o jogador derrotado e se algum botao for pressionado retorna ao menu
    while(1){
        botao_pressionado = estado_do_botao();

        Nokia5110_ClearBuffer();
        Nokia5110_PrintBMP2(0, 0, tela_perdeu, 84, 48);
        Nokia5110_DisplayBuffer();

        if(botao_pressionado == 1 ){
            parte_do_jogo = 0;
            break;
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
//                                              MAIN
//-------------------------------------------------------------------------------------------------------------------------
int main(void){ //inicializa o Display, configura o Tiva e os botoes e comanda os setores do game

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //habilitando interrucoes no portal A

    Nokia5110_Init();//inicializa o modulo do Nokia 5110
    Nokia5110_Clear();

    configura();

    while(1){
        if(parte_do_jogo == 0){ menu(); }
        if(parte_do_jogo == 1){ jogo(); }
        if(parte_do_jogo == 2){ regras(); }
        SysCtlDelay(5000);
    }
}
