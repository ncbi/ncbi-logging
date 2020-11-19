#!/bin/bash

# shellcheck source=strides_env.sh
. ./strides_env.sh

export GOOGLE_APPLICATION_CREDENTIALS=$HOME/sandbox-blast-847af7ab431a.json
gcloud config set account 1008590670571-compute@developer.gserviceaccount.com
export CLOUDSDK_CORE_PROJECT="ncbi-sandbox-blast"


bq rm -f strides_analytics.public_fix

bq mk \
    --table \
    strides_analytics.public_fix \
    Run:STRING,ReleaseDate:STRING,LoadDate:STRING,spots:STRING,bases:STRING,spots_with_mates:STRING,avgLength:STRING,size_MB:STRING,AssemblyName:STRING,download_path:STRING,Experiment:STRING,LibraryName:STRING,LibraryStrategy:STRING,LibrarySelection:STRING,LibrarySource:STRING,LibraryLayout:STRING,InsertSize:STRING,InsertDev:STRING,Platform:STRING,Model:STRING,SRAStudy:STRING,BioProject:STRING,Study_Pubmed_id:STRING,ProjectID:STRING,Sample:STRING,BioSample:STRING,SampleType:STRING,TaxID:STRING,ScientificName:STRING,SampleName:STRING,g1k_pop_code:STRING,source:STRING,g1k_analysis_group:STRING,Subject_ID:STRING,Sex:STRING,Disease:STRING,Tumor:STRING,Affection_Status:STRING,Analyte_Type:STRING,Histological_Type:STRING,Body_Site:STRING,CenterName:STRING,Submission:STRING,dbgap_study_accession:STRING,Consent:STRING,RunHash:STRING,ReadHash:STRING

bq load --source_format=CSV \
    strides_analytics.public_fix \
    "$PANFS/public_fix.csv"


export CLOUDSDK_CORE_PROJECT="ncbi-logmon"
gcloud config set account 253716305623-compute@developer.gserviceaccount.com

bq rm -f strides_analytics.public_fix

bq mk \
    --table \
    strides_analytics.public_fix \
    Run:STRING,ReleaseDate:STRING,LoadDate:STRING,spots:STRING,bases:STRING,spots_with_mates:STRING,avgLength:STRING,size_MB:STRING,AssemblyName:STRING,download_path:STRING,Experiment:STRING,LibraryName:STRING,LibraryStrategy:STRING,LibrarySelection:STRING,LibrarySource:STRING,LibraryLayout:STRING,InsertSize:STRING,InsertDev:STRING,Platform:STRING,Model:STRING,SRAStudy:STRING,BioProject:STRING,Study_Pubmed_id:STRING,ProjectID:STRING,Sample:STRING,BioSample:STRING,SampleType:STRING,TaxID:STRING,ScientificName:STRING,SampleName:STRING,g1k_pop_code:STRING,source:STRING,g1k_analysis_group:STRING,Subject_ID:STRING,Sex:STRING,Disease:STRING,Tumor:STRING,Affection_Status:STRING,Analyte_Type:STRING,Histological_Type:STRING,Body_Site:STRING,CenterName:STRING,Submission:STRING,dbgap_study_accession:STRING,Consent:STRING,RunHash:STRING,ReadHash:STRING

bq load --source_format=CSV \
    strides_analytics.public_fix \
    "$PANFS/public_fix.csv"

bq load \
        --source_format=NEWLINE_DELIMITED_JSON \
        --autodetect \
        strides_analytics.iplookup \
        "gs://logmon_cfg/iplookup.jsonl.gz"


bq load \
        --source_format=NEWLINE_DELIMITED_JSON \
        --autodetect \
        strides_analytics.rdns\
        "gs://logmon_cfg/rdns.jsonl"

bq load \
    --source_format=CSV \
    strides_analytics.ip2location \
    /home/vartanianmh/IP2LOCATION-LITE-DB11.CSV.gz \
    ip_from:INTEGER,ip_to:INTEGER,country_code:STRING,country_name:STRING,region_name:STRING,city_name:STRING,latitude:STRING,longitude:STRING,zip_code:STRING,time_zone:STRING
