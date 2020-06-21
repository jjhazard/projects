#!/bin/sh

for i in midmark.um sandmark.umz
do
    /usr/bin/time -f "um $i: %e seconds" ./um $i > /dev/null
done
