//
// Created by michael on 2018-09-15.
//

#include "common.h"


// Checks the incoming packet to see which STATE the ring is in.
// Copies the message and stores it for client to use.
void getPacketInformation(char *buffer){
  bool correctMessageFormat = false;
  for (int i = 0; i < strlen(buffer) && buffer[i] != '\0'; i++){
    // Look for the phase
    if (buffer[i] == '\n'){
      if ((ringInfo.currentPhase == NOT_STARTED || ringInfo.currentPhase == ELECTION) &&
          !strncmp(buffer, ELECTION_STR, i)){
        ringInfo.currentPhase = ELECTION;
      }else if ((ringInfo.currentPhase == ELECTION || ringInfo.currentPhase == ELECTION_OVER) &&
                !strncmp(buffer, ELECTION_OVER_STR, i)){
        ringInfo.currentPhase = ELECTION_OVER;
        // Make sure nones tries to restart the ring
      }else if ((ringInfo.currentPhase == ELECTION_OVER || ringInfo.currentPhase == MESSAGE)
                && !strncmp(buffer, MESSAGE_STR, i)){

        ringInfo.currentPhase = MESSAGE;
      } else {
        fprintf(stderr, "Received an unexpected ring phase. Terminating program\n");
        ringInfo.ringActive = false;
      }

      // Copy the nodeId or message.
      memset(ringInfo.receivedMessage, '\0', PACKET_SIZE);
      strncpy(ringInfo.receivedMessage, &buffer[i + 1], strlen(&buffer[i + 1]));
      correctMessageFormat = true;
      break;
    }
  }
  if (!correctMessageFormat){
    ringInfo.ringActive = false;
  }
}


bool shouldMessageBeForwarded(bool isUDP){
  bool forwardMessage = false;
  int ret;
  switch(ringInfo.currentPhase){
    case NOT_STARTED:
      if (isUDP){
        ringInfo.participant = false;
      } else {
        ringInfo.participant = true;
      }
      forwardMessage = true;
      break;
    case ELECTION:
      ret = strcmp(ringInfo.receivedMessage, ringInfo.highestId);
      if(ret > 0 ){
        ringInfo.participant = true;
        ringInfo.highestId = ringInfo.receivedMessage;
        forwardMessage = true;
      }
      else if (ret < 0 && ringInfo.participant == true){
        forwardMessage = false;
      }
      else if ( ret < 0 && ringInfo.participant == false){
        forwardMessage = true;
        ringInfo.participant = true;
      }
      else if ( ret == 0){
        if (strcmp(ringInfo.receivedMessage, ringInfo.ownId) == 0){
          ringInfo.currentPhase = ELECTION_OVER;
          ringInfo.participant = false;
        }
        forwardMessage = true;
      }
      break;
    case ELECTION_OVER:
      if (strcmp(ringInfo.receivedMessage, ringInfo.ownId) == 0){
        // Mark as leader to start timer
        ringInfo.ringLeader = true;
        ringInfo.currentPhase = MESSAGE;
        ringInfo.startTime = clock();
      }
      forwardMessage = true;
      break;
    case MESSAGE:
      ringInfo.message = ringInfo.receivedMessage;
      forwardMessage = true;
      break;
    default:
      fprintf(stderr, "Unknown phase - Should not happen.\n");
      ringInfo.ringActive = false;
      break;
  }
  return forwardMessage;
}

void prepareMessage(char *packetToSend){
  switch(ringInfo.currentPhase){
    case NOT_STARTED:
      strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR) + 1);
      strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
      break;
    case ELECTION:
      strncpy(packetToSend, ELECTION_STR, strlen(ELECTION_STR) + 1);
      strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
      break;
    case ELECTION_OVER:
      strncpy(packetToSend, ELECTION_OVER_STR, strlen(ELECTION_OVER_STR) + 1);
      strncat(packetToSend, ringInfo.highestId, strlen(ringInfo.highestId));
      break;
    case MESSAGE:
      strncpy(packetToSend, MESSAGE_STR, strlen(MESSAGE_STR) + 1);
      strncat(packetToSend, ringInfo.message, strlen(ringInfo.message));
      break;
    default:
      fprintf(stderr, "Invalid phase, should not happen\n");
      ringInfo.ringActive = false;
      break;
  }
}