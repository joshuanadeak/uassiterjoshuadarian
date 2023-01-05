/****************************************************************************/
/*                                                                          */
/* Darian Texanditama dan Joshua Mihai Daniel Nadeak Project Akhir Semester */
/*                                                                          */
/****************************************************************************/

/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <ioport.h>
#include <board.h>
#include <string.h>

// defining for USART
#define USART_SERIAL_EXAMPLE             &USARTC0
#define USART_SERIAL_EXAMPLE_BAUDRATE    9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false


// ---- TEMPLATE SET UP FOR USART ----
static char strbuf[201];
static char reads[100];
int result = 0;
char in = 'x';

char *str1 = "atas ";
char *str2 = "bawah ";

void setUpSerial()
{
	// Baud rate selection
	// BSEL = (2000000 / (2^0 * 16*9600) -1 = 12.0208... ~ 12 -> BSCALE = 0
	// FBAUD = ( (2000000)/(2^0*16(12+1)) = 9615.384 -> mendekati lah ya
	
	USARTC0_BAUDCTRLB = 0; //memastikan BSCALE = 0
	USARTC0_BAUDCTRLA = 0x0C; // 12
	
	//USARTC0_BAUDCTRLB = 0; //Just to be sure that BSCALE is 0
	//USARTC0_BAUDCTRLA = 0xCF; // 207
	
	//Disable interrupts, just for safety
	USARTC0_CTRLA = 0;
	//8 data bits, no parity and 1 stop bit
	USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;
	
	//Enable receive and transmit
	USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

void sendChar(char c)
{
	
	while( !(USARTC0_STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	
	USARTC0_DATA = c;
	
}

void sendString(char *text)
{
	while(*text)
	{
		//sendChar(*text++);
		usart_putchar(USART_SERIAL_EXAMPLE, *text++);
	}
}

char receiveChar()
{
	while( !(USARTC0_STATUS & USART_RXCIF_bm) ); //Wait until receive finish
	return USARTC0_DATA;
}

void receiveString()
{
	int i = 0;
	while(1){
		//char inp = receiveChar();
		char inp = usart_getchar(USART_SERIAL_EXAMPLE);
		if(inp=='\n') break;
		else reads[i++] = inp;
	}
	if(strcmp(str1,reads) == 0){
		gpio_set_pin_high(J2_PIN0);
		}else if(strcmp(str2,reads) == 0){
		gpio_set_pin_high(J2_PIN0);
		}else{
		gpio_set_pin_low(J2_PIN0);
	}
}
// ---- TEMPLATE SET UP FOR USART ----

void setup_timer(void);
void print_message(void);

int score = 0;
int phase = 0;
int incremental = 0;
int distance = 0;
static char buffarray[200];

//Fungsi setup timer
void setup_timer(void){
	tc_enable(&TCC0);
	tc_set_overflow_interrupt_callback(&TCC0,print_message);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 58);
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_HI);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
}

//Fungsi ini bukan utk print message, tapi increment nilai variabel "increment" setiap 29us
void print_message(void){
	incremental = incremental + 1;
}

int main (void)
{
	//init board
	board_init();
	
	// Insert system clock initialization code here (sysclk_init()).
	sysclk_init();
	pmic_init();
	
	//init lcd
	gfx_mono_init();
	
	//set background lcd on
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);

	// Workaround for known issue: Enable RTC32 sysclk
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	while (RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm) {
		// Wait for RTC32 sysclk to become stable
	}
	
	//delay 1000ms = 1s
	delay_ms(1000);
	setup_timer();

	//set display - output lcd
	gfx_mono_draw_string("Darian Joshua ",0, 0, &sysfont);
	
	//set J1 Pin 0 to output
	ioport_set_pin_dir(J1_PIN0, IOPORT_DIR_OUTPUT);
	
	//define state
	int state = 0;
	
	//define to find interrupt
	int interrupt = 0;
	
	//define orang jalan
	int orang = 0;
	
	//define kereta jalan
	int kereta = 0;
	
	// setup lampu led di port c
	PORTC.DIR = 0b11111111; //Set output
	PORTC.OUT = 0b00000000; //Set low
	
	//setup buzzer di port e
	PORTE.DIR = 0b11111111; //Set output
	PORTE.OUT = 0b11110000; //Set low
	
	PORTC_OUTSET = PIN3_bm; // PC3 as TX
    PORTC_DIRSET = PIN3_bm; //TX pin as output
   
    PORTC_OUTCLR = PIN2_bm; //PC2 as RX
    PORTC_DIRCLR = PIN2_bm; //RX pin as input
	
	// Set Up Serial for USART
    setUpSerial();
	
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	
	usart_init_rs232(USART_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);
	
	ioport_set_pin_dir(J2_PIN0, IOPORT_DIR_OUTPUT);
	
	//forever loop
	while(1){		
		//ketika lampu merah
		if (state == 0) {	
			for (int i = 10; i > 0; i--) {				
				//in = receiveChar();
				// Untuk mengambil string dari Arduino
				receiveString();
				
				// Untuk mengecek isi stringnya
				in = usart_getchar(USART_SERIAL_EXAMPLE);
				reads[0] = in;
				
				delay_ms(50);
				
				int cekSatu = strncmp(reads, "1", 1);
				
				// Untuk Handle Ada Kereta
				if (cekSatu == 0) {
					kereta = 1;
					state = 0;
				}
				
				// Jika Masih Merah
				if (state == 0) {
					delay_us(500);
					PORTC.OUT = 0b00100000;
					gfx_mono_draw_string("Lampu Merah ",0, 8, &sysfont);
					snprintf(strbuf, sizeof(strbuf), "Counter : %3d Detik",i); // Show counter to lcd
					gfx_mono_draw_string(strbuf,0, 16, &sysfont);
					delay_ms(100);
					interrupt++;
					
					PORTB.DIR = 0b11111111; //Set output
					PORTB.OUT = 0b00000000; //Set low
					PORTB.OUT = 0b11111111; //Set high selama 5us
					delay_us(5);
					PORTB.OUT = 0b00000000; //Kembali menjadi low
					PORTB.DIR = 0b00000000; //Set menjadi input
					delay_us(750); //Delay holdoff selama 750us
					int oldinc = incremental;
					delay_us(115); //Delay lagi, kali ini seharusnya pin menjadi high
					cpu_irq_enable(); //Mulai interrupt
					while(PORTB.IN & PIN0_bm){
						//Tidak ada apa-apa di sini. Loop ini berfungsi untuk mendeteksi pin 0 PORT B yang berubah menjadi low
					}
					int newinc = incremental; //Catat selisih waktu antara suara dikirim hingga diterima
					cpu_irq_disable(); //Interrupt dimatikan
					if (incremental > 100){ //Jika hasil lebih dari 300 cm, dibulatkan menjadi 300 cm
						score = 100;
						snprintf(buffarray, sizeof(buffarray), "Panjang: %d cm  ", score);
						gfx_mono_draw_string(buffarray, 0, 24, &sysfont);
						delay_ms(100);
						incremental = 0;
					} 
					else if (incremental <= 100 && orang == 0 && kereta == 0) {
						int inc = newinc - oldinc;
						int newscore = inc/2; //Dibagi 2 seperti rumus sonar
						snprintf(buffarray, sizeof(buffarray), "Panjang: %d cm  ", newscore);
						gfx_mono_draw_string(buffarray, 0, 24, &sysfont);
						delay_ms(100);
						incremental = 0; //reset nilai variable incremental
						state = 1;
					}
					else {
						int inc = newinc - oldinc;
						int newscore = inc/2; //Dibagi 2 seperti rumus sonar
						snprintf(buffarray, sizeof(buffarray), "Panjang: %d cm  ", newscore);
						gfx_mono_draw_string(buffarray, 0, 24, &sysfont);
						delay_ms(100);
						incremental = 0; //reset nilai variable incremental
					}
				}
				else {
					break;
				}
			}
			
			// Ketika Counter Sudah 10
			if(interrupt == 10){
				PORTE.OUT = 0b11110000; // Matiin Buzzer
				orang = 0;
				kereta = 0;
				state = 1;
			}
			interrupt = 0;
		}
		
		//ketika lampu kuning
		else if(state == 1){			
			for(int i = 3; i > 0; i--){			
				//in = receiveChar();
				// Untuk mengambil string dari Arduino
				receiveString();
				
				// Untuk mengecek isi stringnya
				in = usart_getchar(USART_SERIAL_EXAMPLE);
				reads[0] = in;
				
				delay_ms(50);
				
				int cekSatu = strncmp(reads, "1", 1);
				
				// Untuk Handle Ada Kereta
				if (cekSatu == 0) {
					kereta = 1;
					state = 0;
				}
				
				// Jika Masih Kuning
				if (state == 1){
					delay_us(500);
					PORTC.OUT = 0b01000000;
					gfx_mono_draw_string("Lampu Kuning",0, 8, &sysfont);
					snprintf(strbuf, sizeof(strbuf), "Counter : %3d Detik",i); // Show counter to lcd
					gfx_mono_draw_string(strbuf,0, 16, &sysfont);
					delay_ms(200);
					interrupt++;
					
					// Jika Button Dipencet
					if (ioport_get_pin_level(GPIO_PUSH_BUTTON_0) == 0) {
						ioport_set_pin_dir(J1_PIN0, IOPORT_DIR_OUTPUT);
						PORTE.OUT = 0b11110001; // Hidupin Buzzer
						delay_us(500);
						orang = 1;
						state = 0;
					}
				}
				else {
					break;
				}
			}
			
			// Ketika Counter Sudah 3
			if(interrupt == 3){
				state = 2;
			}
			interrupt = 0;
		}
		
		//ketika lampu hijau
		else if (state == 2) {			
			for(int i = 5; i > 0;i--){
				//in = receiveChar();
				// Untuk mengambil string dari Arduino
				receiveString();
				
				// Untuk mengecek isi stringnya
				in = usart_getchar(USART_SERIAL_EXAMPLE);
				reads[0] = in;
				
				delay_ms(50);
				
				int cekSatu = strncmp(reads, "1", 1);
				
				// Untuk Handle Ada Kereta
				if (cekSatu == 0) {
					kereta = 1;
					state = 0;
				}
				
				// Jika Masih Hijau
				if (state == 2){
					delay_us(500);
					PORTC.OUT = 0b10000000;
					gfx_mono_draw_string("Lampu Hijau ",0, 8, &sysfont);
					snprintf(strbuf, sizeof(strbuf), "Counter : %3d Detik",i); // Show counter to lcd
					gfx_mono_draw_string(strbuf,0, 16, &sysfont);
					delay_ms(200);
					interrupt++;
					
					// Jika Button Dipencet
					if (ioport_get_pin_level(GPIO_PUSH_BUTTON_0) == 0) {
						ioport_set_pin_dir(J1_PIN0, IOPORT_DIR_OUTPUT);
						PORTE.OUT = 0b11110001; // Hidupin Buzzer
						delay_us(500);
						orang = 1;
						state = 0;
					}
				}
				else {
					break;
				}
			}
			
			// Ketika Counter Sudah 5
			if(interrupt == 5){
				state = 0;
			}
			interrupt = 0;
		}
	}	
}