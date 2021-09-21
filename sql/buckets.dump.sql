PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE cloud_providers (
    cloud_provder text primary key
);
INSERT INTO cloud_providers VALUES('S3');
INSERT INTO cloud_providers VALUES('GS');
INSERT INTO cloud_providers VALUES('OP');
INSERT INTO cloud_providers VALUES('Splunk');
CREATE TABLE service_accounts (
    service_account text primary key,
    owner text
);
INSERT INTO service_accounts VALUES('strides-analytics',NULL);
INSERT INTO service_accounts VALUES('opendata',NULL);
INSERT INTO service_accounts VALUES('s3_readers',NULL);
INSERT INTO service_accounts VALUES('logmon',NULL);
INSERT INTO service_accounts VALUES('nih-nlm-ncbi-sra-protected',NULL);
INSERT INTO service_accounts VALUES('nih-sra-datastore-protected',NULL);
INSERT INTO service_accounts VALUES('logmon_private',NULL);
CREATE TABLE log_formats (
    log_format text primary key,
    parser_binary text default '',
    parser_options text default ''
);
INSERT INTO log_formats VALUES('GS log','gcp2jsn-rel','');
INSERT INTO log_formats VALUES('S3 log','aws2jsn-rel','');
INSERT INTO log_formats VALUES('nginx log','op2jsn-rel','');
INSERT INTO log_formats VALUES('apache log','op2jsn-rel','');
INSERT INTO log_formats VALUES('traceweb','tw2jsn-rel','');
INSERT INTO log_formats VALUES('Cloudian log','cl2jsn-rel','');
CREATE TABLE formats (
    format text primary key
);
INSERT INTO formats VALUES('GS log');
INSERT INTO formats VALUES('nginx log');
INSERT INTO formats VALUES('apache log');
INSERT INTO formats VALUES('cloudian log');
INSERT INTO formats VALUES('S3 log');
INSERT INTO formats VALUES('ETL + BQS');
INSERT INTO formats VALUES('ETL - BQS');
INSERT INTO formats VALUES('Original');
INSERT INTO formats VALUES('fastq');
INSERT INTO formats VALUES('metadata');
INSERT INTO formats VALUES('blast');
INSERT INTO formats VALUES('contigs');
INSERT INTO formats VALUES('stat');
INSERT INTO formats VALUES('snps');
CREATE TABLE scopes (
    scope text primary key
);
INSERT INTO scopes VALUES('public');
INSERT INTO scopes VALUES('private');
INSERT INTO scopes VALUES('user-pays');
CREATE TABLE storage_classes (
    storage_class text primary key
);
INSERT INTO storage_classes VALUES('hot');
INSERT INTO storage_classes VALUES('cold');
INSERT INTO storage_classes VALUES('deep');
CREATE TABLE log_buckets (
    log_bucket text primary key
);
INSERT INTO log_buckets VALUES('sra-pub-src-1-logs');
INSERT INTO log_buckets VALUES('sra-pub-logs-1');
INSERT INTO log_buckets VALUES('sra-pub-run-1-logs');
INSERT INTO log_buckets VALUES('sra-ca-logs-1');
INSERT INTO log_buckets VALUES('sra-ca-run-logs');
INSERT INTO log_buckets VALUES('sra-ca-run-odp-logs');
CREATE TABLE buckets (
    cloud_provider text not null references cloud_providers (cloud_provider),
    bucket_name text not null,
    description text,
    owner text,
    log_bucket text references log_buckets (log_bucket) default ('sra-pub-src-1-logs'),
    service_account text references service_accounts (service_account) default 'strides-analytics',
    scope text references scopes (scope) default 'public',
    immutable text default "false",
    format text references formats (format),
    log_format text references logs_formats (log_format) default 'nginx log',
    storage_class text references storage_classes (storage_class) default 'hot',

    unique (cloud_provider, bucket_name)
);
INSERT INTO buckets VALUES('S3','sra-pub-run-1-logs',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-1',NULL,'nih-nlm-ncbi-sra-opendata','sra-pub-src-1-logs','s3_readers','public','true','Original','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-pub-src-2',NULL,'nih-nlm-ncbi-sra-opendata','sra-pub-src-1-logs','s3_readers','public','true','Original','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-pub-src-3',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-4',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-5',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-6',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-7',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-8',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-9',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-10',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-11',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-12',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-13',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-14',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-15',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-16',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-17',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-src-18',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-1',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-2',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-3',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-4',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-5',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-6',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-7',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-8',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-9',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-10',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-11',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-12',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-run-13',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-run-logs',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-1',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-2',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-3',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-4',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-5',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-6',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-7',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-8',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-9',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-10',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-11',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-12',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-13',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-run-14',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-src-1',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-src-2',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-3',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-4',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-5',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-6',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-7',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-8',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-9',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-10',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-11',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-12',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-13',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-src-14',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','Original','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-1',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-2',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-3',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-4',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-5',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-6',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-7',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-8',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-pub-zq-9',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL - BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-zq-1',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL - BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-zq-2',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL - BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-zq-3',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL - BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-zq-4',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL - BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-zq-5',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL - BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-zq-6',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL - BQS','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-pub-run-odp',NULL,'nih-nlm-ncbi-sra','sra-pub-run-1-logs','strides-analytics','public','true','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-run-odp',NULL,'nih-sra-datastore-protected','sra-ca-run-logs','logmon_private','private','true','ETL + BQS','S3 log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-run-1-logs',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-1',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-src-2',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-src-3',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-4',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-5',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-6',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-7',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-8',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-9',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-10',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-11',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-12',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-13',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-14',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-15',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-16',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-17',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-src-18',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-1',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-2',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-3',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-4',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-5',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-6',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-7',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-8',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-9',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-10',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-11',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-12',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-run-13',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-run-logs',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-1',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-2',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-3',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-4',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-5',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-6',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-7',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-8',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-9',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-10',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-11',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-12',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-13',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-run-14',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-src-1',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-src-2',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-3',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-4',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-5',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-6',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-7',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-8',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-9',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-10',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-11',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-12',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-13',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-src-14',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','Original','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-1',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-2',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-3',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-4',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-5',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-6',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-7',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-8',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-zq-9',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL - BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-zq-1',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL - BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-zq-2',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL - BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-zq-3',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL - BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-zq-4',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL - BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-zq-5',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL - BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-zq-6',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL - BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-run-odp',NULL,'nih-sra-datastore','sra-pub-logs-1','strides-analytics','public','true','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-run-odp',NULL,'nih-sra-datastore-protected','sra-ca-logs-1','logmon_private','private','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('S3','sra-pub-sars-cov2','CoViD-19','Efremov','sra-pub-run-1-logs','s3_readers','public','true','Original','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-pub-sars-cov2-metadata-us-east-1','CoViD-19','Efremov','sra-pub-run-1-logs','s3_readers','public','true','Original','S3 log','hot');
INSERT INTO buckets VALUES('OP','OP','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles11/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles11','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles11/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles12','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles12/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles13','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles13/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles21','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles21/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles22','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles22/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles23','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles23/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles31','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles31/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles32','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles32/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles33','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles33/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles34','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles34/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles35','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles35/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','srafiles36','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles36/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fasp*/access_*.gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap1','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap1/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap21','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap21/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap31','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap31/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap11','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap11/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap12','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap12/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap22','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap22/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap3','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap3/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','faspgap32','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap32/access.log_??????*gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp1','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp1/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp11','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp11/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp12','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp12/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp13','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp13/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp2','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp2/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp21','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp21/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp22','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp22/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp31','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp31/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp32','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp32/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp33','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp33/access.log_??????*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','gapsview','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/*gap*/access*.gz','','private','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','ftp','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp*/access_*gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('OP','sdl-sra-err','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sra.*.err*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-sra-log','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sra.*.log_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-sra-trace','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/trace.*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-names-err','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/names.*err_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-names-log','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/names.*log_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-sdlr-err','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sdlr.*err_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-sdlr-log','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sdlr.*log_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-sdlr-trace','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sdlr.*trace_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-index-err','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/index.*err_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-index-log','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/index.*log_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-index-trace','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/index.*trace_*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sdl-jwt-cart','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/jwt_cart_builder*.gz','','public','false','nginx log','traceweb','hot');
INSERT INTO buckets VALUES('OP','sweb','applog rotated','Applog','/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/weblog/local_archive/*/*/*/sweb*/*.gz','','public','false','nginx log','nginx log','hot');
INSERT INTO buckets VALUES('S3','sra-pub-metadata-us-east-1','Athena metadata','Efremov','sra-pub-src-1-logs','s3_readers','public','false','metadata','S3 log','hot');
INSERT INTO buckets VALUES('S3','sra-ca-crun-1','Cold ETL Data','efremov','sra-ca-run-logs','nih-nlm-ncbi-sra-protected','private','false','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-crun-2','Cold ETL Data','efremov','sra-ca-run-logs','nih-nlm-ncbi-sra-protected','private','false','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-crun-3','Cold ETL Data','efremov','sra-ca-run-logs','nih-nlm-ncbi-sra-protected','private','false','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-crun-4','Cold ETL Data','efremov','sra-ca-run-logs','nih-nlm-ncbi-sra-protected','private','false','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-crun-5','Cold ETL Data','efremov','sra-ca-run-logs','nih-nlm-ncbi-sra-protected','private','false','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('S3','sra-ca-crun-6','Cold ETL Data','efremov','sra-ca-run-logs','nih-nlm-ncbi-sra-protected','private','false','ETL + BQS','S3 log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-crun-1','Cold ETL Data','efremov','sra-ca-run-logs','nih-sra-protected','private','false','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-crun-2','Cold ETL Data','efremov','sra-ca-run-logs','nih-sra-protected','private','false','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-crun-3','Cold ETL Data','efremov','sra-ca-run-logs','nih-sra-protected','private','false','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-crun-4','Cold ETL Data','efremov','sra-ca-run-logs','nih-sra-protected','private','false','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-crun-5','Cold ETL Data','efremov','sra-ca-run-logs','nih-sra-protected','private','false','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-ca-crun-6','Cold ETL Data','efremov','sra-ca-run-logs','nih-sra-protected','private','false','ETL + BQS','GS log','cold');
INSERT INTO buckets VALUES('GS','sra-pub-crun-1','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-2','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-3','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-4','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-5','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-6','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-7','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-8','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-9','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-10','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-11','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-12','Hot ETL Data','efremov','sra-pub-logs-1','nih-datastore','public','false','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-pub-crun-13','Hot ETL Data','efremov','sra-ca-logs-1','logmon_private','public','true','ETL + BQS','GS log','hot');
INSERT INTO buckets VALUES('GS','sra-ca-logs-1','Log bucket','efremov','sra-ca-logs-1','logmon_private','private','true','GS log','GS log','hot');
INSERT INTO buckets VALUES('Splunk','Cloudian','Cloudian','Splunk','splunk','bearer.txt','public','false','Cloudian log','Cloudian log','hot');
COMMIT;
