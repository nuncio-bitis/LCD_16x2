#!/usr/bin/env bash

export total=$(grep MemTotal /proc/meminfo | awk '{ print $2 }')

export free=$(grep MemFree /proc/meminfo | awk '{ print $2 }')

export avail=$(grep MemAvailable /proc/meminfo | awk '{ print $2 }')

#echo "Total: $total ; Free: $free ; Avail: $avail"

freePct=$(echo "scale=2; 100.0 * $free / $total" | bc -q)
availPct=$(echo "scale=2; 100.0 * $avail / $total" | bc -q)

echo "Free      : $freePct %"
echo "Available : $availPct %"

