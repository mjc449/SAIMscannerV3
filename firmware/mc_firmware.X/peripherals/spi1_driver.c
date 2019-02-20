/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "mcc.h"
#include "spi1_driver.h"

inline void spi1_close(void)
{
    SPI1STATbits.SPIEN = 0;
}

//con1 == SPIxCON1, con2 == SPIxCON2, stat == SPIxSTAT
typedef struct { uint16_t con1; uint16_t con2; uint16_t stat;} spi1_configuration_t;
static const spi1_configuration_t spi1_configuration[] = {   
};

bool spi1_master_open(spi1_modes spiUniqueConfiguration)
{
    if(!SPI1STATbits.SPIEN)
    {
        SPI1CON1 = spi1_configuration[spiUniqueConfiguration].con1 | 0x0020;
        SPI1CON2 = spi1_configuration[spiUniqueConfiguration].con2;
        SPI1STAT = spi1_configuration[spiUniqueConfiguration].stat | 0x8000;
        
        SCK1OUT_SetDigitalOutput();
        return true;
    }
    return false;
}

//Setup SPI for Slave Mode
bool spi1_slave_open(spi1_modes spiUniqueConfiguration)
{
    if(!SPI1STATbits.SPIEN)
    {
        SPI1CON1 = spi1_configuration[spiUniqueConfiguration].con1;
        SPI1CON2 = spi1_configuration[spiUniqueConfiguration].con2;
        SPI1STAT = spi1_configuration[spiUniqueConfiguration].stat | 0x8000;
        
        SCK1OUT_SetDigitalInput();
        return true;
    }
    return false;
}

// Full Duplex SPI Functions
uint8_t spi1_exchangeByte(uint8_t b)
{
    SPI1BUF = b;
    while(!SPI1STATbits.SPIRBF);
    return SPI1BUF;
}

void spi1_exchangeBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data = spi1_exchangeByte(*data );
        data++;
    }
}

// Half Duplex SPI Functions
void spi1_writeBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        spi1_exchangeByte(*data++);
    }
}

void spi1_readBlock(void *block, size_t blockSize)
{
    uint8_t *data = block;
    while(blockSize--)
    {
        *data++ = spi1_exchangeByte(0);
    }
}
