/*******************************************************************************************************************************//**
 *
 * @file		DR_I2C.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		4 de nov. de 2017
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/

#ifndef DR_I2C_H_
#define DR_I2C_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "DR_Tipos.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
//puertos , pines
#define		SDA1		0 , 0
#define		SCL1		0 , 1


/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

#define BUFSIZE			0x20
#define MAX_TIMEOUT		0x00FFFFFF

#define I2CMASTER		0x01
#define I2CSLAVE		0x02

#define	ADR_BYTE	0xA0
#define RD_BIT		0x01

#define I2C_IDLE              0
#define I2C_STARTED           1
#define I2C_RESTARTED         2
#define I2C_REPEATED_START    3
#define DATA_ACK              4
#define DATA_NACK             5
#define I2C_BUSY              6
#define I2C_NO_DATA           7
#define I2C_NACK_ON_ADDRESS   8
#define I2C_NACK_ON_DATA      9
#define I2C_ARBITRATION_LOST  10
#define I2C_TIME_OUT          11
#define I2C_OK                12

#define I2CSTATE_SLA_NACK 	  13
#define I2CSTATE_PENDING	  14
#define I2CSTATE_ARB_LOSS	  15


#define I2CONSET_I2EN		0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA			0x00000004
#define I2CONSET_SI			0x00000008
#define I2CONSET_STO		0x00000010
#define I2CONSET_STA		0x00000020

#define I2CONCLR_AAC		0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC		0x00000008
#define I2CONCLR_STAC		0x00000020
#define I2CONCLR_I2ENC		0x00000040

#define I2DAT_I2C			0x00000000  /* I2C Data Reg */
#define I2ADR_I2C			0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH			0x00000080  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL			0x00000080  /* I2C SCL Duty Cycle Low Reg */



//Registros de I2C_1

#define		I2C1			( (__RW uint32_t *) 	0x4005C000UL )

#define		I2C1CONSET			I2C1 [ 0 ]
#define		I2C1STAT			I2C1 [ 1 ]
#define		I2C1DAT				I2C1 [ 2 ]
#define		I2C1ADR0			I2C1 [ 3 ]
#define		I2C1SCLH			I2C1 [ 4 ]
#define		I2C1SCLL			I2C1 [ 5 ]
#define		I2C1CONCLR			I2C1 [ 6 ]
#define		I2C1MMCTRL			I2C1 [ 7 ]
#define		I2C1ADR1			I2C1 [ 8 ]
#define		I2C1ADR2			I2C1 [ 9 ]
#define		I2C1ADR3			I2C1 [ 10 ]
#define		I2C1DATA_ BUFFER	I2C1 [ 11 ]
#define		I2C1MASK0			I2C1 [ 12 ]
#define		I2C1MASK1			I2C1 [ 13 ]
#define		I2C1MASK2			I2C1 [ 14 ]
#define		I2C1MASK3			I2C1 [ 15 ]



/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/
// extern tipo nombreVariable;
extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint32_t I2CCmd, I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;
//uint8_t  j,i;

extern uint8_t flag_i2c_listo;
/******1*****************************************************************************************************************************
 *** PROTOTIPOS DE FUNCIONES GLOBALES
 **********************************************************************************************************************************/
void MdE_I2C1 (void);
void I2C1_Inicializacion ( void);

uint32_t I2CStart( void );
uint32_t I2CStop( void );
uint32_t I2CEngine( void );


void Init_Infotronic( void );

#endif /* DR_I2C_H_ */
