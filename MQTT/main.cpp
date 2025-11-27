#include <iostream>
#include "MQTT_broker.h"

int main() {
    MQTTBroker broker;
    broker.start();
    return 0;
}