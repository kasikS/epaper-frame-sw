#ifndef POWER_H
#define POWER_H

#include <stdbool.h>

void power_init(void);
void power_sd(bool enable);
void power_epaper(bool enable);
void power_esp(bool enable);

#endif // POWER_H
