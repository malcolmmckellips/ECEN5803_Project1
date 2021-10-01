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


#include "mbed.h"

//defines for temperature calculation
#define V_TEMP25                (716U)      /*! Typical VTEMP25 in mV */
#define M                       (1620U)     /*! Typical slope: (mV x 1000)/oC */
#define STANDARD_TEMP           (25)


#define CHANNEL_0               (0U)
#define CHANNEL_1               (1U)
#define CHANNEL_2               (2U)


#define CH0_ADC_CH              (30u) //PTB0 -> A/d pin VREFL (internal ADC ch30)
#define CH1_ADC_CH              (9u)  //PTB1 -> J10_4, virtual vortex frequency input (ch9)
#define CH2_ADC_CH              (26u) //PTB2 -> Interal TempSensor(internal ADC ch 26)

//Bit posititions for CFG1 fields
#define CFG1_ADLPC              (7u)
#define CFG1_ADIV               (5u) //it is two bits so actuallly 6-5
#define CFG1_ADLSMP             (4u)
#define CFG1_MODE               (2u) //it is two bits so actually 3-2
#define CFG1_ADICLK            (0u) //it is two bits so actually 1-0
#define SC3_CONT_MODE           (3u)

//bit position for calibration fields 
#define SC3_AVGE_FIELD     (2u)
#define SC3_AVGS_FIELD     (0u)
#define SC2_TRG_FIELD      (6u)
#define SC3_CAL_FIELD      (7u) //calibration begin
#define SC3_CALF_FIELD     (6u) //calibration failure

//Configuration values
#define NORMAL_POWER_CONFIG (1u) //Actually equal to 0 so we have to clear this bit
#define CLOCK_RATIO_1       (3u) //Actually equal to 00 so we have to clear these bits
#define LONG_SAMPLE         (1u) //long sample = 1 so we want to set this bit
#define MODE_8bit           (3u) //8 bit mode is 00 so clear these bits 
#define MODE_16bit          (3u) //16 bit mode is 11 so set these bits 
#define BUS_CLK_IN          (3u) //Actually equal to 00 so we want to clear these bits 
#define CONT_MODE_EN        (1u) //Set to enable, clear to disable continuous conversions

//Calibration values
#define SC3_AVGE_EN        (1u) //set during calibration (SC3[AVGE]=1)
#define SC3_AVGS_32samples (3u) //Set during calibration (SC3[AVGS]=11)
#define SC2_TRG_EN         (1u) //Set to 1 for hardware, clear to 0 for software
#define S3_CAL_EN          (1u) //Set to 1 to start calibration, automatically cleared after calibration

// Initialize a pins to perform analog input and digital output fucntions
AnalogIn   vrefl(PTB0);         //PTB0 -> A/d pin VREFL (internal ADC ch30) 
AnalogIn   virtual_freq(PTB1);  //PTB1 -> J10_4, virtual vortex frequency input
AnalogIn   temp_sensor(PTB2);   //PTB2 -> Interal TempSensor(internal ADC ch 26) 


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

//Calibrate ADC 
uint16_t    calibrateADC(void){
    //ENABLE AND SET UP CLOCKS HERE 
    
    //Set up hardware config for config with "highest accuracy". 
    //Set up only clock source and frequency, low power config, voltage ref, sample time, and high speed config
    //The input channel, conversion mode cont, compare function, resolution mode, and differential/single-ended mode are all ignored

     ADC0->CFG1 &= ~(NORMAL_POWER_CONFIG << CFG1_ADLPC); //We want to be in normal power mode (0)
     ADC0->CFG1   |= (LONG_SAMPLE << CFG1_ADLSMP);  //Long samples
     ADC0->CFG1   &= ~(BUS_CLK_IN << CFG1_ADICLK);  //Input clock set to bus clock
     ADC0->CFG1   &= ~(CLOCK_RATIO_1 << CFG1_ADIV); //We want ADIV clk selecct to 00 for divide ratio of 1
    
    //"For best results set hardware avging to a maximum: SC3[AVGE]=1 and SC3[AVGS]=11"
    ADC0->SC3 |= (SC3_AVGE_EN << SC3_AVGE_FIELD);
    ADC0->SC3 |= (SC3_AVGS_32samples << SC3_AVGS_FIELD);
    
    //Might want to clear the config, but it says its ignored anyways...
    
    //Set ADC clock frequency <= 4 MHz and set Vrefh = Vdda ?????
    
    //To start calibration, clr ADCx_SC2[ADTRG] = 0 and set SC3[CAL] = 1
    ADC0->SC2 &= ~(SC2_TRG_EN << SC2_TRG_FIELD); //clear adc0 sc2 trg
    ADC0->SC3 |=  (S3_CAL_EN  << SC3_CAL_FIELD); //Set to start calibration
    
    //MIGHT HAVE TO WAIT FOR CALIBRATION TO FINISH HEREEEEEE!!! 
    
    //FROM RM pg. 495: 
    /*
    1. Initialize or clear a 16-bit variable in RAM.
    2. Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, and CLPS to
        the variable.
    3. Divide the variable by two.
    4. Set the MSB of the variable.
    5. The previous two steps can be achieved by setting the carry bit, rotating to the right
        through the carry bit on the high byte and again on the low byte.
    6. Store the value in the plus-side gain calibration register PG.
    7. Repeat the procedure for the minus-side gain calibration value.
    */
    
    // Initialize or clear a 16-bit variable in RAM.
    uint16_t PG_value = 0; //plus side gain register value
    
    //Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, and CLPS to vthe variable.
    PG_value = (ADC0->CLP0) + (ADC0->CLP1) + (ADC0->CLP2) + (ADC0->CLP3) + (ADC0->CLP4) + (ADC0->CLPS);
    PG_value = PG_value / 2; //Divide the variable by two.
    PG_value = PG_value |= 0x8000; //Set the MSB of the variable. could be 0xFF00 instead, Don't know if they mean most significant BYTE or BIT
    ADC0->PG = PG_value;//Store the value in the plus-side gain calibration register PG.
    
    //Repeat he procedure for the minus cide calibration value
    // Initialize or clear a 16-bit variable in RAM.
    uint16_t MG_value = 0; 
    
    //Add the plus-side calibration results CLM0, CLM1, CLP2, CLM3, CLM4, and CLMS to vthe variable.
    MG_value = (ADC0->CLM0) + (ADC0->CLM1) + (ADC0->CLM2) + (ADC0->CLM3) + (ADC0->CLM4) + (ADC0->CLMS);
    MG_value = MG_value / 2; //Divide the variable by two.
    MG_value = MG_value |= 0x8000; //Set the MSB of the variable. could be 0xFF00 instead, Don't know if they mean most significant BYTE or BIT
    ADC0->MG = MG_value;//Store the value in the minus-side gain calibration register MG.
    
    //MIGHT HAVE TO WAIT FOR CALIBRATION TO FINISH HEREEEEEE!!! 
    
    
    //return whether calibration failed or not with CALF
    uint16_t failure = 0; 
    
    if (ADC0->SC3 & (1<<SC3_CALF_FIELD) )
        failure = 1;
    
    return failure; 
}
	
	
uint16_t readADC(uint8_t input_ch){
    switch(input_ch){
        //channel 0 (PTB0 -> ch30) VREFL
        case CHANNEL_0: 
            //Configure for channel 0 
            ADC0->SC1[0] = CH0_ADC_CH; //ch30 is input
            ADC0->CFG1   &= ~(NORMAL_POWER_CONFIG << CFG1_ADLPC); //We want to be in normal power mode (0) 
            ADC0->CFG1   &= ~(CLOCK_RATIO_1 << CFG1_ADIV); //We want ADIV clk selecct to 00 for divide ratio of 1
            ADC0->CFG1   |= (LONG_SAMPLE << CFG1_ADLSMP);  //Long samples
            ADC0->CFG1   &= ~(MODE_8bit << CFG1_MODE);     //Mode 8 bit
            ADC0->CFG1   &= ~(BUS_CLK_IN << CFG1_ADICLK);  //Input clock set to bus clock 
            ADC0->SC3    &= ~(CONT_MODE_EN << SC3_CONT_MODE); //disable continuous mode
            
            //read the adc value from the AIN component and return it
            return vrefl.read_u16();

            //Do we need to change CFG2 at all? 
            //ADC0->CFG2   = ();
            
            //Read the 
        //channel 1 (PTB1 -> ch9) Virtual vortex frequency
        case CHANNEL_1:
            //configure for channel 1
            ADC0->SC1[0] = CH1_ADC_CH; //ch30 is input
            ADC0->CFG1   &= ~(NORMAL_POWER_CONFIG << CFG1_ADLPC); //We want to be in normal power mode (0) 
            ADC0->CFG1   &= ~(CLOCK_RATIO_1 << CFG1_ADIV); //We want ADIV clk selecct to 00 for divide ratio of 1
            ADC0->CFG1   |= (LONG_SAMPLE << CFG1_ADLSMP);  //Long samples
            ADC0->CFG1   |= (MODE_16bit << CFG1_MODE);     //Mode 16 bit
            ADC0->CFG1   &= ~(BUS_CLK_IN << CFG1_ADICLK);  //Input clock set to bus clock 
            ADC0->SC3    |= (CONT_MODE_EN << SC3_CONT_MODE); //enable continuous mode
            
            //read the adc value from the AIN component and return it
            return virtual_freq.read_u16();
            
        //channel 2 (PTB2 -> ch26) Temperature sensor
        case CHANNEL_2:
						SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
            ADC0->SC1[0] = CH2_ADC_CH; //ch26 is input
            ADC0->CFG1   &= ~(NORMAL_POWER_CONFIG << CFG1_ADLPC); //We want to be in normal power mode (0)  
            ADC0->CFG1   &= ~(CLOCK_RATIO_1 << CFG1_ADIV); //We want ADIV clk selecct to 00 for divide ratio of 1
            ADC0->CFG1   |= (LONG_SAMPLE << CFG1_ADLSMP);  //Long samples
            ADC0->CFG1   |= (MODE_16bit << CFG1_MODE);     //Mode 16 bit
            ADC0->CFG1   &= ~(BUS_CLK_IN << CFG1_ADICLK);  //Input clock set to bus clock 
            ADC0->SC3    |= (CONT_MODE_EN << SC3_CONT_MODE); //enable continuous mode
            
            return temp_sensor.read_u16();
            
            //HAVE TO SET SAMPLING RATE!!!
       default:
            return 0xFFFF; // if we got here, an invalid channel was passed in     
            
    }
}

//Variables for frequency detection
const uint16_t freq_data[1000] = {0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E,0x7FFF,0xF3D0,0xE29F,0x602A,0x0244,0x34C3,0xBDA9,0xFFBE,0xAF1E,0x2860,0x0643,0x6FF4,0xEC12,0xEC12,0x6FF4,0x0643,0x2860,0xAF1E,0xFFBE,0xBDA9,0x34C3,0x0244,0x602A,0xE29F,0xF3D0,0x7FFF,0x0C2E,0x1D5F,0x9FD4,0xFDBA,0xCB3B,0x4255,0x0040,0x50E0,0xD79E,0xF9BB,0x900A,0x13EC,0x13EC,0x900A,0xF9BB,0xD79E,0x50E0,0x0040,0x4255,0xCB3B,0xFDBA,0x9FD4,0x1D5F,0x0C2E};
const uint16_t pos_threshold = 50000;
const uint16_t neg_threshold = 20000;
volatile static uint16_t freq_index = 0; 
volatile static uint8_t  positive   = 0;	
volatile static uint16_t peaks = 0;
volatile static float flow_frequency = 0; 
	
uint16_t calculate_frequency(){
	uint16_t new_sample = freq_data[freq_index]; //eventually, this will be freq_data[freq_index] + noise (aka adcREAD(CHANNEL_1))
	
	//We are going from pos to neg (aka crossing neg threshold and having a negative peak)
	if ((positive) && (new_sample < neg_threshold)){
		positive = 0; 
		peaks++;
	}
	
	//We are going from neg to pos (aka crossing positive threshold and having a positive peak) 
	else if ((!positive) && (new_sample > pos_threshold)){
		positive = 1;
		peaks++; 
	}
	
	if (freq_index < 999)
		freq_index ++; 
	else{
		//We have iterated through the entire frequency array. Update the frequency variables and reset index
		if (peaks>0)
			flow_frequency = ((float(peaks)/2) / (0.1)); // peaks/2 will give average of number of pos peaks and neg peaks																	
																				// we divide by 0.1 because we took 1000 samples each 100uS apart, so (1000*1e-6) = 0.1
			freq_index = 0; 
			peaks = 0; 
			positive = 0; 
	}
	
	return flow_frequency;
}

float calculate_flow(uint16_t freq, float temp){
	float flow = 0;
	float PID  = 2.9; //inner pipe diameter = 2.9 inches
	float d 	 = 0.5; //bluff body width=d of 0.5 inches
	
	
	//unit conversions and flow eqns here 
	
	return flow;
}

int main() 
{
	
	//To test ADC: 
	
	calibrateADC();
	
	while (1) {
        //Calculate temperature from Fig. 28-63
				volatile float current_temp = 0;
				float adc_voltage = readADC(CHANNEL_2) * 3.3f / 65536; //convert from ADC reading to temperature
        current_temp = 25 - (adc_voltage - 716)/1620;
				current_temp = 0;
				/*
				volatile uint16_t adc_reading = 0;
				adc_reading = readADC(CHANNEL_2);
				adc_reading = 0;
				*/
   }
	
/****************      ECEN 5803 add code as indicated   ***************/
//	tick.attach(&timer0,.0001);       //  Add code to call timer0 function every 100 uS

//    uint32_t  count = 0;   
    
// initialize serial buffer pointers
//   rx_in_ptr =  rx_buf; /* pointer to the receive in data */
//   rx_out_ptr = rx_buf; /* pointer to the receive out data*/
//   tx_in_ptr =  tx_buf; /* pointer to the transmit in data*/
//   tx_out_ptr = tx_buf; /* pointer to the transmit out */
//    
//   
//  // Print the initial banner
//    pc.printf("\r\nHello World!\n\n\r");

//    /****************      ECEN 5803 add code as indicated   ***************/
//    // uncomment this section after adding monitor code.  
//   /* send a message to the terminal  */                    
//   
//   UART_direct_msg_put("\r\nSystem Reset\r\nCode ver. ");
//   UART_direct_msg_put( CODE_VERSION );
//   UART_direct_msg_put("\r\n");
//   UART_direct_msg_put( COPYRIGHT );
//   UART_direct_msg_put("\r\n");

//   set_display_mode();                                      
//   
//   uint16_t my_freq = 0;

//    while(1)       /// Cyclical Executive Loop
//    {

//        count++;               // counts the number of times through the loop
////     __enable_interrupts();
////     __clear_watchdog_timer();

//   /****************      ECEN 5803 add code as indicated   ***************/
//    // uncomment this section after adding monitor code. 
//    
//        serial();            // Polls the serial port
//        chk_UART_msg();     // checks for a serial port message received
//        monitor();           // Sends serial port output messages depending
//                         //     on commands received and display mode

//		if(LED_heartbeatFlag)
//		{
//			
//			redLED = !redLED;
//			LED_heartbeatFlag=0;
//			
//		}

//        if ((SwTimerIsrCounter & 0x1FFF) > 0x0FFF)
//   
//        {
//            flip();  // Toggle Green LED
//        }
//		}	
			
			//While 1 loop to test frequency detection algorithm. Result: success, my_freq = 1800 Hz
			/*
			while(1){
				volatile uint16_t my_freq = 0;
				for (int i = 0; i < 2000; i++) {
					my_freq = calculate_frequency();
				}

				my_freq = 0; //breakpoint here to peak at the frequency value in debugger...
			}
			*/	
			
			
}

