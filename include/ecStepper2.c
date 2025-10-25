#include "stm32f4xx.h"
#include "ecStepper2.h"

// 28BYJ-48 Stepper Motor Specifications
#define STEPS_PER_REV_FULL 32   // Motor internal steps per revolution in FULL mode
#define STEPS_PER_REV_HALF 64   // Motor internal steps per revolution in HALF mode
#define GEAR_RATIO 64           // Gear ratio 64:1

// Output shaft steps per revolution
#define STEPS_PER_REV_OUTPUT_FULL (STEPS_PER_REV_FULL * GEAR_RATIO)  // 2048
#define STEPS_PER_REV_OUTPUT_HALF (STEPS_PER_REV_HALF * GEAR_RATIO)  // 4096 

//State number 
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5
#define S6 6
#define S7 7


// Stepper Motor function
uint32_t direction = 1; 
uint32_t step_delay = 100; 
uint32_t step_per_rev = 64*32;
	 

// Stepper Motor variable
volatile Stepper_t myStepper; 


//FULL stepping sequence  - FSM
typedef struct {
  	uint32_t next[2];
	uint8_t out[4];

} State_full_t;

State_full_t FSM_full[4] = {  	// 1010 , 0110 , 0101 , 1001
 	{{S3,S1},{1,0,1,0}},		// S0: 1010 -> next: CCW=S3, CW=S1
	{{S0,S2},{0,1,1,0}},		// S1: 0110 -> next: CCW=S0, CW=S2
	{{S1,S3},{0,1,0,1}},		// S2: 0101 -> next: CCW=S1, CW=S3
	{{S2,S0},{1,0,0,1}}		// S3: 1001 -> next: CCW=S2, CW=S0
};

//HALF stepping sequence
typedef struct {
	uint32_t next[2];
	uint8_t out[4];
} State_half_t;

State_half_t FSM_half[8] = {	// 1000 , 1010 , 0010 , 0110 , 0100 , 0101, 0001, 1001
 	{{S1,S7},{1,0,0,1}},	
	{{S2,S0},{1,1,0,0}},
	{{S3,S1},{0,1,0,0}},
	{{S4,S2},{0,1,1,0}},
	{{S5,S3},{0,0,1,0}},
	{{S6,S4},{0,0,1,1}},
	{{S7,S5},{0,0,0,1}},
	{{S0,S6},{1,0,0,0}}
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
	 // YOUR CODE
};



void Stepper_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4){
	 
	//  GPIO Digital Out Initiation
	myStepper.pin1 = pinName1;
	myStepper.pin2 = pinName2;
	myStepper.pin3 = pinName3;
	myStepper.pin4 = pinName4;
	//  GPIO Digital Out Initiation
	// No pull-up Pull-down , Push-Pull, Fast	
	GPIO_init(myStepper.pin1, OUTPUT);
	GPIO_init(myStepper.pin2, OUTPUT);
	GPIO_init(myStepper.pin3, OUTPUT);
	GPIO_init(myStepper.pin4, OUTPUT);
	GPIO_pupd(myStepper.pin1, 0);
	GPIO_pupd(myStepper.pin2, 0);
	GPIO_pupd(myStepper.pin3, 0);
	GPIO_pupd(myStepper.pin4, 0);
	GPIO_otype(myStepper.pin1, 0);
	GPIO_otype(myStepper.pin2, 0);
	GPIO_otype(myStepper.pin3, 0);
	GPIO_otype(myStepper.pin4, 0);
	GPIO_ospeed(myStepper.pin1, 2);
	GPIO_ospeed(myStepper.pin2, 2);
	GPIO_ospeed(myStepper.pin3, 2);
	GPIO_ospeed(myStepper.pin4, 2);
		
}


void Stepper_pinOut (uint32_t state, uint32_t mode){	
   	if (mode == FULL){         // FULL mode
		GPIO_write(myStepper.pin1, FSM_full[state].out[0]);
		GPIO_write(myStepper.pin2, FSM_full[state].out[1]);
		GPIO_write(myStepper.pin3, FSM_full[state].out[2]);
		GPIO_write(myStepper.pin4, FSM_full[state].out[3]);
  		
	}	 
 	else if (mode == HALF){    // HALF mode
		GPIO_write(myStepper.pin1, FSM_half[state].out[0]);
		GPIO_write(myStepper.pin2, FSM_half[state].out[1]);
		GPIO_write(myStepper.pin3, FSM_half[state].out[2]);
		GPIO_write(myStepper.pin4, FSM_half[state].out[3]);
	}
}


void Stepper_setSpeed(long whatSpeed) {
    if (whatSpeed > 0) {
        // Store RPM, actual delay will be calculated in Stepper_step based on mode
        // Use FULL mode as default for step_delay calculation
        step_delay = 60000000UL / (whatSpeed * STEPS_PER_REV_OUTPUT_FULL);
    } else {
        step_delay = 0; // 0 RPM means stop
    }
}


void Stepper_step(uint32_t steps, uint32_t direction, uint32_t mode){
	 uint32_t state = 0;
	 myStepper._step_num = steps;
	 
	 // Calculate actual delay based on mode
	 uint32_t actual_delay;
	 if (mode == FULL) {
	 	actual_delay = step_delay;  // Already calculated for FULL mode
	 } else {  // HALF mode
	 	// HALF mode has 2x more steps per revolution, so delay should be half
	 	actual_delay = step_delay / 2;
	 }

	 for(; myStepper._step_num > 0; myStepper._step_num--){ // run for step size
		// Output first, then delay
		Stepper_pinOut(state, mode);
		delay_us(actual_delay);  // Use mode-specific delay
		
		// Update state for next iteration
	    	if (mode == FULL) 		 												
			state = FSM_full[state].next[direction];  // state = next state
		else if (mode == HALF) 
			state = FSM_half[state].next[direction];  // state = next state		
   	}
}


void Stepper_stop (void){ 
    	myStepper._step_num = 0;    
	// All pins(A,AN,B,BN) set as DigitalOut '0'
	GPIO_write(myStepper.pin1, 0);
	GPIO_write(myStepper.pin2, 0);
	GPIO_write(myStepper.pin3, 0);
	GPIO_write(myStepper.pin4, 0);
	
}

