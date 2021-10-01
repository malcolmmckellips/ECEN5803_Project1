/************************************************************************//**
* \file main.c
* \brief LAB EXERCISE 5.2 - SQUARE ROOT APPROXIMATION
*
* Write an assembly code subroutine to approximate the square root of an 
* argument using the bisection method. All math is done with integers, so the 
* resulting square root will also be an integer
******************************************************************************
* GOOD LUCK!
 ****************************************************************************/

 #include "stdint.h"
 
 
 /** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
 #include "string.h"
 /** @endcond */
 
 /**
 * @brief 5863 Write a brief description of the function here
 *
 * Detailed description of the function after one line gap
 *
 * @note You can also add a note this way
 *
 * @param[in] 
 *  You can also start writing on a new line. 
 *  And continue on the next line. This is how you describe an input parameter
 *
 * @return
 *  This is how you describe the return value
 * 
 */
__asm unsigned int my_sqrt(unsigned int x){
	
//	done = 0
//	a = 0
//	b = square root of largest possible argument (e.g. ~216).
//	c = -1
//	do {
//		c_old <- c
//		c <- (a+b)/2
//		if (c*c == x) {
//			done = 1
//		} else if (c*c < x) {
//			a <- c
//		} else {
//			b <- c
//	}
//	} while (!done) && (c != c_old)
//	return c 
	
	//r0 <- x
	//r1 = done
	//r2 = a
	//r3 = b
	//r4 = c
	//r5 = c_old
	//r6 = c * c 
	
		MOVS	r1, #0;				// done = 0
		MOVS 	r2, #0;				// a = 0
		MOVS	r3, #0xFF; 			// r3 = 1
  	LSLS  r3, r3, #8;  // r3 = r3 << 16			(b = 2^16)
		ADDS  r3, r3, #0xFF
		MOVS r4, #0; 				//c = 0 
		SUBS r4, r4, #1; 		//c = -1
		
dowhile
			MOVS r5, r4;		 //		c_old <- c
			
			ADDS r4, r2, r3; // c = a + b
			ASRS r4, r4, #1; // c = (a + b) / 2
			
			MOVS r6, r4; 			// r6 = c
			MULS r6, r6, r6;  // r6 = c*c 
			CMP	 r6, r0;			// Comparing on (c^2 - x) 
			
			BNE notx; 		   // if (c*c != X) -> notdone 
			MOVS r1, #1;     // done = 1 
			B whilecond;	//don't need to check other if statements (if, else)
notx
			BGT cintob;			// if (c*c > x put c in b )
			
			MOVS r2, r4; 		// if (c * c < x put c into a )
			B whilecond; 
			
cintob
				MOVS r3, r4; // b <- c 
			
			//check while conditions 
whilecond 
				CMP r1, #0; // compare r1 == 0 
				BNE donewithloop ; // if (done != 0) -> end while loop 
			
				CMP r4, r5; //
				BEQ donewithloop;  //if (c == c_old) -> end while loop
			
				B	dowhile; 	//continue while loop if conditions are satisfied 
				
donewithloop
		
		//MOVS r3, #46341; // b = sqrt (2^31 - 1) //(or 2^16?) 
		//MOVS r4, #-1; 
		
		MOVS r0, r4; //load c into r0 for returning to C code		

		BX lr; //return
//}
}

/*----------------------------------------------------------------------------
 MAIN function
 *----------------------------------------------------------------------------*/
 /**
 * @brief Main function
 *
 * Detailed description of the main
 */
int main(void){

	volatile unsigned int r, j=0;
	unsigned int i;    
	
  r = my_sqrt(2);    
  r = my_sqrt(4);    
	r = my_sqrt(22); 	
	r = my_sqrt(121); 	
//	r = my_sqrt(4294836225); 	//Testing largest value
	
  for (i=0; i<10000; i++){
		r = my_sqrt(i);
    j+=r;
  }
	
	while(1)
		;
}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************/

