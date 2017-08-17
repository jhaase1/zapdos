#!/bin/bash

echo "scale=2; `qstat -u jhaase1 -s r | wc -l`/`qstat -u jhaase1 | wc -l`" | bc
