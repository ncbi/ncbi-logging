1. Go to https://console.cloud.google.com/iam-admin/serviceaccounts?project=ncbi-logmon
2. On the line for  ncbi-logmon@appspot.gserviceaccount.com, click on Actions/Create Key. Choose JSON
3. Move the downloaded file to <pathToKeyFile>, set permissions.
4. Run:
$gcloud auth activate-service-account ncbi-logmon@appspot.gserviceaccount.com --key-file=<pathToKeyFile> 
(NB: <pathToKeyFile> should not include ~)
# it may be neccessary to switch the gcp-project via '$gcloud config set project ncbi-logmon'
5. Run e.g.
$echo "select count(*) from strides_analytics.detail_export" | bq query
