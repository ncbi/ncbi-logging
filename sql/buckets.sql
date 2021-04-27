.bail on
pragma page_size=512; -- DB is half the size

create table cloud_providers (
    cloud_provder text primary key
);
insert into cloud_providers values ('S3');
insert into cloud_providers values ('GS');
insert into cloud_providers values ('OP');
insert into cloud_providers values ('Splunk');

create table service_accounts (
    service_account text primary key,
    owner text
);

insert into service_accounts (service_account) values ('strides-analytics');
insert into service_accounts (service_account) values ('opendata');
insert into service_accounts (service_account) values ('s3_readers');
insert into service_accounts (service_account) values ('logmon');
insert into service_accounts (service_account) values ('nih-nlm-ncbi-sra-protected');
insert into service_accounts (service_account) values ('nih-sra-datastore-protected');
insert into service_accounts (service_account) values ('logmon_private');

create table log_formats (
    log_format text primary key,
    parser_binary text default '',
    parser_options text default ''
);

insert into log_formats values ('GS log', 'gcp2jsn-rel', '');
insert into log_formats values ('S3 log', 'aws2jsn-rel', '');
insert into log_formats values ('nginx log', 'op2jsn-rel', '');
insert into log_formats values ('apache log', 'op2jsn-rel', '');
insert into log_formats values ('traceweb', 'tw2jsn-rel', '');
insert into log_formats values ('Cloudian log', 'cl2jsn-rel', '');

create table formats (
    format text primary key
);
insert into formats values ('GS log');
insert into formats values ('nginx log');
insert into formats values ('apache log');
insert into formats values ('cloudian log');
insert into formats values ('S3 log');
insert into formats values ('ETL + BQS');
insert into formats values ('ETL - BQS');
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
    log_format text references logs_formats (log_format) default 'nginx log',
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
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-15');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-16');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-17');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-src-18');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-1');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-2');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-3');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-4');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-5');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-6');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-7');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-8');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-9');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-10');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-11');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-12');
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-13');

insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-ca-run-logs');
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

insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-odp');

-- https://confluence.ncbi.nlm.nih.gov/display/SRA/SRA+Environments+in+the+Cloud
update buckets
    set owner='nih-nlm-ncbi-sra',
    log_bucket='sra-pub-run-1-logs',
    immutable="true",
    scope='public',
    storage_class='hot'
    where bucket_name like 'sra-pub-%';

update buckets
    set format='EQL + BQS'
    where bucket_name like 'sra-pub-run-%' and bucket_name not like '%log%';

update buckets
    set storage_class = 'cold' where bucket_name not in ('sra-pub-src-1','sra-pub-src-2');


update buckets
    set owner='nih-sra-datastore-protected',
    service_account='logmon_private',
    log_bucket='sra-ca-run-logs',
    immutable="true",
    scope='private',
    storage_class='hot'
    where bucket_name like '%-ca-%';

update buckets
    set format='ETL + BQS'
    where bucket_name like 'sra-ca-run-%' or bucket_name like 'sra-pub-run-%';

update buckets
    set format='ETL - BQS'
    where bucket_name like '%-zq-%';

update buckets
    set format='Original'
    where bucket_name like '%-src-%';

update buckets
    set storage_class='cold'
    where bucket_name like 'sra-ca-src-%' and bucket_name!='sra-ca-src-1';

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
    select
        'GS',
        bucket_name,
        description,
        owner,
        log_bucket,
        service_account,
        immutable,
        format,
        storage_class,
        scope
        from buckets
        where cloud_provider='S3'
        and bucket_name like 'sra-pub-%' or bucket_name like 'sra-ca-%' or bucket_name like '%-zq-%';

update buckets
    set format='GS bucket'
    where cloud_provider='GS' and format='S3 bucket';

update buckets set log_bucket='sra-pub-logs-1',
    owner='nih-sra-datastore'
    where cloud_provider='GS' and log_bucket='sra-pub-run-1-logs';

update buckets set log_bucket='sra-ca-logs-1',
    service_account='logmon_private',
    owner='nih-sra-datastore-protected'
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
values ('OP', 'OP',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles11/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles11',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles11/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles12',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles12/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles13',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles13/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles21',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles21/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles22',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles22/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles23',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles23/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles31',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles31/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles32',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles32/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles33',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles33/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles34',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles34/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles35',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles35/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'srafiles36',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/srafiles36/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fasp*/access_*.gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap1',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap1/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap21',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap21/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap31',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap31/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap11',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap11/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap12',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap12/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap22',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap22/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap3',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/fapgap3/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'faspgap32',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/faspgap32/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp1',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp1/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp11',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp11/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp12',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp12/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp13',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp13/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp2',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp2/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp21',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp21/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp22',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp22/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp31',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp31/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp32',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp32/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp33',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp33/access.log_??????*gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class, scope)
values ('OP', 'gapsview',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/gaps*/access_*.gz',
    '', 'false', 'nginx log', 'hot', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'ftp',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/ftp.http/local_archive/*/*/*/ftp*/access_*gz',
    '', 'false', 'nginx log', 'hot');

-- LOGMON-21
insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-sra-err',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sra.*.err*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-sra-log',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sra.*.log_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-sra-trace',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/trace.*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-names-err',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/names.*err_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-names-log',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/names.*log_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-sdlr-err',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sdlr.*err_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-sdlr-log',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sdlr.*log_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-sdlr-trace',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/sdlr.*trace_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-index-err',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/index.*err_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-index-log',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/index.*log_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-index-trace',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/index.*trace_*.gz',
    '', 'false', 'nginx log', 'hot');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sdl-jwt-cart',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/applog/local_archive/*/*/*/traceweb*/jwt_cart_builder*.gz',
    '', 'false', 'nginx log', 'hot');


insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('OP', 'sweb',
    'applog rotated', 'Applog',
    '/panfs/pan1.be-md.ncbi.nlm.nih.gov/applog_db_tmp/database/logarchive/weblog/local_archive/*/*/*/sweb*/*.gz',
    '', 'false', 'nginx log', 'hot');



-- SRA-8546
insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account, immutable, format, storage_class)
values ('S3', 'sra-pub-metadata-us-east-1', 'Athena metadata', 'Efremov', 'sra-pub-src-1-logs',
    's3_readers', 'false', 'metadata', 'hot');

-- LOGMON-42
insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('S3', 'sra-ca-crun-1', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-nlm-ncbi-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('S3', 'sra-ca-crun-2', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-nlm-ncbi-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('S3', 'sra-ca-crun-3', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-nlm-ncbi-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('S3', 'sra-ca-crun-4', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-nlm-ncbi-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('S3', 'sra-ca-crun-5', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-nlm-ncbi-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('S3', 'sra-ca-crun-6', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-nlm-ncbi-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');


insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-ca-crun-1', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-ca-crun-2', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-ca-crun-3', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-ca-crun-4', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-ca-crun-5', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-ca-crun-6', 'Cold ETL Data',
    'efremov', 'sra-ca-run-logs', 'nih-sra-protected',
    'false', 'ETL + BQS', 'cold', 'private');


insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-1', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-2', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-3', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-4', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-5', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-6', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-7', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-8', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-9', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-10', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-11', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-12', 'Hot ETL Data',
    'efremov', 'sra-pub-logs-1', 'nih-datastore',
    'false', 'ETL + BQS', 'hot', 'public');

insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-pub-crun-13', 'Hot ETL Data',
    'efremov', 'sra-ca-logs-1', 'logmon_private',
    'true', 'ETL + BQS', 'hot', 'public');


insert into buckets (cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    immutable, format, storage_class, scope)
values ('GS', 'sra-ca-logs-1', 'Log bucket',
    'efremov', 'sra-ca-logs-1', 'logmon_private',
    'true', 'GS log', 'hot', 'private');




update buckets set log_format='S3 log' where cloud_provider='S3';
update buckets set log_format='GS log' where cloud_provider='GS';
update buckets set log_format='nginx log' where cloud_provider='OP';
update buckets set log_format='traceweb' where log_bucket like '%traceweb%';

-- LOGMON-31
insert into buckets (
    cloud_provider, bucket_name, description,
    owner, log_bucket, service_account,
    scope, immutable, format, log_format, storage_class)
values (
    'Splunk', 'Cloudian', 'Cloudian',
    'Splunk', 'splunk', 'bearer.txt',
    'public', 'false', 'Cloudian log', 'Cloudian log', 'hot');


.headers on
.mode column
.mode tabs
--.width 20
select * from buckets order by cloud_provider, bucket_name;

.headers off
.mode csv
.output buckets.csv
select * from buckets order by cloud_provider, bucket_name;
