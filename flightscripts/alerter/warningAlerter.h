//
// Created by joeb15 on 4/6/2017.
//

#ifndef WARNING_ALERTER_H
#define WARNING_ALERTER_H


class warningAlerter {

public :
    static void checkStatus();
private:
    static const char* exec(const char* cmd);
    static float readNextFloat(const char* s, unsigned int *pointer);

    static void sendMessage(const char *msg);
};


#endif //WARNING_ALERTER_H
