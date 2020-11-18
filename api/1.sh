TBL="strides_analytics.test_python_logging"
VALUES="{ \"msg\": \"hello\" }"
echo $VALUES | bq insert $TBL
