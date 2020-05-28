#ifndef WAVEMUX_ATOMIC_H
#define WAVEMUX_ATOMIC_H

int     wavemux_atomic_load(int* value);
void    wavemux_atomic_store(int newValue, int* destination);
int     wavemux_atomic_add(int* value, int amount);

#endif
