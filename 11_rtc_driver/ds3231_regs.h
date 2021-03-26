#ifndef DS3231_REGS_H
#define DS3231_REGS_H

#define DS3231_REG_SECS		0x00
#define DS3231_REG_MIN		0x01
#define DS3231_REG_HOUR		0x02
#	define DS3231_BIT_12HR		0x40
#	define DS3231_BIT_PM		0x20
#define DS3231_REG_WDAY		0x03
#define DS3231_REG_MDAY		0x04
#define DS3231_REG_MONTH	0x05
#	define DS3231_BIT_CENTURY	0x80
#define DS3231_REG_YEAR		0x06

#define DS3231_REG_CONTROL	0x0e
#	define DS3231_BIT_nEOSC	0x80
#	define DS3231_BIT_INTCN	0x04
#	define DS3231_BIT_A2IE		0x02
#	define DS3231_BIT_A1IE		0x01
#define DS3231_REG_STATUS	0x0f
#	define DS3231_BIT_OSF		0x80
#	define DS3231_BIT_EN32KHZ	0x08
#	define DS3231_BIT_A2I		0x02
#	define DS3231_BIT_A1I		0x01

// 10 - aging
// 11 - temp msb
// 12 - temp lsb

#endif // DS3231_REGS_H
