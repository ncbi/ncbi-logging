0. Prerequisites (need sudo)

(Debian WSL on Windonws 10)
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get install git-all
    sudo apt-get install python3 python3-pip
    sudo apt-get install curl

    echo "deb [signed-by=/usr/share/keyrings/cloud.google.gpg] https://packages.cloud.google.com/apt cloud-sdk main" | sudo tee -a /etc/apt/sources.list.d/google-cloud-sdk.list
    sudo apt-get install apt-transport-https ca-certificates gnupg
    curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key --keyring /usr/share/keyrings/cloud.google.gpg add -
    sudo apt-get update && sudo apt-get install google-cloud-sdk
    gcloud init
        (will ask you to login to GCP and retrieve a sign-in code which you then paste into the command line)

1. Go to https://console.cloud.google.com/iam-admin/serviceaccounts?project=ncbi-logmon

2. On the line for  ncbi-logmon@appspot.gserviceaccount.com, click on Actions/Create Key. Choose JSON

3. Move the downloaded file to <pathToKeyFile>, set permissions.

4. Authenticate:
    gcloud auth activate-service-account ncbi-logmon@appspot.gserviceaccount.com --key-file=<pathToKeyFile>
(NB: <pathToKeyFile> should not include ~)

    gcloud config set project ncbi-logmon

5. Run e.g.
    echo "select count(*) from strides_analytics.detail_export" | bq query

6. To use BigQuery Python API:
    pip3 install google google.cloud
    pip3 install --upgrade google-cloud-bigquery
    export GOOGLE_APPLICATION_CREDENTIALS=<pathToKeyFile>

7. To test, run in one window
        cd ncbi-logging/api/example
        ./ex_receiver.py

    in another window,
        cd ncbi-logging/api/example
        ./test.py

    You should see:
$./test.py
.
----------------------------------------------------------------------
Ran 1 test in 3.604s

OK
