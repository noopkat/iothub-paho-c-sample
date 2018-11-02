#include <stdio.h>
#include <unistd.h>
#include "string.h"
#include "MQTTClient.h"
#include "MQTTAsync.h"

#define ROOT_CERT    "root.crt"
#define QOS          1 
#define RETAINED     0 

char deviceId[255];

int publishMessage(void *context, char *PAYLOAD, void *onPublishSuccess) 
{
  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

  int topicLen = 26 + strlen(deviceId);
  char topic[topicLen];
  snprintf(topic, topicLen, "devices/%s/messages/events/", deviceId);

  pubmsg.payloadlen = strlen(PAYLOAD);
  pubmsg.payload = PAYLOAD;
  pubmsg.qos = QOS;
  pubmsg.retained = RETAINED;

  opts.onSuccess = onPublishSuccess;
  opts.context = context;

  int rc = MQTTAsync_sendMessage(context, topic, &pubmsg, &opts);
  return rc;
}

int subscribe(void *context, void *onSubscribeSuccess)
{
  MQTTAsync client = (MQTTAsync)context;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  
  int topicLen = 32 + strlen(deviceId);
  char topic[topicLen];
  snprintf(topic, topicLen, "devices/%s/messages/devicebound/#", deviceId);

  opts.onSuccess = onSubscribeSuccess;
  opts.context = client;
  
  int rc = MQTTAsync_subscribe(client, topic, QOS, &opts);
  return rc;
}

int connectIoTHub(char *iotHubName, char *device, char *sasToken, void *onConnectSuccess, void *onConnectFailure, void *onDisconnect, void *onMessageReceived) 
{
  strcpy(deviceId, device);

  MQTTAsync client;
  MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
  MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;

  int addressLen = 30 + strlen(iotHubName);
  char address[addressLen];
  snprintf(address, addressLen, "ssl://%s.azure-devices.net:8883", iotHubName);

  int userLen = 20 + strlen(iotHubName) + strlen(deviceId);
  char username[userLen];
  snprintf(username, userLen, "%s.azure-devices.net/%s", iotHubName, deviceId);

  MQTTAsync_create(&client, address, deviceId, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  MQTTAsync_setCallbacks(client, client, onDisconnect, onMessageReceived, NULL);

  conn_opts.context = client;
  conn_opts.cleansession = 1;
  conn_opts.keepAliveInterval = 20;
  conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
  conn_opts.ssl = &sslopts;
  conn_opts.username = username;
  conn_opts.password = sasToken;

  conn_opts.onSuccess = onConnectSuccess;
  conn_opts.onFailure = onConnectFailure;

  sslopts.enableServerCertAuth = 1;
  sslopts.sslVersion = MQTT_SSL_VERSION_TLS_1_2;
  sslopts.trustStore = ROOT_CERT;

  int rc = MQTTAsync_connect(client, &conn_opts);
  return rc;
}

