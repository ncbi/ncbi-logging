0. Prerequisites (need sudo)

$ echo "deb [signed-by=/usr/share/keyrings/cloud.google.gpg] https://packages.cloud.google.com/apt cloud-sdk main" | sudo tee -a /etc/apt/sources.list.d/google-cloud-sdk.list
$ sudo apt-get install apt-transport-https ca-certificates gnupg
$ curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key --keyring /usr/share/keyrings/cloud.google.gpg add -
$ sudo apt-get update && sudo apt-get install google-cloud-sdk
$ gcloud init

1. Go to https://console.cloud.google.com/iam-admin/serviceaccounts?project=ncbi-logmon
2. On the line for  ncbi-logmon@appspot.gserviceaccount.com, click on Actions/Create Key. Choose JSON
3. Move the downloaded file to <pathToKeyFile>, set permissions.
4. Run:
$ gcloud auth activate-service-account ncbi-logmon@appspot.gserviceaccount.com --key-file=<pathToKeyFile>
(NB: <pathToKeyFile> should not include ~)
# it may be neccessary to switch the gcp-project via '$ gcloud config set project ncbi-logmon'
5. Run e.g.
$ echo "select count(*) from strides_analytics.detail_export" | bq query
