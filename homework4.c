#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";
    char *r;

    // TODO: Declare the variables that main uses to interact with your state machine.
    bool complete;

    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.
    eUSCI_UART_ConfigV1 uartConfig =
    {
        .selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .clockPrescalar = 19,
        .firstModReg = 8,
        .secondModReg = 0x55,
        .parity = EUSCI_A_UART_NO_PARITY,
        .msborLsbFirst = EUSCI_A_UART_LSB_FIRST,
        .numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT,
        .uartMode = EUSCI_A_UART_MODE,
        .overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
        .dataLength = EUSCI_A_UART_8_BIT_LEN
    } ;

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    // TODO: Initialize EUSCI_A0
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    // Enable EUSCI_A0
    UART_enableModule(EUSCI_A0_BASE);

    while(1)
    {
        // TCheck the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
            if(UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == 1)
                rChar = UART_receiveData(EUSCI_A0_BASE);
            else
                rChar = 0xFF;

        // If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.
            if(rChar != 0xFF)
            {
                while(UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == 0)
                {}
                UART_transmitData(EUSCI_A0_BASE,rChar);
                complete = charFSM(rChar); // true is returned if final input is correct to FSM
            }


        // If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
           if(complete)
           {

               for(r = response; *r != '\0'; r++)
               {
                   while(UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == 0)
                       {}
                   UART_transmitData(EUSCI_A0_BASE,*r);
               }
               complete = false;
           }
               //transmit the response

    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// FSM functionality: Start is a standbye state until the first character is correct. Each state
// is named to indicate the number received.  If 2 is entered at any point, return back to state 2
// if an incorrect character is received, go back to start to await the 2 correctly.
bool charFSM(char rChar)
{
    enum State {Start, State2, State5, State3, State4};
    static enum State curState = Start;
    bool finished = false;

    switch (curState)
    {
    case Start:
        if(rChar == '2')
            curState = State2;
        break;
    case State2:
        if(rChar == '2')
            curState = State2;
        else if(rChar == '5')
            curState = State5;
        else
            curState = Start;
        break;
    case State5:
        if(rChar == '2')
            curState = State2;
        else if(rChar == '3')
            curState = State3;
        else
            curState = Start;

        break;
    case State3:
        if(rChar == '2')
            curState = State2;
        else if(rChar == '4')
            {
            curState = State4;
            finished = true;
            }
        else
            curState = Start;
        break;
    case State4: //Unnecessary as a state, but included for completeness
        if(rChar == '2')
            curState = State2;
        else
            curState = Start;
        break;

    }




    return finished;
}
