/**----------------------------------------------------------------------------
             \file Monitor.cpp
--                                                                           --
--              ECEN 5003 Mastering Embedded System Architecture             --
--                  Project 1 Module 3                                       --
--                Microcontroller Firmware                                   --
--                      Monitor.cpp                                            --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--               
--                
--  Designed by:  Tim Scherr
--  Revised by:  Malcolm McKellips and Erich Clever 
-- 
-- Version: 2.0
-- Date of current revision:  2016-09-29   
-- Target Microcontroller: Freescale MKL25ZVMT4 
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--               
-- 
   Functional Description: See below 
--
--      Copyright (c) 2015 Tim Scherr All rights reserved.
--
*/              

#include <stdio.h>
#include "shared.h"


 /**
 * @brief Function for outputting register 0 value 
 *
 */
__asm uint32_t returnReg0(void){
	
	BX	LR;
	
}

 /**
 * @brief Function for outputting register 1 value 
 *
 */
__asm uint32_t returnReg1(void){
	
	MOVS	r0,r1;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 2 value 
 *
 */
__asm uint32_t returnReg2(void){
	
	MOVS	r0,r2;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 3 value 
 *
 */ 
__asm uint32_t returnReg3(void){
	
	MOVS	r0,r3;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 4 value 
 *
 */
__asm uint32_t returnReg4(void){
	
	MOVS	r0,r4;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 5 value 
 *
 */ 
__asm uint32_t returnReg5(void){
	
	MOVS	r0,r5;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 6 value 
 *
 */ 
__asm uint32_t returnReg6(void){
	
	MOVS	r0,r6;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 7 value 
 *
 */
__asm uint32_t returnReg7(void){
	
	MOVS	r0,r7;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 8 value 
 *
 */
__asm uint32_t returnReg8(void){
	
	MOV	r0,r8;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 9 value 
 *
 */ 
__asm uint32_t returnReg9(void){
	
	MOV	r0,r9;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 10 value 
 *
 */
__asm uint32_t returnReg10(void){
	
	MOV	r0,r10;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 11 value 
 *
 */
__asm uint32_t returnReg11(void){
	
	MOV	r0,r11;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 12 value 
 *
 */ 
__asm uint32_t returnReg12(void){
	
	MOV	r0,r12;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 13 value 
 *
 */ 
__asm uint32_t returnReg13(void){
	
	MOV	r0,r13;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 14 value 
 *
 */ 
__asm uint32_t returnReg14(void){
	
	MOV	r0,r14;
	BX		LR;
	
}

 /**
 * @brief Function for outputting register 15 value 
 *
 */ 
__asm uint32_t returnReg15(void){
	
	MOV	r0,r15;
	BX		LR;
	
}

 /**
 * @brief Get a data word from the stack
 *
 * This function will read a single word from the stack into r0
 *
 * @param[in] stackOffset
 *  A 32 bit offset for how deep into the stack we want to read
 *
 * @param[in] stackBase
 *  The address defining the beginning of the stack. We cannot read values beyond this. 
 *
 * @return
 *  The data word found by adding the stack offset to the stack pointer
 * 
 */
__asm uint32_t loadOneFromStack(uint32_t stackOffset,uint32_t stackBase){
	
	ADD	r0, r0, SP //Store sum of offset from SP in r0
	
	CMP r0, r1
	
	BGE AtStackBase
	
	LDR	r0, [r0]	//Load contents of address pointed to by r0 in r0
	// MOVS r1, #0
	
	BX LR
	
AtStackBase	
	MOVS r0, #0
	BX	LR
}

 /**
 * @brief Load a word long section of memory
 *
 * This function will read a single word from memory into r0 
 *
 * @param[in] address
 *  A 32 bit address of memory to read from
 *
 * @return
 *  The data word pointed to by the input address
 * 
 */
__asm uint32_t loadFromMem(uint32_t address){
	
	LDR	r0, [r0]	//Load contents of address pointed to by r0 in r0
	
	BX LR
	
}

 /**
 * @brief Send and entire word via UART
 *
 * This function will separate a word into bytes so they can be output over uart with UART_direct_hex_put() 
 *
 * @param[in] myWord
 *  A 32 bit word to output over uart
 * 
 */
void cutUpNSendWord(uint32_t myWord){
	
	char myByte;

	
	for(int i = 3;i > -1;i--){
	
		myByte = (uint8_t)(myWord>>(i*8));
		UART_direct_hex_put(myByte);
	
	}
	UART_direct_msg_put("\r\n");
	
}

 /**
 * @brief Display the value in every register
 *
 * This function will display the data located in each processor register of the M0+
 *
 * 
 */
void displayRegisters(void){
	
				UART_direct_msg_put("\r\nReg 0: ");			
				cutUpNSendWord(returnReg0());
				UART_direct_msg_put("Reg 1: ");		
				cutUpNSendWord(returnReg1());
				UART_direct_msg_put("Reg 2: ");		
				cutUpNSendWord(returnReg1());
				UART_direct_msg_put("Reg 3: ");		
				cutUpNSendWord(returnReg3());
				UART_direct_msg_put("Reg 4: ");		
				cutUpNSendWord(returnReg4());
				UART_direct_msg_put("Reg 5: ");		
				cutUpNSendWord(returnReg5());
				UART_direct_msg_put("Reg 6: ");		
				cutUpNSendWord(returnReg6());
				UART_direct_msg_put("Reg 7: ");		
				cutUpNSendWord(returnReg7());
				UART_direct_msg_put("Reg 8: ");		
				cutUpNSendWord(returnReg8());
				UART_direct_msg_put("Reg 9: ");		
				cutUpNSendWord(returnReg9());
				UART_direct_msg_put("Reg 10: ");		
				cutUpNSendWord(returnReg10());
				UART_direct_msg_put("Reg 11: ");		
				cutUpNSendWord(returnReg11());
				UART_direct_msg_put("Reg 12: ");		
				cutUpNSendWord(returnReg12());
				UART_direct_msg_put("Reg 13: ");		
				cutUpNSendWord(returnReg13());
				UART_direct_msg_put("Reg 14: ");		
				cutUpNSendWord(returnReg14());
				UART_direct_msg_put("Reg 15: ");		
				cutUpNSendWord(returnReg15());
	
}

 /**
 * @brief Display the last 16 values on the stack in reverse chronological order
 *
 * The function will step into the stack printing the last 16 values in reverse chronological order by adding to the stack poitner and outputting over UART. 
 * 
 */

void	displayStackSixteen(void){
	
		UART_msg_put("\r\n");
		
		char stackPtrMsg[20];	
	
		for(int i=0;i<16;i++){
			sprintf(stackPtrMsg,"SP + %d: ",(i*4));
			UART_direct_msg_put(stackPtrMsg);
			cutUpNSendWord(loadOneFromStack(i*4,INITIAL_SP));
		
		}
}

 /**
 * @brief Output a section of memory over UART
 *
 * This function will read a word from memory and output it over UART
 *
 * @param[in] baseAddress
 *  A 32 bit address corresponding to the memory location to read.
 * 
 */
void	displayMemSect(uint32_t baseAddress){
	
	char displayMemMsg[20];
	sprintf(displayMemMsg,"\r\nContents of 0x%x: \r\n",baseAddress);
	UART_direct_msg_put(displayMemMsg);
	
	uint32_t data = loadFromMem(baseAddress);
	cutUpNSendWord(data);
	
}

/*******************************************************************************
* Set Display Mode Function
* Function determines the correct display mode.  The 3 display modes operate as 
*   follows:
*
*  NORMAL MODE       Outputs only mode and state information changes   
*                     and calculated outputs
*
*  QUIET MODE        No Outputs
*
*  DEBUG MODE        Outputs mode and state information, error counts,
*                    register displays, sensor states, and calculated output
*
*
* There is deliberate delay in switching between modes to allow the RS-232 cable 
* to be plugged into the header without causing problems. 
*******************************************************************************/

void set_display_mode(void)   
{
  UART_direct_msg_put("\r\nSelect Mode");
  UART_direct_msg_put("\r\n Hit NOR - Normal");
  UART_direct_msg_put("\r\n Hit QUI - Quiet");
  UART_direct_msg_put("\r\n Hit DEB - Debug" );
  UART_direct_msg_put("\r\n Hit V - Version#\r\n");
  UART_direct_msg_put("\r\n Hit S - Display the last 16 words from the Stack\r\n");
  UART_direct_msg_put("\r\n Hit R - Display the contents of all the registers\r\n");
	UART_direct_msg_put("\r\n Hit M then enter a 32-bit word-aligned HEX address - Display a word from memory\r\n");
  UART_direct_msg_put("\r\nSelect:  ");
  
}


 /**
 * @brief Check a message received over UART
 *
 * This function will check incoming UART data to see if it is of an acceptable formtat. It also allows for backspace editing
 * 
 */
void chk_UART_msg(void)    
{
   UCHAR j;
   while( UART_input() )      // becomes true only when a byte has been received
   {                                    // skip if no characters pending
      j = UART_get();                 // get next character

      if( j == '\r' )          // on a enter (return) key press
      {                // complete message (all messages end in carriage return)
         UART_msg_put("->");
         UART_msg_process();
      }
      else 
      {
         if ((j != 0x02) )         // if not ^B
         {                             // if not command, then   
            UART_put(j);              // echo the character   
         }
         else
         {
          ;
         }
         if( j == '\b' ) 
         {                             // backspace editor
            if( msg_buf_idx != 0) 
            {                       // if not 1st character then destructive 
               UART_msg_put(" \b");// backspace
               msg_buf_idx--;
            }
         }
         else if( msg_buf_idx >= MSG_BUF_SIZE )  
         {                                // check message length too large
            UART_msg_put("\r\nToo Long!");
            msg_buf_idx = 0;
         }
         else if ((display_mode == QUIET) && (msg_buf[0] != 0x02) && 
                  (msg_buf[0] != 'D') && (msg_buf[0] != 'N') && 
                  (msg_buf[0] != 'V') && (msg_buf[0] != 'M') && 
				  (msg_buf[0] != 'S') && (msg_buf[0] != 'R') &&
                  (msg_buf_idx != 0))
         {                          // if first character is bad in Quiet mode
            msg_buf_idx = 0;        // then start over
         }
         else {                        // not complete message, store character
 
            msg_buf[msg_buf_idx] = j;
            msg_buf_idx++;
            // if (msg_buf_idx > 2)
            // {
               // UART_msg_process();
            // }
			//Personalizing ;)
         }
      }
   }
}

 /**
 * @brief UART function for processing input
 *
 * This function will see if a recognizable command has been identified. It will check incoming UART messages for certain command strings that will set the display mode of the debug console.
 * 
 */
void UART_msg_process(void)
{
   UCHAR chr,err=0;
//   unsigned char  data;


   if( (chr = msg_buf[0]) <= 0x60 ) 
   {      // Upper Case
      switch( chr ) 
      {
         case 'D':
            if((msg_buf[1] == 'E') && (msg_buf[2] == 'B') && (msg_buf_idx == 3)) 
            {
               display_mode = DEBUG;
               UART_msg_put("\r\nMode=DEBUG\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'N':
            if((msg_buf[1] == 'O') && (msg_buf[2] == 'R') && (msg_buf_idx == 3)) 
            {
               display_mode = NORMAL;
               UART_msg_put("\r\nMode=NORMAL\n");
               //display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'Q':
            if((msg_buf[1] == 'U') && (msg_buf[2] == 'I') && (msg_buf_idx == 3)) 
            {
               display_mode = QUIET;
               UART_msg_put("\r\nMode=QUIET\n");
               display_timer = 0;
            }
            else
               err = 1;
            break;

         case 'V':
            display_mode = VERSION;
            UART_msg_put("\r\n");
            UART_msg_put( CODE_VERSION ); 
            UART_msg_put("\r\nSelect  ");
            display_timer = 0;
            break;
			
			
		case 'R':
            display_mode = REGISTERS;
            UART_msg_put("\r\n");
			
            displayRegisters();
			
            UART_msg_put("\r\nSelect  ");
            display_timer = 0;
            break;	
                
		case 'S':
            display_mode = STACK;
            UART_msg_put("\r\n");
			
            displayStackSixteen();
			
            UART_msg_put("\r\nSelect  ");
            display_timer = 0;
            break;
		case 'M':
			display_mode = MEMORY;
			UART_msg_put("\r\n");
			
			uint32_t addressDesired=0;
			
			for(int i = 1;i<9;i++){
				
				addressDesired += (((uint32_t)(asc_to_hex(msg_buf[i])))<<((8-i)*4));
//				UART_direct_msg_put("\r\nSelect  ");
			}
			
			if( (addressDesired%4)==0 ){
			
				displayMemSect(addressDesired);
			
				UART_direct_msg_put("\r\nSelect  ");   
  
			}
			else{
				UART_direct_msg_put("\r\n Address must be word-aligned (divisible by 4)\r\n");
			}
			display_timer = 0;
			break;
			
			default:
            err = 1;
			}  
		}

   else 
   {                                 // Lower Case
      switch( chr ) 
      {
        default:
         err = 1;
      }
   }

   if( err == 1 )
   {
      UART_msg_put("\n\rError!");
   }   
   else if( err == 2 )
   {
      UART_msg_put("\n\rNot in DEBUG Mode!");
   }   
   else
   {
    msg_buf_idx = 0;          // put index to start of buffer for next message
      ;
   }
   msg_buf_idx = 0;          // put index to start of buffer for next message


}


//*****************************************************************************
///   \fn   is_hex
/// Function takes 
///  @param a single ASCII character and returns 
///  @return 1 if hex digit, 0 otherwise.
///    
//*****************************************************************************
UCHAR is_hex(UCHAR c)
{
   if( (((c |= 0x20) >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'))  )
      return 1;
   return 0;
}

 /**
 * @brief DEBUG and DIAGNOSTIC Mode UART Operation
 *
 * This function will spew the outputs of the UART monitor. In debug mode it will print the flow, temp, and freq
 * 
 */
void monitor(void)
{

/**********************************/
/*     Spew outputs               */
/**********************************/

   switch(display_mode)
   {
      case(QUIET):
         {
             UART_msg_put("\r\n ");
             display_flag = 0;
         }  
         break;
      case(VERSION):
         {
             display_flag = 0;
         }  
         break;         
      case(NORMAL):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nNORMAL ");
               UART_msg_put(" Flow: ");
               // ECEN 5803 add code as indicated
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: ");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               display_flag = 0;
            }
         }  
         break;
      case(DEBUG):
         {
            if (display_flag == 1)
            {
               UART_msg_put("\r\nDEBUG \0");
               UART_msg_put(" Flow: \0");
               // ECEN 5803 add code as indicated               
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Temp: \0");
               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               UART_msg_put(" Freq: \0");

               //  add flow data output here, use UART_hex_put or similar for 
               // numbers
               
               
 /****************      ECEN 5803 add code as indicated   ***************/             
               //  Create a display of  error counts, sensor states, and
               //  ARM Registers R0-R15
               // clear flag to ISR      
               display_flag = 0;
             }   
         } 
         break;
		case(REGISTERS):
		{
			display_flag = 0;
		}
		 break;
		
		case(STACK):
		{
			display_flag = 0;
		}
		 break;
		
		case(MEMORY):
		{
			display_flag = 0;
		}
		 break;
      default:
      {
         UART_msg_put("Mode Error");
      }  
   }
}  
