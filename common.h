//
// Created by michael on 2018-09-15.
//

#ifndef DODOU1_COMMON_H
#define DODOU1_COMMON_H

#include <string.h>
#include <stdio.h>
#include "ringmain.h"
#include <stdbool.h>

/*
* Retreives and change currentPhase to the one specified in buffer
*/
void getPacketInformation(char *buffer);

/*
* Checks the content of the incoming message, and will make a decision
* if or what message that should be forwarded in the ring.
* Return true if message should be forwarded/sent.
*/
bool shouldMessageBeForwarded(bool isUDP);

/*
* Prepares the next packet/message.
*/
void prepareMessage();

#endif //DODOU1_COMMON_H
