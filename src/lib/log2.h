#ifndef LOG2_H
#define LOG2_H

#define _LOG_1(n) (((n) >= 2) ? 1 : 0)
#define _LOG_2(n) (((n) >= 1<<2) ? (2 + _LOG_1((n)>>2)) : _LOG_1(n))
#define _LOG_4(n) (((n) >= 1<<4) ? (4 + _LOG_2((n)>>4)) : _LOG_2(n))
#define _LOG_8(n) (((n) >= 1<<8) ? (8 + _LOG_4((n)>>8)) : _LOG_4(n))
#define LOG_2(n)  (((n) >= 1<<16) ? (16 + _LOG_8((n)>>16)) : _LOG_8(n))

#endif