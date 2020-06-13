buckets.db : buckets.sql
	-rm -f buckets.db buckets.tsv buckets.csv
	sqlite3 buckets.db < buckets.sql > buckets.tsv
	echo "gsutil versioning set on gs://strides_analytics_cfg/"
	echo "gsutil cp buckets.db gs://strides_analytics_cfg/buckets.db"
	echo "rm -f /dev/shm/vartanianmh/buckets.db"


