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

======================================================================
the logging-api has 3 modules:

1) setup the python-logger in the application which wants to log:
    the code is in: ncbi_logging_api.py
    usage:
        import ncbi_logging_api

        setup_buffer_file_log( '/my-location/my-file.log' )
        #this will rotate the logs in my-location every minute

        #there are 2 more arguments: when and interval
        #when defaults to 'M' ... minutes
        #interval defaults to 1

        documentation at:
        https://docs.python.org/3/library/logging.handlers.html#timedrotatingfilehandler

    To generate formatted log messages from Python code, use logging.error() and its siblings, e.g.:
        logging.error("message %s %d", "value", 42 )

    To pass extra data to be loaded into BigQuery columns directly, use this form:
        logging.error("message", extra = { 'bqCol1' : 'bqValue1' } )

        Make sure the keys in the 'extra' correspond to names of columns in the BigQuery table (the details are TBD)

2) the file-watcher:
    file_watcher.py
    this python3-application has to be run controlled by a cron-job
    example crontab-line running the app every minute:
    * * * * * /usr/bin/env python3 file_watcher.py -p /my-location/my-file.log -u http://concentrator-url:port
