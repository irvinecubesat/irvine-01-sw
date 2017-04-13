//
// Created by joeb15 on 4/6/2017.
//

#include "string"
#include "warningAlerter.h"
#include <iostream>
#include <memory>

const char* warningAlerter::exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result.c_str();
}

float warningAlerter::readNextFloat(const char* str, unsigned int *pointer){
    std::string currFloat = "";
    std::string s = str;
    char curr;
    bool stillReading = true;
    bool startedReadingNumbers = false;

    //only go on if still reading
    while(stillReading) {
        //prevent overflow of pointer
        if(*pointer>=s.size()){
            break;
        }
        //reads current character at position *pointer, and increments the pointer
        curr = s.at((*pointer)++);

        //if it is a number or a decimal, append to buffer
        if (curr =='-' || curr == '.' || (curr >= '0' && curr <= '9')) {
            startedReadingNumbers=true;
            currFloat+=curr;
        } else if(startedReadingNumbers){
            stillReading=false;
        }
    }

    //prevent invalid conversions
    if(currFloat.size()==0)
        return -1;

    //returns the float data of our string
    return std::stof(currFloat.c_str(), 0);
}

void warningAlerter::sendMessage(const char* msg){
    std::string message = msg;
    system("\""+message+"\" | mail -s \"CubeSat Status\" joebanko15@gmail.com");
}

void warningAlerter::checkStatus(){
    const char *result = exec("sys-status | grep fuel");
//    const char *result = "fuelOne volt: 3.908798 V\nfuelOne current: 0.000000 A\nfuelOne currentAccum: 0.000000 A\nfuelOne volt: 3.908798 V\nfuelOne current: 0.000000 A\nfuelOne currentAccum: 0.000000 A\n";
    unsigned int pointer = 0;

    float volt1 = readNextFloat(result, &pointer);
    float curr1 = readNextFloat(result, &pointer);
    float curr1accum = readNextFloat(result, &pointer);

    float volt2 = readNextFloat(result, &pointer);
    float curr2 = readNextFloat(result, &pointer);
    float curr2accum = readNextFloat(result, &pointer);

    const char *result2 = exec("sys-status | grep temp");
//    const char *result2 = "daughter_aTmpSensor temp: 28.500000 C\ndaughter_bTmpSensor temp: 28.250000 C\nthreeV_plTmpSensor temp: 28.500000 C\nthreeV_plTmpSensor temp: 28.500000 C\nthreeV_plTmpSensor temp: 28.500000 C\nthreeV_plTmpSensor temp: 28.500000 C\nthreeV_plTmpSensor temp: 28.500000 C\nthreeV_plTmpSensor temp: 28.500000 C\nthreeV_plTmpSensor temp: 28.500000 C\nthreeV_plTmpSensor temp: 28.500000 C\n";
    pointer = 0;

    const int NUMBER_OF_TEMPERATURE_READINGS = 10;

    float temperatures[NUMBER_OF_TEMPERATURE_READINGS];
    for(int c=0;c<NUMBER_OF_TEMPERATURE_READINGS;c++){
        temperatures[c] = readNextFloat(result2, &pointer);
    }

    std::string res1 = result;
    std::string res2 = result2;

    sendMessage((res1+"\n"+res2).c_str());

    /*
        Voltage in V, Current in A, and Temp in C
    */
}
