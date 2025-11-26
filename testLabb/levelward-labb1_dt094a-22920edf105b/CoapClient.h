//
// Created by noahj on 2025-11-14.
//

#ifndef LABB1_DT094A_COAPCLIENT_H
#define LABB1_DT094A_COAPCLIENT_H
#include "CoapMessage.h"


class CoapClient {
private:
    size_t bufferByteSize = 4096;

public:
    void startConnTest();

    bool sendMessage(CoapMessage msg);
};


#endif //LABB1_DT094A_COAPCLIENT_H