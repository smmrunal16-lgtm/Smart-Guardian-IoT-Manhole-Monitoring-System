/* Portable C demo: print a sample JSON reading. Not a physical sensor driver. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(void) {
    srand((unsigned int)time(NULL));
    int h2s = rand() % 1400, ch4 = rand() % 1400;
    double temperature = 24.0 + (rand() % 100) / 10.0;
    double humidity = 40.0 + (rand() % 400) / 10.0;
    printf("{\"device_id\":\"c-simulator\",\"h2s_raw\":%d,\"ch4_raw\":%d,"
           "\"temperature_c\":%.1f,\"humidity_pct\":%.1f}\n",
           h2s, ch4, temperature, humidity);
    return 0;
}
