#! /bin/bash

while [[ true ]]; do
    ./lcd_text "$(date | cut -d" " -f1-4)" "$(date | cut -d" " -f5-)"
    sleep 0.9
done
