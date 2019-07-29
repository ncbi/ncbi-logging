#!/bin/bash

. $HOME/strides/strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=/home/vartanianmh/strides-service-account-file.json

date
~/strides/gs_agent_blast_hackathon.py
date
