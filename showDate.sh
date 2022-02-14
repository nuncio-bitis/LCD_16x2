#! /bin/bash

while [[ true ]]; do
    ./testLcd "$(date | cut -d" " -f1-4)" "$(date | cut -d" " -f5-)"
    sleep 0.9
done
