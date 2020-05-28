#include "atomic.h"


int wavemux_atomic_load(int* value) {
    return __sync_fetch_and_add(value, 0);
}

void wavemux_atomic_store(int newValue, int* destination) {
    while(1) {
        int oldValue = *destination;
        if (__sync_val_compare_and_swap(destination, oldValue, newValue)) {
            return;
        }
    }
}

int wavemux_atomic_add(int* value, int amount) {
    return __sync_fetch_and_add(value, amount);
}
