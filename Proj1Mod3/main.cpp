/**----------------------------------------------------------------------------
 
   \file main.cpp

--                                                                           --
--              ECEN 5803 Mastering Embedded System Architecture             --
--                  Project 1 Module 3                                       --
--                Microcontroller Firmware                                   --
--                      main.cpp                                            --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--               
--                
--  Designed by:  Tim Scherr
--  Revised by:  Erich Clever and Malcolm McKellips
-- 
-- Version: 2.1
-- Date of current revision:  2021-09-20  
-- Target Microcontroller: Freescale MKL25ZVMT4 
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--               
-- 
-- Functional Description:  Main code file generated by mbed, and then
--                           modified to implement a super loop bare metal OS.
--
--      Copyright (c) 2015, 2016 Tim Scherr  All rights reserved.
--
*/

#define MAIN
#include "shared.h"
#undef MAIN

extern volatile uint16_t SwTimerIsrCounter; 


Ticker tick;             //  Creates a timer interrupt using mbed methods
 /****************      ECEN 5803 add code as indicated   ***************/
DigitalOut greenLED(LED_GREEN);
DigitalOut redLED(LED_RED);
DigitalOut blueLED(LED_BLUE);                                                   

Serial pc(USBTX, USBRX);     
 
void flip()  
{                
    greenLED = !greenLED;
}
 
int main() 
{
/****************      ECEN 5803 add code as indicated   ***************/
tick.attach(&timer0,.0001);       //  Add code to call timer0 function every 100 uS

    pc.printf("Hello World!\n"); 
    uint32_t  count = 0;   
    
// initialize serial buffer pointers
   rx_in_ptr =  rx_buf; /* pointer to the receive in data */
   rx_out_ptr = rx_buf; /* pointer to the receive out data*/
   tx_in_ptr =  tx_buf; /* pointer to the transmit in data*/
   tx_out_ptr = tx_buf; /* pointer to the transmit out */
    
   
  // Print the initial banner
    pc.printf("\r\nHello World!\n\n\r");

    /****************      ECEN 5803 add code as indicated   ***************/
    // uncomment this section after adding monitor code.  
   /* send a message to the terminal  */                    
   
   UART_direct_msg_put("\r\nSystem Reset\r\nCode ver. ");
   UART_direct_msg_put( CODE_VERSION );
   UART_direct_msg_put("\r\n");
   UART_direct_msg_put( COPYRIGHT );
   UART_direct_msg_put("\r\n");

   set_display_mode();                                      
   
   

    while(1)       /// Cyclical Executive Loop
    {

        count++;               // counts the number of times through the loop
//     __enable_interrupts();
//     __clear_watchdog_timer();

   /****************      ECEN 5803 add code as indicated   ***************/
    // uncomment this section after adding monitor code. 
    
        serial();            // Polls the serial port
        chk_UART_msg();     // checks for a serial port message received
        monitor();           // Sends serial port output messages depending
                         //     on commands received and display mode

        if ((SwTimerIsrCounter & 0x1FFF) > 0x0FFF)
   
        {
            flip();  // Toggle Green LED
        }
   
    } 
       
}

