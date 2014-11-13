//-----------------------------------------------------------------
// Name:	Coulston
// File:	lab5.h
// Date:	Fall 2014
// Purp:	Include file for the MSP430
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Page 76 : MSP430 Optimizing C/C++ Compiler v 4.3 User's Guide
//-----------------------------------------------------------------
typedef		unsigned char		int8;
typedef		unsigned short		int16;
typedef		unsigned long		int32;
typedef		unsigned long long	int64;

#define		TRUE				1
#define		FALSE				0

//-----------------------------------------------------------------
// Function prototypes found in lab5.c
//-----------------------------------------------------------------
void initMSP430();
__interrupt void pinChange (void);
__interrupt void timerOverflow (void);


//-----------------------------------------------------------------
// Each PxIES bit selects the interrupt edge for the corresponding I/O pin.
//	Bit = 0: The PxIFGx flag is set with a low-to-high transition
//	Bit = 1: The PxIFGx flag is set with a high-to-low transition
//-----------------------------------------------------------------

#define		IR_PIN			(P2IN & BIT6)
#define		HIGH_2_LOW		P2IES |= BIT6
#define		LOW_2_HIGH		P2IES &= ~BIT6


#define		averageLogic0Pulse	0x0212
#define		averageLogic1Pulse	0x0666
#define		averageStartPulse	0x1140
#define		minLogic0Pulse		averageLogic0Pulse - 0x100
#define		maxLogic0Pulse		averageLogic0Pulse + 0x100
#define		minLogic1Pulse		averageLogic1Pulse - 0x100
#define		maxLogic1Pulse		averageLogic1Pulse + 0x100
#define		minStartPulse		averageStartPulse - 0x100
#define		maxStartPulse		averageStartPulse + 0x100

#define		PWR		0x61A0F00E
#define		ONE		0x61A000FE
#define		TWO		0x61A0807E
#define		THR		0x61A040BE

#define		VOL_UP	0x61A030CE
#define		VOL_DW	0x61A0B04E
#define		CH_UP	0x61A050AE
#define		CH_DW	0x61A0D02E
