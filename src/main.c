#include <stdio.h>
#include <unistd.h>
#include "string.h"
#include "MQTTAsync.h"
#include "../include/iothub.h"
#include "../include/sas.h"

#define IOT_HUB_NAME ""
#define DEVICE_ID    ""
#define DEVICE_KEY   ""
#define EXPIRY       1546300800
#define PAYLOAD      "{\"id\":123}"
#define TIMEOUT      10000L

int finished = 0;

void onSubscribeSuccess(void* context, MQTTAsync_successData *response)
{
    printf("Subscribe succeeded\n");
}

void onMessageReceived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
  printf("Message arrived:\n");

  char* payloadptr;
  payloadptr = message->payload;
  for (int i = 0; i < message->payloadlen; i++)
  {
    putchar(*payloadptr++);
  }
  putchar('\n');

  MQTTAsync_freeMessage(&message);
  MQTTAsync_free(topicName);
}

void onPublishSuccess(void *context, MQTTAsync_successData *response)
{
    printf("Message was successfully published to IoT Hub from device %s\n", DEVICE_ID);
}

void onDisconnect(void *context, char *cause)
{
  printf("Device disconnected from IoT Hub. Cause: %s\n", cause);
  finished = 1;
}

void onConnectSuccess(void *context, MQTTAsync_successData *response)
{
  printf("Device %s connected to IoT Hub\n", DEVICE_ID);

  int rc = publishMessage(context, PAYLOAD, onPublishSuccess);
  if (rc != MQTTASYNC_SUCCESS) {
    printf("Failed to publish message. Response code: %d\n", rc);
    finished = 1;
  }

  printf("Subscribing to C2D messages\n");

  int rc2 = subscribe(context, onSubscribeSuccess);
  if (rc2 != MQTTASYNC_SUCCESS)
  {
    printf("Failed to start subscribe. Response code: %d\n", rc2);
    finished = 1;
  }
}

void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
  printf("Device %s failed to connect. Response code:  %d\n", DEVICE_ID, response->code);
  finished = 1;
}

int main()
{
  int sasTokenLen = 255;
  char sasToken[sasTokenLen];
  generateSasToken(sasToken, sasTokenLen, IOT_HUB_NAME, DEVICE_ID, DEVICE_KEY, EXPIRY);

  int rc = connectIoTHub(IOT_HUB_NAME, DEVICE_ID, sasToken, &onConnectSuccess, &onConnectFailure, &onDisconnect, &onMessageReceived);

  if (rc != MQTTASYNC_SUCCESS)
  {
    printf("Failed to start async connection. Response code: %d\n", rc);
    return -1;
  }

  while (!finished)
  {
    usleep(TIMEOUT);
  }
}
