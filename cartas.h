
#ifndef CARTAS_H
#define CARTAS_H

//------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE CARTA
//------------------------------------------------------------------------------------------------------------------------
int embaralha(int pilha[52]);   //OK
const uint8_t *converte_carta(int k);   //OK
void posicao_carta(int c);  //OK
void escreve_cartas_jogador(int mao[11]);   //OK
int nova_carta(int mao[11], int baralho[52]);   //OK

#endif
