#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// define the type of aircraft. Allowed types are QUAD_X, QUAD_PLUS, TRI, TRI_TEST and QUAD_TEST
#define TRI

//Magic macro to check enum size
//#define ctassert(n,e) extern unsigned char n[(e)?0:-1]
#define ctassert(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]
#define usleep _usleep
#define UNUSED(x) (void)(x)

#define TRUE 1
#define FALSE 0


//typedef enum { false, true } bool;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

extern u8 payload[9];

// SPI functions
void SPI_ProtoInit(void);

/* Mixer functions */
//void MIXER_CalcChannels();

enum TxPower {
    TXPOWER_100uW,
    TXPOWER_300uW,
    TXPOWER_1mW,
    TXPOWER_3mW,
    TXPOWER_10mW,
    TXPOWER_30mW,
    TXPOWER_100mW,
    TXPOWER_150mW,
    TXPOWER_LAST,
};

struct mcu_pin {
    u32 port;
    u16 pin;
};

void _usleep(u32 x);

/*
// Misc
void Delay(u32 count);
u32 Crc(const void *buffer, u32 size);
const char *utf8_to_u32(const char *str, u32 *ch);
int exact_atoi(const char *str); //Like atoi but will not decode a number followed by non-number
size_t strlcpy(char* dst, const char* src, size_t bufsize);
void tempstring_cpy(const char* src);
int fexists(const char *file);
u32 rand32_r(u32 *seed, u8 update); //LFSR based PRNG
u32 rand32(); //LFSR based PRNG
extern volatile u8 priority_ready;
void medium_priority_cb();
void debug_timing(u32 type, int startend); //This is only defined if TIMING_DEBUG is defined
// Battery
#define BATTERY_CRITICAL 0x01
#define BATTERY_LOW      0x02
u8 BATTERY_Check();
*/
#endif
