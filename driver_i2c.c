/*******************************************************************************************************************************//**
 *
 * @file		DR_I2C.c
 * @brief		Descripcion del modulo
 * @date		4 de nov. de 2017
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "DR_I2C.h"
#include "DR_Tipos.h"
#include "DR_PLL.h"
#include "Interrupcion_NVIC.h"
#include "GPIO_FW.h"
#include "DR_pinsel.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/
volatile uint32_t I2CMasterState = I2C_IDLE;
volatile uint32_t I2CSlaveState = I2C_IDLE;

volatile uint32_t I2CCmd;
volatile uint32_t I2CMode;		// maestro o esclavo

volatile uint8_t I2CMasterBuffer[BUFSIZE];
volatile uint8_t I2CSlaveBuffer[BUFSIZE];
volatile uint32_t I2CCount = 0;
volatile uint32_t I2CReadLength;
volatile uint32_t I2CWriteLength;

volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
uint8_t flag_i2c_listo=0;

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/
/**
	\fn  Nombre de la Funcion
	\brief Descripcion
 	\author Ing. Marcelo Trujillo
 	\date 4 de nov. de 2017
 	\param [in] parametros de entrada
 	\param [out] parametros de salida
	\return tipo y descripcion de retorno
*/

void MdE_I2C1 (void)
{
	uint8_t StatValue;

	/* this handler deals with master read and master write only */
	StatValue = I2C1STAT;

	switch ( StatValue )
	{
		case 0x08:
            // A START condition has been transmitted. We now send the slave address and initialize
            // the write buffer (we always start with a write after START+SLA).
            WrIndex = 0;
            I2C1DAT = I2CMasterBuffer[WrIndex++];
            I2C1CONSET = I2CONSET_AA;
            I2C1CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
            I2CMasterState = I2CSTATE_PENDING;
		break;

		case 0x10:
			// A repeated START condition has been transmitted. Now a second, read, transaction follows so we
			// initialize the read buffer.
			RdIndex = 0;
			// Send SLA with R bit set,
			I2C1DAT = I2CMasterBuffer[WrIndex++];
			I2C1CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);		break;

        case 0x18:
            // SLA+W has been transmitted; ACK has been received. We now start writing bytes.
            I2C1DAT = I2CMasterBuffer[WrIndex++];
            I2C1CONCLR = I2CONCLR_SIC;
            break;

        case 0x20:
            // SLA+W has been transmitted; NOT ACK has been received. Send a stop condition to terminate the transaction
            // and signal I2CEngine the transaction is aborted.
            I2C1CONSET = I2CONSET_STO;
            I2C1CONCLR = I2CONCLR_SIC;
            I2CMasterState = I2CSTATE_SLA_NACK;
            break;

        case 0x28:
            // Data in I2DAT has been transmitted; ACK has been received. Continue sending more bytes as
            // long as there are bytes to send and after this check if a read transaction should follow.
            if ( WrIndex < I2CWriteLength )
            {
                // Keep writing as long as bytes avail
                I2C1DAT = I2CMasterBuffer[WrIndex++];
            }
            else
            {
                if ( I2CReadLength != 0 )
                {
                    // Send a Repeated START to initialize a read transaction  (handled in state 0x10)                                */
                    I2C1CONSET = I2CONSET_STA;    // Set Repeated-start flag
                }
                else
                {
                    I2CMasterState = DATA_ACK;
                    I2C1CONSET = I2CONSET_STO;      // Set Stop flag
                }
            }
            I2C1CONCLR = I2CONCLR_SIC;
            break;

        case 0x30:
            // Data byte in I2DAT has been transmitted; NOT ACK has been received. Send a STOP condition
            // to terminate the transaction and inform the I2CEngine that the transaction failed.
            I2C1CONSET = I2CONSET_STO;
            I2C1CONCLR = I2CONCLR_SIC;
            I2CMasterState = DATA_NACK;
            break;

        case 0x38:
            // Arbitration loss in SLA+R/W or Data bytes. This is a fatal condition, the transaction did not
            // complete due to external reasons (e.g. hardware system failure).  Inform the I2CEngine of this and
            // cancel the transaction (this is automatically done by the I2C hardware)
            I2CMasterState = I2CSTATE_ARB_LOSS;
            I2C1CONCLR = I2CONCLR_SIC;
            break;

        // Master Receiver states
        case 0x40:
            // SLA+R has been transmitted; ACK has been received. Initialize a read. Since a NOT ACK is sent
            // after reading the last byte, we need to prepare a NOT ACK in case we only read 1 byte.
            if ( I2CReadLength == 1 )
            {
                // last (and only) byte: send a NACK after data is received
                I2C1CONCLR = I2CONCLR_AAC;
            }
            else
            {
                // more bytes to follow: send an ACK after data is received
                I2C1CONSET = I2CONSET_AA;
            }
            I2C1CONCLR = I2CONCLR_SIC;
            break;

        case 0x48:
            // SLA+R has been transmitted; NOT ACK has been received. Send a stop condition to terminate
            // the transaction and signal I2CEngine the transaction is aborted.
            I2C1CONSET = I2CONSET_STO;
            I2C1CONCLR = I2CONCLR_SIC;
            I2CMasterState = I2CSTATE_SLA_NACK;
            break;

        case 0x50:
            // Data byte has been received; ACK has been returned. Read the byte and check for more bytes to read.
            // Send a NOT ACK after the last byte is received
            I2CSlaveBuffer[RdIndex++] = I2C1DAT;
            if ( RdIndex < (I2CReadLength-1) )
            {
                // l more byte to follow: send an ACK after data is received
                I2C1CONSET = I2CONSET_AA;
            }
            else
            {
                // last byte: send a NACK after data is received
                I2C1CONCLR = I2CONCLR_AAC;
            }
            I2C1CONCLR = I2CONCLR_SIC;
            break;

        case 0x58:
            // Data byte has been received; NOT ACK has been returned. This is the last byte to read.
            // Generate a STOP condition and flag the I2CEngine that the transaction is finished.
            I2CSlaveBuffer[RdIndex++] = I2C1DAT;
            I2CMasterState = I2C_OK;
            I2C1CONSET = I2CONSET_STO;    // Set Stop flag
            I2C1CONCLR = I2CONCLR_SIC;    // Clear SI flag
            break;

        default:
            I2C1CONCLR = I2CONCLR_SIC;
            break;
    }
    return;
}
/*******************************/

uint32_t I2CStart( void )
{
  uint32_t timeout = 0;

  /*--- Issue a start condition ---*/
  I2C1CONSET = I2CONSET_STA;	/* Set Start flag */

  while((I2CMasterState != I2CSTATE_PENDING) && (timeout < MAX_TIMEOUT))
  {
       timeout++;
  }

  return (timeout < MAX_TIMEOUT);
}


/*****************************************/

uint32_t I2CStop( void )
{
	uint32_t timeout = 0;

	I2C1CONSET = I2CONSET_STO;  /* Set Stop flag */
	I2C1CONCLR = I2CONCLR_SIC;  /* Clear SI flag */

	/*--- Wait for STOP detected ---*/
	while(( I2C1CONSET & I2CONSET_STO )&&(timeout < MAX_TIMEOUT))
	{
         timeout++;
    }
    return (timeout >= MAX_TIMEOUT);
}

/*************************************************/


void I2C1_Inicializacion ( void)
{
	//Enciendo el periferico
	PCONP |= ( 0x01 << PCI2C1 );

	//Configuro el  PCLK = CCLK/4
	PCLKSEL1 &= ~( 0x11 << PCLK_I2C1 );		//limpio los bits del registro
	PCLKSEL1 |= ( 0x01 << PCLK_I2C1 );		//PCLK = CCLK

//	//I2C_bit_frecuencia = PCLKI2C / ( I2C0SCLH + I2C0SCLL ) entonces  I2C_bit_frecuencia = 100 Khz
	I2C1SCLH = I2SCLH_SCLH;
	I2C1SCLL = I2SCLL_SCLL;

	//Configuro los pines en modo  I2C1
	SetPINSEL( SDA1 , 3 );
	SetPINSEL( SCL1 , 3 );

	//viene por defecto con resistencia de PULLUP se lo tengo que sacar y poner como OPEN COLECTOR
	SetPINMODE( SDA1 , 2 );		//Le saco las resistencia de PULLUP y PULLDOWN
	SetPINMODE( SCL1 , 2 );		//Le saco las resistencia de PULLUP y PULLDOWN

	SetPINMODE_OD( SDA1 , 1 );		//configurado como OPEN COLECTOR  (drenaje abierto)
	SetPINMODE_OD( SCL1 , 1 );		//configurado como OPEN COLECTOR  (drenaje abierto)


	//limpio los flags
	I2C1CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;

	//Activar Interrupcion
	ISER0 |= ( 0x01 << ISE_I2C1 );

	I2C1CONSET = I2CONSET_I2EN;
}

/***********************************/

uint32_t I2CEngine( void )
{
  I2CMasterState = I2C_IDLE;
  RdIndex = 0;
  WrIndex = 0;
  if ( I2CStart() != 1 )
  {
	I2CStop();
	return ( 0 );
  }

  // wait until the state is at a terminal state
  while (I2CMasterState < 0x100);

  return (I2CMasterState);
}


/*************************************/

void I2C1_IRQHandler(void)
{
	MdE_I2C1 ( );
}


void Init_Infotronic( void )
{
	//RGB
	//Inicializo como PWM
	SetPINSEL( 2 , 1 , 1 );
	SetPINSEL( 2 , 2 , 1 );
	SetPINSEL( 2 , 3 , 1 );

	//RELAY QUE NO SE APAGA
	//Inicializo como GPIO
	SetPINSEL( 0 , 27 , 0 );
	//lo pongo como salida
	SetDIR( 0 , 27 , 1 );
	//lo apago
	SetDIR( 0 , 27 , 0 );
}

