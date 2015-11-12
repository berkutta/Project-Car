//////////////////////////////////
// Programm: Mainboard			//
// Name:	 Eric Suter			//
// Datum:	 8.11.2015			//
// Version:	 1.1				//
//////////////////////////////////

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "predefines.h"

ISR(INT7_vect){														// Wenn Daten vom Funkmodul empfangen worden sind

	PORTF = PORTF | 0x01;											// Interrupt LED einschalten
	decode();														// Daten von Funkmodul lesen und verarbeiten
	
	if ( transmit == 0)	{
		
		twi_transmit(MM, 0x01,m_data1);								// Daten f�r Servo senden
		twi_transmit(MM, 0x02,m_data2);								// Daten f�r Motor senden

	}
	
	PORTF = PORTF & 0xFE;											// Interrupt LED ausschalten
}

int main (void){

	DDRF = 0xFF;													// INIT	von Ports
	PORTF = 0x02;
	DDRE = 0x7F;
	PORTE = PORTE | 0x80;
	DDRC = 0xFF;
	PORTC = 0x00;
	
	EIMSK = 1<<INT7;												// Interrupt auf INT7 aktivieren
	EICRB = 1<<ISC71 | 1<<ISC70;									// Ausl�sen auf der Positiven Flanke
	sei();															// Interrupts global aktivieren
	
	spi_init();
	radio_init();													// Funkmodul und TWI initialisieren
	twi_init();
	
	while(1){
		
		if(transmit == 1){											// Wenn daten von fernbedienung verlangt werden
			
			int ldata = twi_receive(EM, 0x00);						// Akkustand lesen
			transmit_mode(0xFF, ldata);								// Akkustand �bertragen
			transmit_mode(0x0F, PORTC);								// Beleuchtungsstand �bertragen
			
			transmit = 0;											// in den Empfangsmodus wechseln
			
			receive_mode();
			
		}
		
	}
}

void spi_init(void){
	
	 /* Set MOSI and SCK output, all others input */
	 DDRB = (1<<DDRB2)|(1<<DDRB1)|(1<<SS);
	 /* Enable SPI, Master, set clock rate fck/16 */
	 SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	 PORTB = PORTB | 0x01;
	 
	 _delay_ms(10);
	
	
}

void decode(void){
	
	motordata = receive_data();										// daten lesen
	m_data1 = (motordata >> 8) & 0x00FF;							// entschl�sseln von daten
	m_data2 = motordata >> 24;										// entschl�sseln von daten
	
}

//////////////////////////////////////////
// Funktion: TWI Initialisieren
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void twi_init(void){
																
																	// Init von TWI EF
	twi_transmit(MM, 0x01,0x0000);									// Init von Motor
	twi_transmit(MM, 0x02,0x0000);									// Init von Motor
	twi_receive(EM, 0x01);											// Akkustand laden
	
}

//////////////////////////////////////////
// Funktion: TWI Daten senden
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void twi_transmit(char adress, char mode, int data){
	
																	// EF
	
}

//////////////////////////////////////////
// Funktion: TWI Daten anfordern
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int twi_receive(char r_adress, char r_mode){
	
	int accu = 0;													// define
	
																	// daten empfangen EF
	
	return(accu);													// Wert zur�ckgeben
}

//////////////////////////////////////////
// Funktion: Initialisiert das Funkmodul und SPI Schnittstelle.
// Name:	 Eric Suter, Severin Landolt		
// Datum:	 8.11.2015			
// Version:	 1.1				
//////////////////////////////////////////

void radio_init(void)	{

	_delay_ms(120);													// POR Delay
	SPI_tranceive(w_register(0b00000000));
	_delay_us(50);
	SPI_tranceive(0b0110010);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	_delay_us(1500);
	
	receive_mode();
	
}

//////////////////////////////////////////
// Funktion: �bert�gt SPI Daten und gibt empfangene Daten zur�ck.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int SPI_tranceive(int transmit){									// Sendet Daten per Spi

	SPDR = transmit;												// Starte �bertragung
	while(!(SPSR & (1<<SPIF)));										// Warten bis �bertragung beendet ist
	return SPDR;													// Empfangene Daten zur�ckgeben
	
}

//////////////////////////////////////////
// Funktion: Setzt das Funkmodul in den Empfangs modus.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void receive_mode(void){

	SPI_tranceive(w_register(0b00000000));
	SPI_tranceive(0b0110011);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	PORTE = PORTE | 0x40;											// CE high schalten

}

//////////////////////////////////////////
// Funktion: Liest empfangene daten aus Funkmodul.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

uint32_t receive_data(void){
	
	SPI_tranceive(0b01100001);										// In den lesemodus wechseln Muss vl. mit receive_mode(void); ersetzt werden
	
	SPI_tranceive(0xFF);											// Erstes Byte auslesen
	r_data1 = SPDR;
	
	if (r_data1 == 0x77){ transmit = 1; }							// Wenn gewisse daten kommen in den Sendemodus wechseln.
	
	SPI_tranceive(0xFF);											// Zweites Byte auslesen
	r_data2 = SPDR;
	
	SPI_tranceive(0xFF);											// Drites Byte auslesen
	r_data3 = SPDR;
	
	SPI_tranceive(0xFF);											// Viertes Byte auslesen
	r_data4 = SPDR;
		
	SPI_tranceive(0xFF);											// F�nftes Byte auslesen
	r_data5 = SPDR;
	
	uint32_t result = ((((((((r_data5<<8)+r_data4)<<8)+r_data3)<<8)+r_data2)<<8)+r_data1);	// Die ganzen Daten in einen Datentyp zum �bergeben wandeln
	
	for(int i = 29; i == 0; --i){									// Dummy Daten empfangen

		SPI_tranceive(0xFF);

	}
	
	return(result);
	
}

//////////////////////////////////////////
// Funktion: Setzt das Funkmodul in den �bertragungsmodus und sendet die daten
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void transmit_mode(char mode, int data){

	SPI_tranceive(0b10100000);										// FIFO Beschreiben

	SPI_tranceive(mode);											// Daten zum Senden geben
	SPI_tranceive(data | 0x00FF);
	SPI_tranceive((data | 0xFF00) >> 8);
	for(int i = 29; i == 0; --i){									// Dummy Daten senden

		SPI_tranceive(0xFF);

	}

	SPI_tranceive(w_register(0b00000000));
	SPI_tranceive(0b0110010);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	PORTE = PORTE | 0x40;											// CE high schalten

}
