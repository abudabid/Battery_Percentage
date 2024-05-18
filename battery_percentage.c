#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// initialize the ADC 
void adc_init(void) {
    ADMUX = (1 << REFS0); // Reference voltage set to AVcc (5V) using the REFS0 port
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, set prescaler to 128
}

// read the ADC value from a given channel from 0-8
uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    return ADC;
}

// Initialize UART
void uart_init(unsigned int baud) {
    unsigned int ubrr = F_CPU/16/baud-1;
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Set frame: 8 data bits, 1 stop bit
}

// Transmit single character via UART
void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = data; // Put data into buffer, sends the data
}

// Send a string via UART
void uart_send_string(const char *str) {
    while (*str) {
        uart_transmit(*str++);
    }
}


int main(void) {
    adc_init(); // Initialize ADC
    uart_init(9600); // Initialize UART with 9600 baud rate
    
    char buffer[50];
    uint16_t adc_value;
    uint16_t battery_percentage;

    while (1) {
        adc_value = adc_read(0); // Read from ADC channel 0
		//max adc value : 860	4.2V  
		//min adc value : 614 	3.0V 
        battery_percentage = (uint16_t)(100 * (adc_value-614)/(860-614)); // Convert adc_value to battery percentage
        
        // Format the battery percentage into a string with newline characters
        snprintf(buffer, sizeof(buffer), "%u%%\r\n", battery_percentage);
        uart_send_string(buffer); // Send the string via UART

        _delay_ms(1000);
    }
    return 0;
}
