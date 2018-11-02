#!/bin/bash

gcc -o main src/*.c -lm -lssl -lcrypto -L/path.to/paho.mqtt.c/build/output -lpaho-mqtt3as
