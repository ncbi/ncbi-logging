#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

pip3.4 install --user gunicorn psycopg2-binary crcmod google-cloud-bigquery google-cloud pylint google-cloud-storage
pip3.6 install --user --upgrade gunicorn psycopg2-binary crcmod google-cloud-bigquery google-cloud pylint black google-cloud-storage python-whois netaddr flake8 boto3
