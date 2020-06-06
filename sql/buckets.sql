.bail on
pragma page_size=512; -- DB is half the size
create table cloud_providers (
    cloud_provder text primary key
);
insert into cloud_providers values ('S3');
insert into cloud_providers values ('GS');
insert into cloud_providers values ('NCBI');

create table service_accounts (
    service_account text primary key,
    owner text
);

insert into service_accounts (service_account) values ('strides-analytics');
insert into service_accounts (service_account) values ('opendata');
insert into service_accounts (service_account) values ('s3_readers');

create table formats (
    format text primary key
);
insert into formats values ('GS log');
insert into formats values ('nginx log');
insert into formats values ('apache log');
insert into formats values ('cloudian log');
insert into formats values ('S3 log');
insert into formats values ('ETL');
insert into formats values ('BQS');
insert into formats values ('ETL + BQS');
insert into formats values ('Original');
insert into formats values ('fastq');
insert into formats values ('metadata');
insert into formats values ('blast');
insert into formats values ('contigs');
insert into formats values ('stat');
insert into formats values ('snps');

create table scopes (
    scope text primary key
);
insert into scopes values ('public');
insert into scopes values ('private');
insert into scopes values ('user-pays');

create table storage_classes (
    storage_class text primary key
);
insert into storage_classes values ('hot');
insert into storage_classes values ('cold');
insert into storage_classes values ('deep');

create table log_buckets (
    log_bucket text primary key
);
insert into log_buckets values ('sra-pub-src-1-logs');
insert into log_buckets values ('sra-pub-logs-1');
insert into log_buckets values ('sra-pub-run-1-logs');
insert into log_buckets values ('sra-ca-logs-1');
insert into log_buckets values ('sra-ca-run-logs');

create table buckets (
    cloud_provider text not null references cloud_providers (cloud_provider),
    bucket_name text not null,
    description text,
    owner text,
    log_bucket text references log_buckets (log_bucket) default ('sra-pub-src-1-logs'),
    service_account text references service_accounts (service_account) default 'strides-analytics',
    scope text references scopes (scope) default 'public',
    immutable text default "false",
    format text references formats (format),
    storage_class text references storage_classes (storage_class) default 'hot',

    unique (cloud_provider, bucket_name)
);

insert into buckets (cloud_provider, bucket_name, service_account, format)
    values ('S3', 'sra-pub-run-1-logs', 'strides-analytics', 'S3 log');



insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-1');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-2');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-3');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-4');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-5');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-6');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-7');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-8');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-9');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-10');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-11');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-12');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-13');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-14');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-1');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-2');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-3');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-4');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-5');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-6');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-7');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-8');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-9');

insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-1');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-2');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-3');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-4');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-5');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-6');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-7');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-8');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-9');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-10');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-11');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-12');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-13');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-14');

insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-1');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-2');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-3');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-4');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-5');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-6');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-7');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-8');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-9');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-10');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-11');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-12');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-13');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-src-14');

insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-1');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-2');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-3');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-4');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-5');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-6');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-7');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-8');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-zq-9');

insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-zq-1');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-zq-2');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-zq-3');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-zq-4');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-zq-5');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-zq-6');


-- https://confluence.ncbi.nlm.nih.gov/display/SRA/SRA+Environments+in+the+Cloud
update buckets
    set owner='nih-nlm-ncbi-sra',
    log_bucket='sra-pub-run-1-logs',
    immutable="true",
    scope='public',
    storage_class='hot'
    where bucket_name like 'sra-pub-%';

update buckets
    set format='EQL & BQS'
    where bucket_name like 'sra-pub-run-%';

update buckets
    set storage_class = 'cold' where bucket_name not in ('sra-pub-src-1','sra-pub-src-2');


update buckets
    set owner='nih-sra-datastore-protected',
    service_account='s3_readers',
    log_bucket='sra-ca-run-logs',
    immutable="true",
    scope='private',
    storage_class='hot'
    where bucket_name like 'sra-ca-%';

update buckets
    set format='ETL & BQS'
    where bucket_name like 'sra-ca-run-%';

update buckets
    set format='Original'
    where bucket_name like 'sra-ca-src-%';

update buckets
    set storage_class='cold'
    where bucket_name like 'sra-ca-src-%' and bucket_name!='sra-ca-src-1';

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
    select
        'GS',
        bucket_name,
        description,
        owner,
        log_bucket,
        service_account,
        immutable,
        format,
        storage_class
        from buckets
        where cloud_provider='S3'
        and bucket_name like 'sra-pub-%' or bucket_name like 'sra-ca-%';

update buckets set log_bucket='sra-pub-logs-1',
    format='GS log',
    owner='nih-sra-datastore'
    where cloud_provider='GS' and log_bucket='sra-pub-run-1-logs';

update buckets set log_bucket='sra-ca-logs-1',
    owner='nih-sra-datastore-protected',
    format='GS log'
    where cloud_provider='GS' and log_bucket='sra-ca-run-logs';

update buckets
set log_bucket='sra-pub-src-1-logs',
    service_account='s3_readers',
    owner='nih-nlm-ncbi-sra-opendata'
where cloud_provider='S3' and bucket_name in ('sra-pub-src-1','sra-pub-src-2');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('S3', 'sra-pub-sars-cov2', 'CoViD-19', 'Efremov', 'sra-pub-run-1-logs', 's3_readers',
    'true', 'Original', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('S3', 'sra-pub-sars-cov2-metadata-us-east-1', 'CoViD-19', 'Efremov', 'sra-pub-run-1-logs',
    's3_readers', 'true', 'Original', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap1',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap21',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap31',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap11',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap12',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap22',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap3',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'faspgap32',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp1',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp11',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp12',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp13',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp2',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp21',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp22',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp31',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp32',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'ftp33',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'gapsview11',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'gapsview21',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles11',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles12',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles13',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles21',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles22',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles23',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles31',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles32',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles33',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles34',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles35',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('NCBI', 'srafiles36',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive',
    '', 'false', 'nginx log', 'hot');



.headers on
.mode column
.mode tabs
--.width 20
select * from buckets order by cloud_provider, bucket_name;

.headers off
.mode csv
.output buckets.csv
select * from buckets order by cloud_provider, bucket_name;

