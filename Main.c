#include <mega324a.h>
#include <delay.h>
#include <alcd.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

char* Ouvert = "<ID00><L1><PA><FA><MQ><WB><FA><CD><AB>PONT OUVERT4B<E>";
char* Prudence = "<ID00><L1><PA><FA><MQ><WB><FA><CH><AB>PRUDENCE73<E>";
char* Ferme = "<ID00><L1><PA><FA><MQ><WB><FA><CA><AB>PONT FERMEE5D<E>";

char* AT = "AT";
char* CMGF = "AT+CMGF=1";
char* PIN = "AT+CPIN=\"0000\"";
char* NUM = "AT+CMGS=\"+33782643608\"";

int Alert = 0;
int dist = 0;

void init_timer() {
    TCCR0A = 0; // Mode normal
    TCCR0B = 0; // Timer arrêté
    TCNT0 = 0; // Réinitialiser le compteur du Timer0
    TIFR0 |= 1; // Réinitialiser le drapeau de débordement
}

void InitRS232(void)
{
    UCSR0B=0x08;  
    UCSR0C=0x06;
    UBRR0H=0x00;
    UBRR0L=0x67;
    UCSR1B=0x08;  
    UCSR1C=0x06;
    UBRR1H=0x00;
    UBRR1L=0x67;
}

void init_hrsc(void) {
    DDRC.6 = 1;
    DDRC.7 = 0;
}



unsigned int mesure_distance() {
    unsigned long duration = 0;
    unsigned int distance;
    unsigned int overflow_count = 0;
   
    PORTC.6 = 0;
    delay_us(2);
    PORTC.6 = 1;
    delay_us(10);
    PORTC.6 = 0;
     
    while (PINC.7 == 0) {;}
   
    TCNT0 = 0;
    TCCR0B |= (1 << CS01);
   
    while (PINC.7) {
        if (TIFR0 & 1) { // Si TIFR0 (compteur) > à 255 il repasse a 0 et ajoute 1 a overflow count
            overflow_count++;
            TIFR0 |= 1;
        }
    }
   
    TCCR0B &= 0xFD;   
   
    duration = (overflow_count * 256) + TCNT0;
   
    distance = duration / 116;
   
    return distance;
}

void SendAff(char* str)
{
    while(*str != '\0'){
        delay_us(1100); 
        UDR1 = *str;
        str++;
    }
}

void SendGSM(char* str)
{
    while(*str != '\0'){
        delay_us(1100); 
        UDR0 = *str;
        str++;
    }
    UDR0 = '\x0D';
    delay_ms(1000);
}

void InitGSM(void)
{
    SendGSM(AT);
    SendGSM(PIN);
    delay_ms(3000);
    SendGSM(CMGF);
}

void SendSMS(char c)
{
    SendGSM(NUM);
    delay_ms(1000);
    UDR0 = c;
    delay_us(1100);
    UDR0 = '\x1A';
    delay_ms(1000);
}

void init (void) {
 InitRS232();
 init_hrsc();
 lcd_init(16);
 lcd_clear();
 init_timer();
 InitGSM();
 
}


void main (void) {
    init();
    while (1) {

        dist = mesure_distance();
       
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_printf("Distance: %d cm", dist);
       
        lcd_gotoxy(0,1);
        
        
        if (dist < 10) {
            if(Alert !=3){
                SendAff(Ferme);
                SendSMS('3');
            Alert = 3;
            } 
        } else if (dist < 20) {
            if(Alert !=2){
                SendAff(Prudence);
                SendSMS('2');
            Alert = 2;
            }
        } else if (dist < 32) {
            if(Alert !=1){
                SendAff(Ouvert);
                SendSMS('1');
            Alert = 1;
            }
        } else if (dist > 32) {
            if(Alert !=0){
                SendAff(Ouvert);
                SendSMS('0');
            Alert = 0;
            }
        }
       
        lcd_printf("Niveau : %d", Alert);
       
        delay_ms(500);
    }
}
