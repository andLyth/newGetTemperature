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
    double temperature = 0.0;
    double maxVal = 0.0;
    double minVal = 50.0;
    double sum = 0.0;
    double average = 0.0;
    int countrForDivision = 0;

    static std::string endTime = "end time not generated";
    static std::string startTime; //empty on first run
    static std::deque<nlohmann::json> backupDeque;

    if(startTime.empty()) startTime = getISOCurrentTimestamp<std::chrono::nanoseconds>();

    while (!m_temperQueue.empty())
    {
        //convert ADC value to temperature and arrange temp value accordingly
        temperature = SLOPE * m_temperQueue.front()- INTERCEPT;
        m_temperQueue.pop();
        if(temperature > maxVal) maxVal = temperature;
        else if(temperature < minVal) minVal = temperature;
        sum += temperature;
        countrForDivision++;
    }

    //calculate average for timeblock
    average = sum/countrForDivision;

    //generate end time for this periods temperature readings.
    endTime = getISOCurrentTimestamp<std::chrono::nanoseconds>();

    //create output files
    m_createOutputs(&maxVal, &minVal, &average, &endTime, &startTime);
}

void DataHandler::m_createOutputs(double* ptr_maxVal, double* ptr_minVal, double* ptr_average,
                          auto* ptr_endTime, auto* ptr_startTime)
{
    static std::deque<nlohmann::json> backupDeque;
    std::ofstream o1("temeratureCalculations.json");
    std::ofstream o2("backup.json");

    //create json object
    nlohmann::json j = {
        {"time: ",
            {"start: ", *ptr_startTime},
            {"end: ", *ptr_endTime}
        },
        {"min: ", *ptr_minVal},
        {"max: ", *ptr_maxVal},
        {"average: ", *ptr_average},
    };

    //create JSON file for sending. Will be overwritten each WAIT_PERIOD_SEND
    std::cout << std::setprecision(2);
    o1 << std::setw(3) << j << std::endl;
    o1.close();

    //backup ten last JSON objects into JSON backup file. . Will be overwritten each WAIT_PERIOD_SEND
    if(backupDeque.size() >= MAX_VALUE_BACKUP_ELEMENT_SIZE) backupDeque.pop_front();
    backupDeque.push_back(j);
    for (std::deque<nlohmann::json>::iterator it = backupDeque.begin(); it!=backupDeque.end(); ++it)
    {
        std::cout << std::setprecision(2);
        o2 << std::setw(3) << j << std::endl;
    }
    o2.close();

    //test by printing
    //print(maxValuesBackup);

    //generate start time for next period.
    *ptr_startTime = getISOCurrentTimestamp<std::chrono::nanoseconds>();

}


