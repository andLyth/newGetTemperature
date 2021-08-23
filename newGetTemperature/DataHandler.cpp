#include "DataHandler.h"

DataHandler::DataHandler(std::string fileName, int fetchPeriod, int sendPeriod) : m_fileName(fileName), m_fetchPeriod(fetchPeriod), m_sendPeriod(sendPeriod)
{
    m_timer_start(m_pollADC, m_fetchPeriod);
    m_timer_start(m_sendFile, m_sendPeriod);
}

DataHandler::~DataHandler(){}

void DataHandler::m_timer_start( std::function<void(DataHandler*)> func, std::uint32_t interval)
{
    std::thread([this, func, interval]()
    {
        while (true)
        {
            auto wakeUpTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
            func(this);
            std::this_thread::sleep_until(wakeUpTime);
        }
    }).detach();
}

void DataHandler::m_pollADC()
{
    std::ifstream temperStream(m_fileName);

    if (temperStream.is_open())
    {
        while (std::getline(temperStream, m_tempStr))
        {
            /*converting adc value to temperature and pushing to queue*/
            m_temperQueue.push(stod(m_tempStr));
        }
        std::cout << "1" << std::endl;
        temperStream.close();


    }
    else
    {
        std::cout << "Error opening file";
    };

}

void DataHandler::m_sendFile()
{
    std::cout << "2" << std::endl;
}

