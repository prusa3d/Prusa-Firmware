//uart2.cpp

/**
 * Robert McKenzie 7 byte payload comms protol with 0x7F as start byte
 * and 0xF7 as finish bytes.
 * ACK is with 0x06
 * NACK is 0x15
 */

#include "uart2.h"
#include "system_timer.h"

volatile unsigned char readRxBuffer, rxData1 = 0, rxData2 = 0, rxData3 = 0,
                                     rxData4 = 0, rxData5 = 0, rxFINDA = 0;
volatile bool confirmedPayload = false, confirmedFINDA = false, atomic_MMU_IRSENS = false;
volatile long rxTimeout = _micros();
enum class rx
{
    Idle,
    Data1,
    Data2,
    Data3,
    Data4,
    Data5,
    End,
    FINDA,
    EndFINDA
};
rx rxCount = rx::Idle;

inline rx& operator++(rx& byte, int)
{
    const int i = static_cast<int>(byte) + 1;
    byte = static_cast<rx>((i) % 9);
    return byte;
}

void uart2_init(void)
{
    UCSR2A = (0 << U2X2); // baudrate multiplier
    UCSR2B = (1 << RXEN2) | (1 << TXEN2) | (0 << UCSZ22); // enable receiver and transmitter
    UCSR2C = (0 << UMSEL21) | (0 << UMSEL20) | (0 << UPM21) |
             (0 << UPM20) | (1 << USBS2) |(1 << UCSZ21) | (1 << UCSZ20); // Use 8-bit character sizes
    UBRR2H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
    UBRR2L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
    UCSR2B |= (1 << RXCIE2); // enable rx interrupt
}

ISR(USART2_RX_vect)
{
    readRxBuffer = UDR2;
    if (rxTimeout + 1855 < _micros()) rxCount = rx::Idle;
    switch (rxCount) {
    case rx::Idle:
        if (readRxBuffer == 0x7F) { rxCount++; rxTimeout = _micros(); }
        if (readRxBuffer == 0x06) { rxCount = rx::FINDA; rxTimeout = _micros(); }
        break;
    case rx::Data1:
        rxData1 = readRxBuffer;
        rxCount++;
        break;
    case rx::Data2:
        rxData2 = readRxBuffer;
        rxCount++;
        break;
    case rx::Data3:
        rxData3 = readRxBuffer;
        rxCount++;
        break;
    case rx::Data4:
        rxData4 = readRxBuffer;
        rxCount++;
        break;
    case rx::Data5:
        rxData5 = readRxBuffer;
        rxCount++;
        break;
    case rx::End:
        if (readRxBuffer == 0xF7) {
            if (rxData1 == 'I' && rxData2 == 'R' && rxData3 == 'S' && rxData4 == 'E' && rxData5 == 'N') atomic_MMU_IRSENS = true;
            else confirmedPayload = true;
        }
        rxCount = rx::Idle;
        break;
    case rx::FINDA:
        rxFINDA = readRxBuffer;
        rxCount++;
        break;
    case rx::EndFINDA:
        if (readRxBuffer == 0xF7) confirmedFINDA = true;
        rxCount = rx::Idle;
        break;
    }
}

void uart2_txPayload(unsigned char payload[])
{
#ifdef MMU_DEBUG
    printf_P(PSTR("\nUART2 TX 0x%2X %2X %2X\n"), payload[0], payload[1], payload[2]);
#endif //MMU_DEBUG
    mmu_last_request = _millis();
    loop_until_bit_is_set(UCSR2A, UDRE2);     // Do nothing until UDR is ready for more data to be written to it
    UDR2 = 0x7F;
    for (uint8_t i = 0; i < 5; i++) {
        loop_until_bit_is_set(UCSR2A, UDRE2); // Do nothing until UDR is ready for more data to be written to it
        UDR2 = (0xFF & (int)payload[i]);
    }
    loop_until_bit_is_set(UCSR2A, UDRE2);     // Do nothing until UDR is ready for more data to be written to it
    UDR2 = 0xF7;
}
