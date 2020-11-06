1. Go to https://console.cloud.google.com/iam-admin/serviceaccounts?project=ncbi-logmon
2. On th eline for  ncbi-logmon@appspot.gserviceaccount.com, click on Actions/Create Key. Choose JSON
3. Move the downloaded file to <pathToKeyFile>, set permissions.
4. Run
gcloud auth activate-service-account ncbi-logmon@appspot.gserviceaccount.com --key-file=<pathToKeyFile> 
(NB: <pathToKeyFile> should not include ~)
5. Run e.g.
echo "select count(*) from strides_analytics.detail_export" | bq query