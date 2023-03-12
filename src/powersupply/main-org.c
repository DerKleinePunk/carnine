#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define SerBufMax 100

// Message queueing variables
volatile uint8_t SerInBuffIn, SerInBuffOut;
volatile uint8_t InputBuffer[SerBufMax];
volatile uint8_t SerOutBuffIn, SerOutBuffOut;
volatile uint8_t OutputBuffer[SerBufMax];
volatile uint8_t LEDTimer;
volatile uint16_t ADCRes[8];

// System variables
volatile uint32_t GlobalTime;
volatile uint8_t Int1msCount;
volatile uint8_t Int100msCount;
volatile uint16_t Int1000msCount;
volatile uint8_t Int10msCount;
volatile uint8_t Int25msCount;
volatile uint8_t Sleep;

uint32_t RunTime;

SIGNAL(SIG_OUTPUT_COMPARE0A) // Global Timer, 1000 Hz
{
    GlobalTime++;
    Int1msCount++;
}

void sleep(long ms)
{
    long Gt;

    Gt = GlobalTime;
    while(Gt + ms > GlobalTime)
        ;
}

SIGNAL(SIG_ADC)
{
    volatile uint16_t Temp;

    Temp = ADCL;
    Temp = Temp + (ADCH << 8);
    ADCRes[ADMUX & 7] = Temp;

    ADMUX = ((ADMUX & (192 + 7)) + 1) & 199;
    ADCSRA = ADCSRA | 64;
}

SIGNAL(SIG_USART_DATA) // Received Data from PC
{
    if(SerOutBuffIn != SerOutBuffOut) {
        UDR0 = OutputBuffer[SerOutBuffOut];
        if(SerOutBuffOut >= SerBufMax - 1)
            SerOutBuffOut = 0;
        else
            SerOutBuffOut++;
    } else
        UCSR0B = UCSR0B & (255 - 32);
}

SIGNAL(SIG_USART_RECV) // Received Data from PC
{
    InputBuffer[SerInBuffIn] = UDR0;
    if(SerInBuffIn >= SerBufMax - 1) {
        if(SerInBuffOut != 0) SerInBuffIn = 0;
    } else
        SerInBuffIn++;
}

uint8_t NewDataInAvailable()
{
    if(SerInBuffIn != SerInBuffOut)
        return (1);
    else
        return (0);
}

uint8_t GetDataIn()
{
    uint8_t Temp;

    if(NewDataInAvailable() == 1) {
        Temp = InputBuffer[SerInBuffOut];
        if(SerInBuffOut >= SerBufMax - 1)
            SerInBuffOut = 0;
        else
            SerInBuffOut++;
        return Temp;
    } else
        return (0);
}

void SendByte(uint8_t Data)
{
    cli();
    OutputBuffer[SerOutBuffIn] = Data;
    if(SerOutBuffIn >= SerBufMax - 1)
        SerOutBuffIn = 0;
    else
        SerOutBuffIn++;
    sei();
    UCSR0B = UCSR0B | 32;
}

void SendString(char s[50])
{
    uint8_t n;

    n = 0;
    while(n < 50 && s[n] != 0)
        SendByte(s[n++]);
    SendByte(13);
}

void SendString_(char s[50])
{
    uint8_t n;

    n = 0;
    while(n < 50 && s[n] != 0)
        SendByte(s[n++]);
}

void InitTimer()
{
    TCCR0A = 3;
    TCCR0B = 8 + 3;
    OCR0A = 125;
    TIMSK0 = 2;
}

void InitSerial()
{
    UCSR0A = 2;
    UCSR0B = 128 + 16 + 8;
    UCSR0C = 128 | 6;
    UBRR0H = 0;
    UBRR0L = 25; //  BPS
}

void InitGlobalVariables()
{
    SerInBuffIn = 0;
    SerInBuffOut = 0;
    SerOutBuffIn = 0;
    SerOutBuffOut = 0;
    LEDTimer = 0;
    GlobalTime = 0;
    Int1msCount = 0;
    Int1000msCount = 0;
    Int100msCount = 0;
    Int10msCount = 0;
}

void InitIO()
{
    PORTD = 3;
    DDRD = 128 + 2;
    PORTC = 32 + 16;
    DDRC = 32;
    PORTB = 0;
    DDRB = 7;
}

void InitADC()
{
    ADMUX = 64;
    ADCSRA = 128 + 64 + 8 + 7;
}

void InitWDT()
{
    WDTCSR = 16 + 8;
    WDTCSR = 8 + 3;
}

void Init()
{
    InitIO();
    InitGlobalVariables();
    InitTimer();
    InitSerial();
    InitADC();
    InitWDT();
    sei();
}

void SwitchToBootLoaderOperation()
{
    volatile long n;

    for(n = 0; n < 1000; n++)
        asm("WDR");
    WDTCSR = 16 + 8;
    WDTCSR = 8 + 1;
    cli();
    while(1)
        ;
}


void SwitchOff()
{
    PORTC = PORTC & (255 - 32);
    while(1)
        asm("WDR");
}

uint8_t KL15_Aktiv() // 1 if KL15 is 12V
{
    if((PINC & 16) == 0)
        return (0);
    else
        return (1);
}

uint16_t Get12Volt() // Result in mV
{
    return (ADCRes[6] * 35);
}

uint8_t USB_Connected()
{
    if((PINC & 1) == 1)
        return (1);
    else
        return (0);
}

uint8_t GetAnalogInput(uint8_t Nr) // mV
{
    if(Nr >= 0 && Nr <= 2)
        return (ADCRes[Nr + 1] * 3300 / 1024);
    else
        return (0);
}

void Timer100ms()
{
}

void Timer10ms()
{
}

void Timer25ms()
{
}

void Timer1000ms()
{
    char s[100];

    PORTD = PORTD ^ 128; // LED
    RunTime++;

    SendString_("KL15: ");
    itoa(KL15_Aktiv(), s, 10);
    SendString_(s);

    SendString_("Spannung (0,1V): ");
    itoa(Get12Volt() / 100, s, 10);
    SendString_(s);

    SendString("");
}

void Timer1ms()
{
    Int1000msCount++;
    if(Int1000msCount > 1000) {
        Int1000msCount = 0;
        Timer1000ms();
    }
    Int100msCount++;
    if(Int100msCount > 100) {
        Int100msCount = 0;
        Timer100ms();
    }
    Int25msCount++;
    if(Int25msCount > 25) {
        Int25msCount = 0;
        Timer25ms();
    }
    Int10msCount++;
    if(Int10msCount > 10) {
        Int10msCount = 0;
        Timer10ms();
    }
}

void SetRelaisOn(uint8_t Nr)
{
    if(Nr == 0) PORTB = PORTB | 1;
    if(Nr == 1) PORTB = PORTB | 2;
    if(Nr == 2) PORTB = PORTB | 4;
}

void SetRelaisOff(uint8_t Nr)
{
    if(Nr == 0) PORTB = PORTB & 254;
    if(Nr == 1) PORTB = PORTB & 253;
    if(Nr == 2) PORTB = PORTB & 251;
}

void Interpret(uint8_t Command)
{
    if(Command == 'U') SwitchToBootLoaderOperation();
    if(Command == '1') SetRelaisOn(0);
    if(Command == '2') SetRelaisOn(1);
    if(Command == '3') SetRelaisOn(2);
    if(Command == '4') SetRelaisOff(0);
    if(Command == '5') SetRelaisOff(1);
    if(Command == '6') SetRelaisOff(2);
}

void MainLoop()
{
    if(Int1msCount > 0) {
        Timer1ms();
        Int1msCount--;
    }
    if(NewDataInAvailable() == 1) Interpret(GetDataIn());
    asm("WDR");
}

int main()
{
    Init();
    SendString("Running OK");
    while(1)
        MainLoop();
}
