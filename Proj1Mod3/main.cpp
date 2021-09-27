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
DigitalOut greenLED(LED_GREEN,1);
DigitalOut redLED(LED_RED,1);
DigitalOut blueLED(LED_BLUE,1);

Serial pc(USBTX, USBRX);     
 
void flip()  
{                
    greenLED = !greenLED;
}

__asm uint32_t MyloadFromMem(uint32_t address){
	
	LDR	r0, [r0]	//Load contents of address pointed to by r0 in r0
	
	BX LR
	
}
 
int main() 
{
/****************      ECEN 5803 add code as indicated   ***************/
	uint32_t dummy = 0;
	
	dummy = MyloadFromMem(0x0000000D0);
	dummy = MyloadFromMem(0x000000180);
	dummy = MyloadFromMem(0x000000198);
	dummy = MyloadFromMem(0x20002FF0);
	
	pc.printf("\r\nDummy: %d\n\n\r",dummy);
	
	tick.attach(&timer0,.0001);       //  Add code to call timer0 function every 100 uS

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

		if(LED_heartbeatFlag)
		{
			
			redLED = !redLED;
			LED_heartbeatFlag=0;
			
		}

        if ((SwTimerIsrCounter & 0x1FFF) > 0x0FFF)
   
        {
            flip();  // Toggle Green LED
        }
   
    } 
       
}

