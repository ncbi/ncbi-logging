#!/bin/bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

ADMINPASSWORD=$(head -c 9 /dev/urandom | base64)
PORT=3389

cd "$HOME" || exit

curl -O "https://dl.grafana.com/oss/release/$GRAFANAVER.linux-amd64.tar.gz"
tar -xaf "$GRAFANAVER.linux-amd64.tar.gz"
rm -f "$GRAFANAVER.linux-amd64.tar.gz"

cd "$GRAFANAVER" || exit
mkdir -p data
mkdir -p logs
mkdir -p plugins



cat <<-EOF > conf/grafana.ini
[paths]
data = $PWD/data
logs = $PWD/logs
plugins = $PWD/plugins

[server]
http_port = $PORT
domain = $HOSTNAME
enforce_domain = true
root_url = http://$HOSTNAME:$PORT

[database]
type = postgres
host = localhost
name = grafana
user = $USER
log_queries = true
max_open_conn = 16
conn_max_lifetime = 1800

[analytics]
reporting_enabled = false
check_for_updates = false

[security]
admin_password = $ADMINPASSWORD
disable_gravatar = true

[users]
allow_sign_up = false

[alerting]
enabled = false
EOF

bin/grafana-cli plugins install grafana-worldmap-panel
bin/grafana-cli plugins install satellogic-3d-globe-panel
bin/grafana-cli plugins install doitintl-bigquery-datasource
bin/grafana-cli plugins install grafana-piechart-panel

nohup bin/grafana-server -config conf/grafana.ini web &

sleep 10

URL="http://admin:$ADMINPASSWORD@$HOSTNAME:$PORT/api/admin/users"
for USER in "vartanianmh" "raetzw" "arndt" ; do
    USERPASSWORD=$(head -c 9 /dev/urandom | base64)
    echo "$USER $USERPASSWORD"
    echo "$USER $USERPASSWORD" >> ~/grafana_passwd
    NEWUSER="{ \"name\":\"$USER\", \"email\": \"$USER@ncbi.nlmnih.gov\", \"login\":\"$USER\", \"Password\": \"$USERPASSWORD\" }"
    curl -X POST -H  "Content-Type: application/json" -d "$NEWUSER" "$URL"
done
GUESTUSER={' "name":"guest", "email": "guest@ncbi.nlmnih.gov", "login^":"guest", "Password": "Z3Vlc3QK" }'
curl -X POST -H  "Content-Type: application/json" -d "$GUESTUSER" "$URL"

