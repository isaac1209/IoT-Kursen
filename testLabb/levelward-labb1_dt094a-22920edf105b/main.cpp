#include <iostream>

#include "CoapClient.h"
#include "CoapMessage.h"
int main() {
    CoapClient client;

    CoapMessage msg(CoapMessageType::NON, CoapMessageMethod::GET);
    client.sendMessage(msg);

    return 0;
}
