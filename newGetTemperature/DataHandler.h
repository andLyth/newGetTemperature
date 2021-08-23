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
        void m_timer_start( std::function<void(DataHandler*)> func, std::uint32_t interval);
        void m_pollADC();
        void m_sendFile();
        std::uint32_t m_sleepTime;
        std::queue<double> m_temperQueue;
};

#endif // DATAHANDLER_H
