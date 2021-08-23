#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <string>
#include <chrono>
#include <cstdint>
#include <thread>
#include <functional>
#include <iostream>
#include <fstream>
#include <queue>
#include <deque>
#include <json.hpp>
#include <precision.h>      //for time precision

#define LOCAL_ADDRESS "http://localhost:5000/api/temperature"
#define LOCAL_BACKUP_ADDRESS "http://localhost:5000/api/temperature/missing"
#define SLOPE 23.0/952.0d                   /*linear function for ADC (x) -> temp conversion: f(x) = (23/952)*x - 50 */
#define INTERCEPT 50.0d                     /*linear function for ADC (x) -> temp conversion: f(x) = (23/952)*x - 50 */
#define MAX_VALUE_BACKUP_ELEMENT_SIZE 10   /*number of max values to be backed up for error handling (errorcode: 500)*/


class DataHandler
{
    public:
        DataHandler(std::string fileName, int fetchPeriod, int sendPeriod);
        ~DataHandler();

    protected:

    private:
        int m_fetchPeriod;
        int m_sendPeriod;
        std::string m_tempStr;
        std::string m_fileName;
        void m_timer_start( std::function<void(DataHandler*)> func/*, std::uint32_t interval*/);
        void m_createOutputs(double* ptr_maxVal, double* ptr_minVal, double* ptr_average,
                          auto* ptr_endTime, auto* ptr_startTime);
        void m_pollADC();
        void m_convertToTemperature();
        void m_createOutputs();
        void m_sendFile();
        void task(void);
        std::uint32_t m_sleepTime;
        std::queue<double> m_temperQueue;
};

#endif // DATAHANDLER_H
