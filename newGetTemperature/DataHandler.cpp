#include "DataHandler.h"

DataHandler::DataHandler(std::string fileName, int fetchPeriod, int sendPeriod) : m_fileName(fileName), m_fetchPeriod(fetchPeriod), m_sendPeriod(sendPeriod)
{
    m_timer_start(m_pollADC/*, m_fetchPeriod*/);
    //m_timer_start(task, m_fetchPeriod);
    m_timer_start(m_convertToTemperature/*, m_sendPeriod*/);
}

DataHandler::~DataHandler(){}

void DataHandler::m_timer_start( std::function<void(DataHandler*)> func/*, std::uint32_t interval*/)
{
    std::thread([this, func/*, interval*/]()
    {
        while (true)
        {
            func(this);
        }
    }).detach();
}

void DataHandler::task(void){std::cout << 1 << std::endl;}

void DataHandler::m_pollADC()
{
    std::ifstream temperStream(m_fileName);

    if (temperStream.is_open())
    {
        while (std::getline(temperStream, m_tempStr))
        {
            /*converting adc value to temperature and pushing to queue*/
            m_temperQueue.push(stod(m_tempStr));
            auto wakeUpTime_Fetch = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_fetchPeriod);
            std::this_thread::sleep_until(wakeUpTime_Fetch);
        }
        temperStream.close();
    }
    else
    {
        std::cout << "Error opening file";
    };
}

void DataHandler::m_convertToTemperature()
{

    double temperature = 0.0;
    double maxVal = 0.0;
    double minVal = 50.0;
    double sum = 0.0;
    double average = 0.0;
    int countrForDivision = 0;
    static std::string endTime = "end time not generated";
    static std::string startTime; //empty on first run
    static std::deque<nlohmann::json> backupDeque;

    //generate nextwake up time for thread
    auto wakeUpTime_Send = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_sendPeriod);

    /*convert ADC value to temperature and arrange temp value accordingly
      and store in container*/
    while (!m_temperQueue.empty())
    {
        temperature = SLOPE * m_temperQueue.front()- INTERCEPT;
        m_temperQueue.pop();
        if(temperature > maxVal) maxVal = temperature;
        else if(temperature < minVal) minVal = temperature;
        sum += temperature;
        countrForDivision++;
    }

    //calculate average for time block
    average = sum/countrForDivision;

    //generate end time for this periods temperature readings.
    endTime = getISOCurrentTimestamp<std::chrono::nanoseconds>();

    //create output files
    m_createOutputs(&maxVal, &minVal, &average, &endTime, &startTime);

    //Thread to sleep
    std::this_thread::sleep_until(wakeUpTime_Send);
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
    backupDeque.push_back(j);
    for (std::deque<nlohmann::json>::iterator it = backupDeque.begin(); it!=backupDeque.end(); ++it)
    {
     //   std::cout << *it <<std::endl;
        o2 << std::setw(3) << j << std::endl;
    }

    if(backupDeque.size() >= MAX_VALUE_BACKUP_ELEMENT_SIZE) backupDeque.pop_front();
    o2.close();

    //pseudo-function call for sending file to localhost
    //m_sendFile();
    std::cout<<"1"<<std::endl;

    //generate start time for next period.
    *ptr_startTime = getISOCurrentTimestamp<std::chrono::nanoseconds>();


}


//Pseudo code function for filesending to localhost.
void DataHandler::m_sendFile(void)
{

    //send ."temeratureCalculations.json" or json object (j) to LOCAL_ADDRESS

    //wait for reply (code)

    //if code is 500, send "backup.json" to LOCAL_BACKUP_ADDRESS
}


