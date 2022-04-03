//Samuel Vinicius M. de Araújo
//120111386
// Sprint 10
#define F_CPU 16000000UL
#define  BAUD 9600
#define MYUBRR  F_CPU/16/BAUD-1
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "SSD1306.h"
#include "Font5x8.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>



//#define tam_vetor 16


uint32_t borda_subida, tempoDelta;
uint16_t tamanho_roda = 65, RPM = 0,b=0,temperatura=0,bateria=0, distancia = 0, tempo_ms = 0, distancia_novo =0, contador =0,velocidade = 0,pwm=0, pwm_bateria=0,pwm_temperatura=0, D=0, t=0,aux_ADC=0, raio=0,display1 = 0, display2 =0,display3 = 0, temperatura_e =0;
char string_eeprom[2];
uint32_t posicaox=0;
//unsigned char display_nokia[tam_vetor];



ISR(INT0_vect){ //O botão é pressionado aumenta  o diamentro do pneu
	tamanho_roda++;
	
	if (tamanho_roda > 80){
		
		tamanho_roda =80;
	}
	eeprom_write_byte(0,tamanho_roda);
}

ISR(INT1_vect){ //O botão é pressionado dimunui o diamentro do pneu

	tamanho_roda--;

	

	if (tamanho_roda < 40){
		
		tamanho_roda =40;
	}

	eeprom_write_byte(0,tamanho_roda);

	
}
ISR(TIMER0_COMPA_vect)
{
	static float  contador_aux;
	tempo_ms++;
	if((tempo_ms % 1000) == 0)
	{
		contador_aux = (contador/2); // dividindo por dois para reduzir a contagem em  descida ou  subida
		RPM = (contador_aux*60); // calculando rpm
		contador = 0;
		distancia_novo++;
		
	}
}

ISR(PCINT2_vect)
{
	contador++; // contar as descida e subidas
}
ISR(ADC_vect)
{
	
	
	if(b==0){ //PC0
		aux_ADC = ADC;
		if (ADC==1023)
		{
			pwm = 255;
		}
		else
		{
			pwm = (ADC/4);
		}
		
		if(tempoDelta < 300 && velocidade>20){
			
			pwm=80;
		}
		if (tempoDelta < 350 )
		{
			PORTD |= 0b00100000;
		}
		else
		{
			PORTD &= 0b11011111;
		}
		
		
	}
	
	
	if(b==1){ //PC1
		
		if (ADC==1023)
		{
			pwm_bateria = 255;
			bateria=100;
		}
		else
		{
			pwm_bateria= (ADC/4);
			bateria=(pwm_bateria*100)/255;
		}
		
	}



	if(b==2){ //PC2
		
		if (ADC==1023)
		{
			pwm_temperatura = 255;
			temperatura=200;
		}
		else
		{
			pwm_temperatura= (ADC/4);
			temperatura= (pwm_temperatura-105)/0.35;
		}
		
	}
	if(b==3){ //PC3
	
		if (ADC==1023)
		{
			posicaox = 255;
		
		}
		else
		{
			posicaox= (ADC/4);
		
		}
	
	}
	}









ISR(TIMER1_CAPT_vect){
	
	if(TCCR1B &(1<<ICES1))
	borda_subida=ICR1; //SALVA A PRIMEIRA CONTAGEM DE SUBIDA
	else //lê a contagem do TC1 na descida
	tempoDelta=((ICR1-borda_subida)*16)/58;
	TCCR1B^=(1<<ICES1);
	
	 
	
	
	

	
}
void USART_Init(unsigned int ubrr){
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L=(unsigned char)ubrr;
	UCSR0B =  (1<<RXCIE0) | ( 1<<RXEN0) | (1<<TXEN0);
	UCSR0C =(1<<USBS0)|(3<<UCSZ00);

}
void USART_Transmit(unsigned char data){
	
	while(!( UCSR0A & (1<< UDRE0)));
	UDR0 = data;
}
unsigned char USART_Receive (void){
	
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
	
	
}
ISR(USART_RX_vect){
	
	char recebido;
	recebido = UDR0;
	int imprimir =0;
	
	sprintf(string_eeprom,"%u",temperatura_e);
	if(recebido=='l'){
		//SART_Transmit(recebido);
		eeprom_write_byte(3,0);
	}
	
	if(recebido=='d')
	{
		//SART_Transmit(recebido);
		if (temperatura_e < 10)
		{
			imprimir=1;
		}
		else if (temperatura_e<100){
			imprimir=2;
		}
		else {
			imprimir=3;
		}
		
		for (int i=0;i<imprimir;i++)
		{
			USART_Transmit(string_eeprom[i]);
		}
		
	}
	
}
int main(void)

{
	
	DDRB = 0b11111110; //Habilita os pinos PB0...7 como saídas
	PORTB = 0b00000001;
	DDRC = 0b11110000; // Habilita os pino PC0..7 como saída
	PORTC = 0b10111000;
	DDRD  = 0b10100001; // Habilitando pinos de entrada
	PORTD = 0b01011101;
	

	
	
	
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000011;
	OCR0A = 249;
	TIMSK0 = 0b00000010;
	TCCR1B= (1<<ICES1)|(1<<CS12);
	TIMSK1 = 1<<ICIE1;
	
	EICRA = 0b00001010; //interrupção INT0 e INT1 -borda de descida
	
	EIMSK = 0b00000011; //habilita a interrupção
	
	
	
	PCICR = 0b00000100;
	PCMSK2= 0b00010000;
	
	ADMUX = 0b11000000;
	ADCSRA = 0b11101111;
	ADCSRB = 0x00;
	DIDR0 = 0b00111000;
	TCCR0A = 0b10100011;
	TCCR0B = 0b00000011;
	USART_Init(MYUBRR);
	sei();//Habilita interrupções globais
	GLCD_Setup();
	GLCD_SetFont(Font5x8,5,8, GLCD_Overwrite);
	GLCD_InvertScreen();
	
	
	
	
	while (1) {
		//calculando velocidade e distancia
		raio=tamanho_roda/2;
		
		velocidade=2*3.14*(raio)/100000*RPM*60;
		distancia = distancia_novo*velocidade/3600;
		

		
		
		OCR0A = pwm;
		tamanho_roda= eeprom_read_byte(0);
		
		
		
		
		
		
		

		
		GLCD_Clear();
		
		ADMUX = 0b11000000;
		b=0;
		_delay_ms(100);
		
		
		ADMUX = 0b11000001;
		b=1;
		_delay_ms(100);
		
		GLCD_GotoXY(120,0);
		GLCD_PrintString("%");
		GLCD_GotoXY(100,0);
		GLCD_PrintInteger(bateria);
		
		
		
		ADMUX = 0b11000010;
		b=2;
		_delay_ms(100);
		
		GLCD_GotoXY(120,15);
		GLCD_PrintString("C");
		GLCD_GotoXY(100,15);
		GLCD_PrintInteger(temperatura);
		temperatura_e = eeprom_read_byte(3);
		if (temperatura>temperatura_e)
		{
			eeprom_write_byte(3,temperatura);
			
		}
		
		ADMUX = 0b01000011;
		b=3;
		_delay_ms(100);
		
		
		GLCD_GotoXY(100,45);
		GLCD_PrintInteger(posicaox);
		
		
		
		if (posicaox > 127 )
		{
			PORTD |= 0b10000000;
		}
		else{
			PORTD &= 0b01111111;
		}
		
		
		if (posicaox < 127 )
		{
			PORTC |= 0b01000000;
		}
		else{
			PORTC &= 0b10111111;
		}
		
		
		GLCD_GotoXY(0,0);
		GLCD_PrintString("sonar:");
		GLCD_GotoXY(40,0);
		GLCD_PrintInteger(tempoDelta);
		
		GLCD_GotoXY(0,15);
		GLCD_PrintString("diamentro:");
		GLCD_GotoXY(60,15);
		GLCD_PrintInteger(tamanho_roda);
		
		GLCD_GotoXY(0,30);
		GLCD_PrintString("rpm:");
		GLCD_GotoXY(30,30);
		GLCD_PrintInteger(RPM);
		
		GLCD_GotoXY(0,45);
		GLCD_PrintString("Distancia:");
		GLCD_GotoXY(70,45);
		GLCD_PrintInteger(distancia);
		
		GLCD_Render();
		
		
		
		//exibindo a velocidade no display
		display1 = ((((velocidade/1)%10)*2) & 0b00011110);
		PORTB &= 0b00000001;
		PORTB |= 0b11000000;
		PORTB |= display1;
		_delay_ms(1);
		display2=  ((((velocidade/10)%10)*2) & 0b00011110);
		PORTB &= 0b00000001;
		PORTB |= 0b10100000;
		PORTB |= display2;
		_delay_ms(1);
		display3 =  ((((velocidade/100)%10)*2) & 0b00011110);
		PORTB &= 0b00000001;
		PORTB |= 0b01100000;
		PORTB |= display3;
		_delay_ms(1);
		
	}
}


