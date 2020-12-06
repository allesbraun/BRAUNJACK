
#include <configuracoes.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"


int press_SW1 = 0, press_SW2 = 0;   //representam os dois botoes

//linhas abaixo desbloqueiam o PF0
#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B

#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))


void IntPortalF (void){ //configura as interrupcoes nos botoes

    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);

    SysCtlDelay(delay_botao);

    //LEITURA DOS BOTOES
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00 && GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) != 0x00){
        press_SW1 = 1;
    }
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0x00 && GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) != 0x00){
        press_SW2 = 1;
    }

    GPIOIntClear(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);
}

void trata_botao_pressionado(int *botao){   //garante que o botao seja sempre 0 quando nao pressionado
    *botao = 0;
}

int estado_do_botao(void){  //se o SW1 for pressionado, retorna 1. Se o SW2 for pressionado, retorna 2
    SysCtlDelay(delay_botao);
    if(press_SW1 == 1 ){
        trata_botao_pressionado(&press_SW1);
        return 1;
    }

    if(press_SW2 == 1){
        trata_botao_pressionado(&press_SW2);
        return 2;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------------
//                                      FUNCOES DE CONFIGURACAO
//-------------------------------------------------------------------------------------------------------------------------
void configura(void){   //configura o Tiva

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // clock do tiva
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // habilitar o PORTAL GPIO

    ESC_REG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; //desbloquear PFO
    ESC_REG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x01; //endereço de PF0

    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_DIR_MODE_IN);//configurando SW1 e SW2 como entrada

    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);//SW1 e SW2 em pull-up

    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_4 | GPIO_INT_PIN_0 ,GPIO_FALLING_EDGE);//configurando como borda de descida

    IntEnable(INT_GPIOF);//habilitando interrucoes no portal F
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4 | GPIO_INT_PIN_0);//indicando quais os pinos usados
    IntMasterEnable();//enable global das interrupcoes
}
//-------------------------------------------------------------------------------------------------------------------------

