create table cloud_providers (
    cloud_provder text not null
);
insert into cloud_providers values ('S3');
insert into cloud_providers values ('GS');
insert into cloud_providers values ('nginx');

create table service_accounts (
    service_account text not null,
    owner text
);

insert into service_accounts (service_account) values ('strides-analytics');
insert into service_accounts (service_account) values ('opendata');
insert into service_accounts (service_account) values ('s3_readers');

create table formats (
    format text not null
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

create table scopes (
    scope text not null
);
insert into scopes values ('public');
insert into scopes values ('private');
insert into scopes values ('user-pays');

create table storage_classes (
    storage_class text
);
insert into storage_classes values ('hot');
insert into storage_classes values ('cold');
insert into storage_classes values ('deep');

create table buckets (
    cloud_provider text not null references cloud_providers (cloud_provider),
    bucket_name text not null,
    description text,
    owner text,
    log_bucket text,
    service_account text references service_accounts (service_account) default 'strides-analytics',
    scope text references scopes (scope) default 'public',
    immutable integer default 0,
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
insert into buckets (cloud_provider, bucket_name) values ('S3', 'sra-pub-run-1'); -- only >=3 ?
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

-- https://confluence.ncbi.nlm.nih.gov/display/SRA/SRA+Environments+in+the+Cloud
update buckets
    set owner='nim-nlm-ncbi-sra-opendata',
    log_bucket='sra-pub-run-1-logs',
    immutable=1,
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
    immutable=1,
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

.headers on
.mode column
.mode tabs
--.width 20
select * from buckets order by bucket_name, cloud_provider;
