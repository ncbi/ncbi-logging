#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

pip3 install --user --upgrade gunicorn psycopg2-binary crcmod google-cloud-bigquery google-cloud pylint black google-cloud-storage python-whois netaddr flake8 boto3
