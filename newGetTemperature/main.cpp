#include <iostream>
#include <queue>            //std::queue
#include "DataHandler.h"

#define TEMPERATURE_FILE "temperature.txt"
#define WAIT_PERIOD_SEND 3000
#define WAIT_PERIOD_FETCH 100




int main()
{
    DataHandler handler1(TEMPERATURE_FILE, WAIT_PERIOD_FETCH, WAIT_PERIOD_SEND);


    while(true){}
}
