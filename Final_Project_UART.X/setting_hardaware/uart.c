#include <xc.h>
    //setting TX/RX

char mystring[20];
int lenStr = 0;

void UART_Initialize() {
           
//           TODObasic   
//           Serial Setting      
//        1.   Setting Baud rate
//        2.   choose sync/async mode 
//        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
//        3.5  enable Tx, Rx Interrupt(optional)
//        4.   Enable Tx & RX
           
    TRISCbits.TRISC6 = 1; // white line         
    TRISCbits.TRISC7 = 1; // green line            
    
    // ---------- Setting baud rate: 1200 ----------
    TXSTAbits.SYNC = 0; // Async mode        
    BAUDCONbits.BRG16 = 0;          
    TXSTAbits.BRGH = 0;
    SPBRG = 51;      
    
    /*
     *  TXREG: store input data 
     *  TSR: if TSR is empty, then TXREG can write data to TSR
     */
    // ---------- Serial enable ----------
    // Step1. Transmitter
    TXSTAbits.TXEN = 1; // Transmit enabled: don't shut down!
    RCSTAbits.SPEN = 1; // Async serial port enabled (configures RX/DT and TX/CK(not as PORTC) pins as serial port pins): don't shut down!
    PIR1bits.TXIF = 1; // set when TXREG is empty(means: TXREG to TSR is finished)
    // Step2. Receiver
    PIR1bits.RCIF = 0; // The Rx buffer is empty(means: no input data); when RCIF = 1: 
    RCSTAbits.CREN = 1; // Asynchronous mode: 1 = Enables receiver: don't shut down!
    // Step3. interrput
    PIE1bits.TXIE = 0; // Disables the Tx interrupt: with TXIF
    IPR1bits.TXIP = 0; // Tx Interrupt Priority bit: 0 = Low priority
    PIE1bits.RCIE = 1; // Enables the Rx interrupt: with RCIF(means: if has input data, then trigger interrupt)
    IPR1bits.RCIP = 0; // Rx Interrupt Priority bit: 0 = Low priority
    }

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT); // if (TRMT == 0) -> TSR still send data -> can't let move TXREG to TSR -> wait
    TXREG = data; // if(TRMT==1) -> TSR is empty -> write to TXREG will send data 
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}

void ClearBuffer(){
    for(int i = 0; i < 10 ; i++) // limit: 10 char
        mystring[i] = '\0';
    lenStr = 0;
}

void MyusartRead()
{
    /* TODObasic: try to use UART_Write to finish this function */
    mystring[lenStr] = RCREG; // get data from RCREG
    if (mystring[lenStr] == '\r'){
        UART_Write('\n');
    }
    UART_Write(mystring[lenStr]); // write one char at each time
    lenStr++;
    lenStr %= 10; // limit: 10 chars
    return ;
}

char *GetString(){
    return mystring;
}


// void interrupt low_priority Lo_ISR(void)
void __interrupt(low_priority)  Lo_ISR(void)
{
    if(RCIF) // will set RCIF, when receive input data have done
    {
        if(RCSTAbits.OERR)
        {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        MyusartRead();
    }
    
   // process other interrupt sources here, if required
    return;
}   