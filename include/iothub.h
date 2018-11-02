#ifndef IOTHUB 
  #define IOTHUB
  int publishMessage(void* context, char *PAYLOAD, void *onPublishSuccess);
  int subscribe(void *context, void *onSubscribeSuccess);
  int connectIoTHub(char *IOT_HUB_NAME, char *DEVICE_ID, char *SAS_TOKEN, void *onConnectSuccess, void *onConnectFailure, void *onDisconnect, void *onMessageReceived);

#endif

