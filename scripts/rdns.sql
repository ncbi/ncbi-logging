-- sqlite3 rdns.db < rdns.sql

.headers on
.bail on
.mode column
.width 40 20

drop table if exists uniq_ips;
create table uniq_ips (line text);
.import /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/uniq_ips/uniq_ips.20200722.json  uniq_ips
select count(*) as uniq_ips_count from uniq_ips;

drop table if exists rdns;
create table rdns as select json_extract(line, '$.remote_ip') as ip, 'Unknown' as domain from uniq_ips;
drop table uniq_ips;

select count(*) as rdns_count_loaded from rdns;
create unique index ip_idx on rdns(ip);

drop table if exists cloud_ips_json;
create table cloud_ips_json(line text);
.import /panfs/traces01.be-md.ncbi.nlm.nih.gov/strides-analytics/cloud_ips.jsonl cloud_ips_json

drop table if exists cloud_ips;
create table cloud_ips as
select json_extract(line, '$.ip') as ip,
       json_extract(line, '$.domain') as domain
    from cloud_ips_json;
select count(*) as cloud_ips from cloud_ips;
create unique index cloud_ip_idx on cloud_ips(ip);

delete from rdns where ip in (select ip from cloud_ips);
select count(*) as rdns_count_cloud_ips_deleted from rdns;
insert into rdns select ip, domain from cloud_ips;
select count(*) as rdns_count_cloud_ips_inserted from rdns;

--drop table if exists rdns_json;
--create table rdns_json (line text);
--.import /home/vartanianmh/ncbi-logging/scripts/rdns.jsonl rdns_json
--select count(*) as rdns_json_count from rdns_json;

--insert into rdns select json_extract(line, '$.ip') as ip, json_extract(line, '$.domain') as domain from rdns_json;
--drop table rdns_json;


delete from rdns where length(ip) > 100 or length(domain) > 100;

.schema
select count(*) as rdns_count from rdns;
--select * from rdns limit 5;

select "running updates";

UPDATE RDNS
SET DOMAIN = 'lrz.de'
WHERE IP = '129.187.44.107';


UPDATE RDNS
SET DOMAIN = 'sdjnptt.net.cn'
WHERE IP = '119.188.52.171';


UPDATE RDNS
SET DOMAIN = 'guangzhou.gd.cn (Guangzhou Beidian Network)'
WHERE IP LIKE '218.19.145.%' OR IP LIKE '218.17%';


UPDATE RDNS
SET DOMAIN = 'osaka-u.ac.jp'
WHERE IP = '133.1.63.238';


UPDATE RDNS
SET DOMAIN = 'u-tokyo.ac.jp'
WHERE IP LIKE '202.175.1%';


UPDATE RDNS
SET DOMAIN = 'cdc.gov (Centers for Disease Control and Prevention)'
WHERE IP LIKE '158.111.%';


UPDATE RDNS
SET DOMAIN = 'Uni-Bielefeld.DE'
WHERE IP LIKE '129.70.51.%';


UPDATE RDNS
SET DOMAIN = 'net.edu.cn (Shanghai Jiaotong University)'
WHERE IP like '202.120.%';


UPDATE RDNS
SET DOMAIN = 'zi.uzh.ch'
WHERE IP = '130.60.102.70';


UPDATE RDNS
SET DOMAIN = 'umab.umd.edu'
WHERE IP LIKE '134.192.%';


UPDATE RDNS
SET DOMAIN = 'chinanet.cn.net (ChinaNetCenter)'
WHERE IP LIKE '222.178.%'
  OR IP LIKE '116.16.%'
  OR IP LIKE '116.17.%'
  OR IP LIKE '116.17.%'
  OR IP LIKE '116.18.%'
  OR IP LIKE '116.19.%'
  OR IP LIKE '116.2%'
  OR IP LIKE '116.30.%'
  OR IP LIKE '116.31.%'
  OR IP LIKE '116.6.%'
  OR IP LIKE '218.9%'
  OR IP LIKE '59.50.85.%'
  OR IP LIKE '119.78.%'
  OR IP LIKE '219.138.%'
  OR IP LIKE '219.139.%'
  OR IP LIKE '219.140.%'
  OR IP LIKE '119.79.%'
  OR IP LIKE '180.160.%'
  OR IP LIKE '180.161.%'
  OR IP LIKE '101.81.%'
  OR IP LIKE '220.243.135.%'
  OR IP LIKE '61.146.%'
  OR IP LIKE '115.218.%'
  OR IP LIKE '115.219.%'
  OR IP LIKE '182.144.%'
  OR IP LIKE '182.145.%'
  OR IP LIKE '182.146.%'
  OR IP LIKE '182.147.%'
  OR IP LIKE '182.148.%'
  OR IP LIKE '182.149.%'
  OR IP LIKE '182.150.%'
  OR IP LIKE '182.151.%'
  OR IP LIKE '183.20.%'
  OR IP LIKE '183.4%'
  OR IP LIKE '183.5%'
  OR IP LIKE '219.129.%'
  OR IP LIKE '14.23.%';

UPDATE RDNS
SET DOMAIN = 'pku.edu.cn'
WHERE IP LIKE '162.105.%';


UPDATE RDNS
SET DOMAIN = 'cstnet.cn (China Science Network)'
WHERE IP LIKE '159.226.%' OR IP LIKE '162.105.%' OR
IP LIKE '202.127.2%' OR IP LIKE '124.16.%' or IP LIKE '124.17.%';


UPDATE RDNS
SET DOMAIN = 'crg.edu'
WHERE IP = '84.88.66.194';


UPDATE RDNS
SET DOMAIN = 'cityu.edu.hk'
WHERE IP LIKE '144.214.37.%';




UPDATE RDNS
SET DOMAIN = 'mdc-berlin.de'
WHERE IP LIKE '141.80.%';


UPDATE RDNS
SET DOMAIN = 'yale.edu'
WHERE IP LIKE '192.31.2.%';


UPDATE RDNS
SET DOMAIN = 'duke.edu'
WHERE IP LIKE '152.16.%';


UPDATE RDNS
SET DOMAIN = 'utexas.edu'
WHERE IP LIKE '206.76.%'
  OR IP LIKE'206.77.%'
OR IP LIKE '129.112.%';


UPDATE RDNS
SET DOMAIN = 'Chinamobile.com'
WHERE IP LIKE '36.1%'
OR IP LIKE '120.221.%'
OR IP LIKE '223.10%'
OR IP LIKE '111.3%'
OR IP LIKE '111.4%'
OR IP LIKE '120.235%'
OR IP LIKE '183.192.%'
OR IP LIKE '183.209.%'
OR IP LIKE '223.64.%'
OR IP LIKE '223.65.%'
OR IP LIKE '223.66.%'
OR IP LIKE '223.67.%'
OR IP LIKE '111.5%';


UPDATE RDNS
SET DOMAIN = 'chinaunicom.cn (China Unicom Henan Province)'
WHERE IP LIKE '119.188.52.%'
  OR IP LIKE '14.204.%'
  OR IP LIKE '14.205.%'
  OR IP LIKE '14.23.%'
  OR IP LIKE '119.4.%'
  OR IP LIKE '119.5.%'
  OR IP LIKE '119.6.%'
  OR IP LIKE '119.7.%'
  OR IP LIKE '116.1%'
  OR IP LIKE '221.206.%'
  OR IP LIKE '61.158.%'
  OR IP LIKE '60.12.%';


UPDATE RDNS
SET DOMAIN = 'cshl.edu'
WHERE IP LIKE '143.48.%';


UPDATE RDNS
SET DOMAIN = 'jlu.edu.cn (Norman Berthune University of Medical Sciences)'
WHERE IP LIKE '202.198.%';


UPDATE RDNS
SET DOMAIN = 'einstein.yu.edu'
WHERE IP LIKE '47.19.%';


UPDATE RDNS
SET DOMAIN = 'jhu.edu (Johns Hopkins Medical Institutions)'
WHERE IP LIKE '162.129.%';

UPDATE RDNS
SET DOMAIN = 'jhuapl.edu (Johns Hopkins Applied Physics Laboratory)'
WHERE IP LIKE '128.244.%';

UPDATE RDNS
SET DOMAIN = 'Chung-ang University'
WHERE IP LIKE '165.194.%';


UPDATE RDNS
SET DOMAIN = 'China Telecom'
WHERE IP LIKE '183.63.%'
OR IP LIKE '171.2%'
OR IP LIKE '119.138.%'
OR IP LIKE '101.110.119.%'
OR IP LIKE '180.16%';



UPDATE RDNS
SET DOMAIN = 'Oklahoma State University'
WHERE IP LIKE '192.31.83.%';


UPDATE RDNS
SET DOMAIN = 'fhcrc.org (Fred Hutchinson Cancer Research Center)'
WHERE IP LIKE '140.107.%';


UPDATE RDNS
SET DOMAIN = 'nus.edu.sg (National University of Singapore)'
WHERE IP LIKE '137.132.%' or ip like '155.69.%';

UPDATE RDNS
SET DOMAIN = 'modares.acir (Tarbiat-Modares University'
WHERE IP LIKE '194.225.1%';


UPDATE RDNS
SET DOMAIN = 'mitre.org (MITRE)'
WHERE IP LIKE '192.80.55.%';


UPDATE RDNS
SET DOMAIN = 'chop.edu (Children''s Hospital of Philadelphia)'
WHERE IP LIKE '159.14.%';


UPDATE RDNS
SET DOMAIN = 'unibas.ch (Universitaet Basel)'
WHERE IP LIKE '131.152.%';


UPDATE RDNS
SET DOMAIN = 'umass.edu (University of Massachusetts Amherst)'
WHERE IP LIKE '205.172.16%';


UPDATE RDNS
SET DOMAIN = 'uga.edu (University of Georgia)'
WHERE IP LIKE '128.192.%';


UPDATE RDNS
SET DOMAIN = 'net.edu.cn (Shanghai Jiaotong University)'
WHERE IP LIKE '202.120.4%';


UPDATE RDNS
SET DOMAIN = 'sdstate.edu (South Dakota State University)'
WHERE IP LIKE '137.216.%';


UPDATE RDNS
SET DOMAIN = 'clemson.edu'
WHERE IP LIKE '205.186.62.%';


UPDATE RDNS
SET DOMAIN = 'harvard.edu (Longwood Medical)'
WHERE IP LIKE '134.174.%';


UPDATE RDNS
SET DOMAIN = 'harvard.edu (Harvard University)'
WHERE IP LIKE '140.247.%';


UPDATE RDNS
SET DOMAIN = 'ucsd.edu (UCSD)'
WHERE IP LIKE '169.228.%';


UPDATE RDNS
SET DOMAIN = 'georgetown.edu (Georgetown University)'
WHERE DOMAIN LIKE '%georgetown.edu%';


UPDATE RDNS
SET DOMAIN = 'msu.edu (Michigan State University)'
WHERE DOMAIN LIKE '%msu.edu%'
or IP LIKE '35.8.%'
or IP LIKE '35.9.%'
or IP LIKE '35.10.%';


UPDATE RDNS
SET DOMAIN = 'dtu.dk (Technical University of Denmark)'
WHERE DOMAIN LIKE '%dtu.dk%';


UPDATE RDNS
SET DOMAIN = 'nig.ac.jp (Japan National Institute of Genetics)'
WHERE IP LIKE '133.39.%' OR IP LIKE '133.103.%';


UPDATE RDNS
SET DOMAIN = 'br.kr (Korea Telecom)'
WHERE IP LIKE '218.15%';


UPDATE RDNS
SET DOMAIN = 'Fundacio Privada Centre de Regulacio Genomica'
WHERE IP LIKE '84.88.66.%';


UPDATE RDNS
SET DOMAIN = 'Paris VII Denis Diderot University'
WHERE IP LIKE '81.194.28.%';


UPDATE RDNS
SET DOMAIN = 'uconn.edu (Connecticut Education Network)'
WHERE IP LIKE '162.221.11.%'
OR IP LIKE '155.37.%';


UPDATE RDNS
SET DOMAIN = 'psu.edu (Penn State University)'
WHERE IP LIKE '128.118.%'
OR IP LIKE '104.38%'
OR IP LIKE '104.39%';


UPDATE RDNS
SET DOMAIN = 'Marine Biological Laboratory'
WHERE IP LIKE '192.152.118.%';


UPDATE RDNS
SET DOMAIN = 'Seoul National University'
WHERE IP LIKE '147.47.%';


UPDATE RDNS
SET DOMAIN = 'Wuhan Hua Zhong Augulture University'
WHERE IP LIKE '211.69.141.%';


UPDATE RDNS
SET DOMAIN = 'ucdavis.edu (University of California Davis)'
WHERE IP LIKE '128.120.%';


UPDATE RDNS
SET DOMAIN = 'uchicago.edu (University of Chicago)'
WHERE IP LIKE '128.135.%'
OR IP LIKE '192.170.%';


UPDATE RDNS
SET DOMAIN = 'hawaii.edu (University of Hawaii)'
WHERE IP LIKE '168.105.%';


UPDATE RDNS
SET DOMAIN = 'nazu.edu (Northern Arizona University)'
WHERE IP LIKE '134.114.%';


UPDATE RDNS
SET DOMAIN = 'georgetown.edu (Georgetown University)'
WHERE IP LIKE '141.161.13.%';


UPDATE RDNS
SET DOMAIN = 'gsk.com (GlaxoSmithKline)'
WHERE IP LIKE '152.51.%';


UPDATE RDNS
SET DOMAIN = 'dartmouth.edu (Dartmouth College)'
WHERE IP LIKE '129.170.%';


UPDATE RDNS
SET DOMAIN = 'roche.com (Hoffmann LaRoche)'
WHERE IP LIKE '196.3.5%';


UPDATE RDNS
SET DOMAIN = 'psc.edu (Pittsburgh Supercomputing Center)'
WHERE IP LIKE '128.182.%';


UPDATE RDNS
SET DOMAIN = 'mit.edu (MIT)'
WHERE IP LIKE '18.4%';


UPDATE RDNS
SET DOMAIN = 'cnnic.ch (Nongkeyuan Science & Technology Information)'
WHERE IP LIKE '210.73.154.%';


UPDATE RDNS
SET DOMAIN = 'njhelath.org (National Jewish Medical and Research Center Corporation)'
WHERE IP LIKE '205.143.248.%';


UPDATE RDNS
SET DOMAIN = 'ucalgary.ca (University of Calgary)'
WHERE IP LIKE '136.159.%';


UPDATE RDNS
SET DOMAIN = 'cannantv.tv (Cable Of The Carolinas Inc)'
WHERE IP LIKE '72.46.72.%';


UPDATE RDNS
SET DOMAIN = 'bostongene.com (BostonGene)'
WHERE IP = '104.130.59.32';


UPDATE RDNS
SET DOMAIN = 'Helmholtz Zentrum Muenchen'
WHERE IP LIKE '146.107.%';


UPDATE RDNS
SET DOMAIN = 'nlm.nih.gov (NLM)'
WHERE IP LIKE '10.10.8%' OR IP LIKE '130.14.%';

UPDATE RDNS
SET DOMAIN = 'ncbi.nlm.nih.gov (NCBI)'
WHERE IP='52.54.203.43' OR IP='35.245.245.236' or IP='35.245.169.105'
or IP='35.245.47.214';

UPDATE RDNS
SET DOMAIN = 'nih.gov (NIH)'
WHERE IP LIKE '128.231.%' OR IP LIKE '156.40.2%'
OR IP LIKE '165.112.%';

UPDATE RDNS
SET DOMAIN = 'AWS (nih.gov)'
WHERE IP in (
'18.204.214.171',
'52.54.203.43',
'18.209.227.133',
'54.80.162.14',
'34.229.225.83'
);

UPDATE RDNS
SET DOMAIN = 'xlate.ufl.edu (University of Florida)'
WHERE IP LIKE '128.227.%';


UPDATE RDNS
SET DOMAIN = 'verizonbusiness.com (Verizon Business)'
WHERE IP LIKE '65.196.%';


UPDATE RDNS
SET DOMAIN = 'level3.com (Level 3 Communications)'
WHERE IP LIKE '4.16.%' or IP LIKE '8.29.8%';


UPDATE RDNS
SET DOMAIN = 'rcn.net (RCN Corporation)'
WHERE IP LIKE '207.237.188.%';


UPDATE RDNS
SET DOMAIN = 'IANA Reserved (ncbi.nlm.nih.gov)'
WHERE IP LIKE '0.%'
  OR IP LIKE '10.%'
  OR IP LIKE '100.64.%'
  OR IP LIKE '127.%'
  OR IP LIKE '169.254.%'
  OR IP LIKE '172.16.%'
  OR IP LIKE '172.17.%'
  OR IP LIKE '172.18.%'
  OR IP LIKE '172.19.%'
  OR IP LIKE '172.20.%'
  OR IP LIKE '172.30.%'
  OR IP LIKE '172.31.%'
  OR IP LIKE '192.0.%'
  OR IP LIKE '192.168.%'
  OR IP LIKE '2002:%'
  OR IP LIKE 'fda3:%';

UPDATE RDNS
SET DOMAIN = 'ovh.com (OVH Cloud)'
WHERE IP LIKE '91.121.%';

UPDATE RDNS
SET DOMAIN = 'sursara.nl (Science Park Watergraafsmeer)'
WHERE IP LIKE '145.100.4%'
OR IP LIKE '145.101.3';


UPDATE RDNS
SET DOMAIN = 'Baylor College of Medicine'
WHERE IP LIKE '128.249.%';


UPDATE RDNS
SET DOMAIN = 'Rothamsted Research Institute'
WHERE IP LIKE '149.155.2%';


UPDATE RDNS
SET DOMAIN = 'meiji.acp.jp (Meiji University)'
WHERE IP LIKE '133.26.%';


UPDATE RDNS
SET DOMAIN = 'rockefeller.edu (The Rockefeller University)'
WHERE IP LIKE '128.85.%'
  OR IP LIKE '129.85.%';


UPDATE RDNS
SET DOMAIN = 'usf.edu (University of South Florida)'
WHERE IP LIKE '131.247.%';


UPDATE RDNS
SET DOMAIN = 'Celgene Corporation'
WHERE IP LIKE '38.99.110.%'
  OR IP LIKE '173.227.5.%';


UPDATE RDNS
SET DOMAIN = 'unam.mx (Universidad Nacional Autonoma de Mexico)'
WHERE IP LIKE '132.247.%';


UPDATE RDNS
SET DOMAIN = 'si.edu (Smithsonian Institution)'
WHERE IP LIKE '160.111.%';


UPDATE RDNS
SET DOMAIN = 'uky.edu (University of Kentucky)'
WHERE IP LIKE '128.163.%';


UPDATE RDNS
SET DOMAIN = 'utoronto.ca (University of Toronto)'
WHERE IP LIKE '142.1.%';


UPDATE RDNS
SET DOMAIN = 'usc.edu (University of Southern California)'
WHERE IP LIKE '128.125.%';


UPDATE RDNS
SET DOMAIN = 'uw.edu (University of Washington)'
WHERE IP LIKE '128.208.%';

UPDATE RDNS
SET DOMAIN='wustl.edu (Washington University)'
WHERE IP LIKE '128.252.%';


UPDATE RDNS
SET DOMAIN = 'sdsu.edu (San Diego State University)'
WHERE IP LIKE '130.191.%';


UPDATE RDNS
SET DOMAIN = 'net.edu.cn (Central South University (Changsha)'
WHERE IP LIKE '122.207.8%';


UPDATE RDNS
SET DOMAIN = 'University of Geneva'
WHERE IP LIKE '129.194.%';


UPDATE RDNS
SET DOMAIN = 'illinois.edu (University of Illinois)'
WHERE IP LIKE '128.174.%';


UPDATE RDNS
SET DOMAIN = 'cchmc.org (Childrens Hospital Medical Center Cincinnati)'
WHERE IP LIKE '205.142.19%';


UPDATE RDNS
SET DOMAIN = 'nd.edu (University of Notre Dame)'
WHERE IP LIKE '129.74.%';


UPDATE RDNS
SET DOMAIN = 'uq.edu.au (University of Queensland)'
WHERE IP LIKE '130.102.%';


UPDATE RDNS
SET DOMAIN = 'nkn.in (Dayalbagh Educational Institute)'
WHERE IP LIKE '14.139.57.%';


UPDATE RDNS
SET DOMAIN = 'uni-bielefeld.de (Universitaet Bielefeld)'
WHERE IP LIKE '129.70.%';


UPDATE RDNS
SET DOMAIN = 'um.edu.mo (University of Macau)'
WHERE IP LIKE '161.64.%';

UPDATE RDNS
SET DOMAIN = 'bell.ca (Bell Canada)'
WHERE IP LIKE '205.200.%';

UPDATE RDNS
SET DOMAIN = 'csiro.au (Commonwealth Scientific and Industrial Research Organisation)'
WHERE IP LIKE '152.83.%';

UPDATE RDNS
SET DOMAIN = 'brown.edu (Brown University)'
WHERE IP LIKE '138.16.%';

UPDATE RDNS
SET DOMAIN = 'ut.ee (University of Tartu, Estonia)'
WHERE IP LIKE '193.40.12.%' OR IP LIKE '193.40.23.%';

UPDATE RDNS
SET DOMAIN = 'nevada.edu (Nevada System of Higher Education)'
WHERE IP LIKE '131.216.%';

UPDATE RDNS
SET DOMAIN = 'IRON (Idaho Regional Optical Network)'
WHERE IP LIKE '74.118.2%';

UPDATE RDNS
SET DOMAIN = 'bit.nl (Keygene N.V.)'
WHERE IP LIKE '213.136.28.%';

UPDATE RDNS
SET DOMAIN = 'verizonbusiness.com (Verizon Business)'
WHERE IP LIKE '108.4%';

UPDATE RDNS
SET DOMAIN = 'comcast.net (Comcast Cable)'
WHERE IP LIKE '69.140.%' or IP LIKE '69.251.%';

UPDATE RDNS
SET DOMAIN = 'toh.ca (The Ottawa Hospital)'
WHERE IP LIKE '204.187.3%';


UPDATE RDNS
SET DOMAIN = 'technion.ac.il (Technion - Technolog Institute)'
WHERE IP LIKE '132.68.%' or IP like '132.69.%';

UPDATE RDNS
SET DOMAIN = 'nchc.org.tw (Taiwan National Center for High-performance computing)'
WHERE IP LIKE '140.110.%';

UPDATE RDNS
SET DOMAIN = 'nscc.sg (Singapore National Supercomputing Centre)'
WHERE IP LIKE '103.72.192.%';

UPDATE RDNS
SET DOMAIN = 'net.edu.cn (University of Science and Technology of China)'
WHERE IP LIKE '211.86.1%';

UPDATE RDNS
SET DOMAIN = 'cuhk.edu.hk (The Chinese University of Hong Kong)'
WHERE IP LIKE '137.189.%';

UPDATE RDNS
SET DOMAIN = 'ccf.org (Cleveland Clinic)'
WHERE IP LIKE '139.137.%';

UPDATE RDNS
SET DOMAIN = 'kreonet.net (Korea Research Environment Open Network)'
WHERE IP LIKE '134.75.%';

UPDATE RDNS
SET DOMAIN = 'unab.cl (Universidad Nacional Andres Bello)'
WHERE IP LIKE '200.27.72.%';

UPDATE RDNS
SET DOMAIN = 'net.edu.cn (Huazhong Agricultural University)'
WHERE IP LIKE '218.199.68.%';

UPDATE RDNS
SET DOMAIN = 'sickkids.ca (The Hospital for Sick Children)'
WHERE IP LIKE '192.75.158.%';

UPDATE RDNS
SET DOMAIN = 'dfn.de (Max-Planck-Institut fuer Pflanzenzuechtungsforschung)'
WHERE IP LIKE '195.37.46.%';

UPDATE RDNS
SET DOMAIN = 'gwu.edu (The George Washington University)'
WHERE IP LIKE '161.253.%';

UPDATE RDNS
SET DOMAIN = 'embl.org (European Molecular Biology Laboratory)'
WHERE IP LIKE '194.94.4%';

UPDATE RDNS
SET DOMAIN = 'kaust.edu.sa (King Abdullah University of Science and Technology)'
WHERE IP LIKE '109.171.137.%';

UPDATE RDNS
SET DOMAIN = 'hanyang.ac.kr (Hanyang Univeristy)'
WHERE IP LIKE '166.104.%';

UPDATE RDNS
SET DOMAIN = 'IDbyDNA.com (IDbyDNA Inc.)'
WHERE IP LIKE '38.142.232.%' or IP LIKE '65.126.68.%';

--UPDATE RDNS
--SET DOMAIN = 'utah.edu (University of Utah)'
--WHERE IP LIKE '2604:c340:%';
UPDATE RDNS
SET DOMAIN = 'utah.edu (University of Utah)'
where IP LIKE '155.101.%' OR IP LIKE '204.99.128.%';

UPDATE RDNS
SET DOMAIN = 'ohsu.edu (Oregon Health & Science University)'
WHERE IP LIKE '137.53.%';

UPDATE RDNS
SET DOMAIN = 'cox.com (Cox Communications Inc.)'
WHERE IP LIKE '98.17%' OR IP LIKE '98.18%';

UPDATE RDNS
SET DOMAIN = 'icr.ac.uk (Institute of Cancer Research)'
WHERE IP LIKE '194.80.195.%';

UPDATE RDNS
SET DOMAIN = 'nic.or.kr (Koeran National Infomation Society Agency)'
WHERE IP LIKE '103.22.220.%' or IP LIKE '103.22.221.%' or
        IP LIKE '103.22.222.%' or IP LIKE '103.22.223.%';

UPDATE RDNS
SET DOMAIN='umich.edu (University of Michigan)'
WHERE IP like '141.214.%';

UPDATE RDNS
SET DOMAIN='uc.edu (University of Cincinnati)'
WHERE IP like '129.137.%';

UPDATE RDNS
SET DOMAIN='monash.edu (Monash University)'
WHERE IP like '118.138.%';

UPDATE RDNS
SET DOMAIN='sdsc.edu (San Diego Supercomputer Center)'
WHERE IP like '132.249.%';

UPDATE RDNS
SET DOMAIN='ebi.ac.uk (European Bioinformatics Institute)'
WHERE IP like '193.62.19%';


UPDATE RDNS
SET DOMAIN='ufpa.br (Universidade Federal do Para)'
WHERE IP like '200.239.%';

UPDATE RDNS
SET DOMAIN='net.edu.cn (Huazhong University Of Science And Technology)'
WHERE IP like '211.67.3%';

UPDATE RDNS
SET DOMAIN='UPenn.edu (University of Pennsylvania)'
WHERE IP like '128.91.%'
OR IP LIKE '130.91.%';

UPDATE RDNS
SET DOMAIN='mcgill.ca (McGill University)'
where IP like '132.216.%';

UPDATE RDNS
SET DOMAIN='uni-koeln.de (University of Cologne)'
where IP like '134.95.%';

UPDATE RDNS
SET DOMAIN='pitt.edu (University of Pittsburgh)'
where IP like '130.49.%';

UPDATE RDNS
SET DOMAIN='tohoku.ac.jp (Tohoku University)'
where IP like '130.34.%';

UPDATE RDNS
SET DOMAIN='uni-bonn.de (University of Bonn)'
where IP like '131.220.%';

UPDATE RDNS
SET DOMAIN='unil.ch (University of Lausanne)'
where IP like '130.223.%';

UPDATE RDNS
SET DOMAIN='NC Biolabs (frontiernet.net)'
where IP like '50.55.%'
or IP LIKE '50.45.%';

UPDATE RDNS
SET DOMAIN='moxnetworks (nantworks.com)'
where IP like '185.130.23%';

UPDATE RDNS
SET DOMAIN='Scripps Research Institute (scipps.edu)'
where IP like '192.26.252.%';

UPDATE RDNS
SET DOMAIN='Cornell University (cornell.edu)'
where IP like '132.236.%';

UPDATE RDNS
SET DOMAIN='University of British Columbia (bc.net)'
where IP like '142.103.%';

UPDATE RDNS
SET DOMAIN='University of Alabama at Birmingham (uab.edu)'
where IP like '164.111.%';

UPDATE RDNS
SET DOMAIN='University of Sheffield (sheffield.ac.uk)'
where IP like '143.167.%';

UPDATE RDNS
SET DOMAIN='Tsinghua University (net.edu.cn)'
where IP like '58.206.%' OR
IP LIKE '219.246.7%'
OR IP LIKE '166.111.%';

UPDATE RDNS
SET DOMAIN='St. Jude Childrens Research Hospital (stjude.org)'
WHERE IP LIKE '192.55.208.%';

UPDATE RDNS
SET DOMAIN='Boston University (bu.edu)'
WHERE IP LIKE '192.12.187.%';

UPDATE RDNS
SET DOMAIN='Alibaba Cloud (alibaba.com)'
WHERE IP LIKE '47.25%'
OR IP LIKE '47.116.%'
OR IP LIKE '101.132.%'
OR IP LIKE '101.133.%';

UPDATE RDNS
SET DOMAIN='KU Leuven (kuleuven.be)'
WHERE IP LIKE '134.58.%';

UPDATE RDNS
SET DOMAIN='University of Hamburg (uni-hamburg.de)'
WHERE IP LIKE '134.100.%';

UPDATE RDNS
SET DOMAIN='Helmholtz Centre for Environmental Research (ufz.de)'
WHERE IP LIKE '141.65.%';

UPDATE RDNS
SET DOMAIN='Edinburgh University (ed.ac.uk)'
WHERE
IP LIKE '192.41.103%' OR
IP LIKE '192.41.104%' OR
IP LIKE '192.41.105%' OR
IP LIKE '192.41.11%' OR
IP LIKE '192.41.12%' OR
IP LIKE '192.41.131.%';

UPDATE RDNS
SET DOMAIN = 'net.edu.cn (Zhejiang University)'
WHERE IP LIKE '222.205.%';

UPDATE RDNS
SET DOMAIN=
'a-star.edu.sg (Singapore Agency For Science, Technology And Research)'
WHERE IP LIKE '103.37.196%';

UPDATE RDNS
SET DOMAIN='net.edu.cn (Shandong Agricultural University)'
WHERE IP LIKE '202.194.13%';

UPDATE RDNS
SET DOMAIN='ucsf.edu (University of California San Francisco)'
WHERE IP LIKE '169.230.%';

UPDATE RDNS
SET DOMAIN='shmtu.edu.cn (Shanghai Maritime University)'
WHERE IP LIKE '58.246.161.1%';

UPDATE RDNS
SET DOMAIN='tu-dresden.de (Dresden University of Technology)'
WHERE IP LIKE '141.30.%';

UPDATE RDNS
SET DOMAIN='memphis.edu'
WHERE IP LIKE '141.22%';

UPDATE RDNS
SET DOMAIN='rr.com (Charter Communications)'
WHERE IP LIKE '76.8%' OR
IP LIKE '76.9%' OR
IP LIKE '68.172.%';

UPDATE RDNS
SET DOMAIN='kumc.edu (University of Kansas Medical Center)'
WHERE IP LIKE '169.147.%';

UPDATE RDNS
SET DOMAIN='ksu.edu (Kansas State University)'
where IP LIKE '129.130.%';

UPDATE RDNS
SET DOMAIN='lanl.gov (Los Alamos National Laboratory)'
WHERE IP LIKE '192.12.184.%';

UPDATE RDNS
SET DOMAIN='cnnic.cn (Beijing Zhongbangyatong Telecom Technology)'
WHERE IP LIKE '124.207.%';

UPDATE RDNS
SET DOMAIN='uohyd.ac.in (University of Hyderabad)'
WHERE IP LIKE '14.139.69.%';

UPDATE RDNS
SET DOMAIN='nebraska.edu (University of Nebraska-Lincoln)'
WHERE IP LIKE '129.93.%';

UPDATE RDNS
SET DOMAIN='lrz.ed (Leibniz Supercomputing Center)'
WHERE IP LIKE '129.187.%';

UPDATE RDNS
SET DOMAIN='ucla.edu'
where IP LIKE '164.67.%';

UPDATE RDNS
SET DOMAIN='rochester.edu (University of Rochester)'
WHERE IP LIKE '128.151.%';

UPDATE RDNS
SET DOMAIN='oath.com (Yahoo/Oath Holdings)'
WHERE IP LIKE '68.180.2%';

UPDATE RDNS
SET DOMAIN='au.dk (Aarhus University)'
WHERE IP LIKE '130.225.3%';

UPDATE RDNS
SET DOMAIN='virginia.edu (University of Virginia)'
WHERE IP LIKE '128.143.%';

UPDATE RDNS
SET DOMAIN='anu.edu.au (Australian National University)'
WHERE IP LIKE '203.0.19.%';

UPDATE RDNS
SET DOMAIN='ethz.ch (Swiss Federal Institute of Technology Zurich)'
WHERE IP LIKE '82.130.8%';

UPDATE RDNS
SET DOMAIN='init7.net (Switzerland Init Seven AG)'
WHERE IP LIKE '81.6.39.%';

UPDATE RDNS
SET DOMAIN='partners.org (Massachusetts General Hospital)'
WHERE IP LIKE '132.183.%';

UPDATE RDNS
SET DOMAIN='merck.com (Merck and Co)'
WHERE IP LIKE '54.4%';

UPDATE RDNS
SET DOMAIN='ucr.edu (University of California Riverside)'
WHERE IP LIKE '138.23.%';

UPDATE RDNS
SET DOMAIN='houstonmethodist.org (The Methodist Hospital)'
where IP LIKE '206.83.48.%'
OR IP LIKE '206.83.5%';

UPDATE RDNS
SET DOMAIN='oist.jp (Okinawa Institute of Science and Technology)'
where IP LIKE '203.181.243.%';

UPDATE RDNS
SET DOMAIN='moffitt.org (H. Lee Moffitt Cancer Center & Research Institute, Inc)'
where IP LIKE '206.81.15$';

UPDATE RDNS
set DOMAIN='en.sbu.ac.ir (Shahid Beheshti University)'
where ip like '94.184.11%';

UPDATE RDNS
set domain='uni-freiburg.edu (University of Freiburg)'
where IP like '132.230.%';

UPDATE RDNS
set domain='shanxitele.com (Shanxi Telecom)'
where ip like '123.174.%';

UPDATE RDNS
set domain='sharcnet.ca (University of Waterloo)'
where ip like '199.241.16%';

UPDATE RDNS
set domain='ucdenver.edu (University of Colorado)'
where ip like '140.226.%';

update rdns
set domain='iu.edu (Indiana University)'
where ip like '149.165.%';

update rdns
set domain='ku.edu (University of Kansas)'
where ip like '129.237.%';

update rdns
set domain='att.com (AT&T)'
where ip like '2600:1700:%';

update rdns
set domain='nih.go.jp (Japan National Institute of Infectious Diseases)'
where ip like '202.241.%';

update rdns
set domain='cam.ac.uk (University of Cambridge)'
where ip like '131.111.%';

UPDATE RDNS
SET DOMAIN = 'googleusercontent.com (GCP)'
WHERE DOMAIN = 'Unknown'
  AND (IP LIKE '66.249.%'
       OR IP LIKE '34.94.%'
       OR IP LIKE '34.6%'
       OR IP LIKE '34.7%'
       OR IP LIKE '34.8%'
       OR IP LIKE '34.9%'
       OR IP LIKE '35.2%'
       OR IP LIKE '2600:1900:%');

UPDATE RDNS
SET DOMAIN = 'googleusercontent.com (GCP)'
WHERE DOMAIN = 'bc.googleusercontent.com (GCP)'
  OR DOMAIN = 'bc.googleusercontent.com';





UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Known Virus Team (GCP)'
WHERE IP = '35.231.190.137';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Novel Virus Team (GCP)'
WHERE IP = '34.73.156.113';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Novel Virus Team (GCP)'
WHERE IP = '35.231.230.192';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Novel Virus Team (GCP)'
WHERE IP = '35.243.208.79';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Novel Virus Team (GCP)'
WHERE IP = '34.73.133.228';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Novel Virus Team (GCP)'
WHERE IP = '34.73.180.208';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Project Testing Team (GCP)'
WHERE IP = '34.73.135.146';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Computational Scaling and Indexing Team (GCP)'
WHERE IP IN ('35.237.148.23',
             '35.196.81.205');


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - Machine Learning Team (GCP)'
WHERE IP = '35.245.82.131';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-rnaseq-clustering'
WHERE IP = '35.196.70.3';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-rnaseq-metadata'
WHERE IP = '35.190.160.141';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-rnaseq-splice-variant'
WHERE IP = '35.196.107.66';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-rnaseq-sre-and-var'
WHERE IP = '35.196.72.10';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-rnaseq-visualization'
WHERE IP = '35.196.10.168';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-rnaseq-test'
WHERE IP = '35.212.2.36';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-rnaseq-gan-gpu'
WHERE IP = '35.196.150.8';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-pangenome-data-upload'
WHERE IP = '35.235.88.60';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - aa-pangenome-data-upload'
WHERE IP = '35.227.101.5';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - vkt-mongodb02'
WHERE IP = '33.245.126.160';


UPDATE RDNS
SET DOMAIN = 'Hackathon (nih.gov)  - vkt-mongodb02'
WHERE IP = '35.245.126.160';

update RDNS
SET DOMAIN = 'googleusercontent.com (GCP) (nih.gov)'
where IP in (
'34.83.135.126',
'35.186.177.152',
'35.230.169.204',
'35.230.183.178',
'35.237.94.203',
'35.245.18.255',
'35.245.189.18',
'35.245.245.236',
'35.245.83.107'
);

update RDNS
SET DOMAIN = 'NCBI Cloud (list.nlm.nih.gov)'
where IP in (
    '10.50.0.101', '10.50.120.37', '10.50.120.38', '10.50.120.39', '10.50.120.41', '10.50.120.42', '10.50.120.43', '10.50.120.45', '10.50.120.46', '10.50.120.47', '10.50.120.48', '10.50.120.54', '10.50.120.57', '10.50.120.58', '10.50.120.60', '10.50.120.61', '10.50.120.90', '10.50.3.94', '10.65.8.12', '10.65.8.16', '10.65.8.17', '10.65.8.18', '10.65.8.23', '10.65.8.29', '10.65.8.3', '10.65.8.37', '10.65.8.4', '10.65.8.42', '10.65.8.43', '10.65.8.49', '10.65.8.51', '10.65.8.52', '10.65.8.60', '10.65.8.65', '10.65.8.66', '10.65.8.70', '10.65.8.9', '10.70.120.12', '10.70.120.14', '10.70.120.16', '10.70.120.18', '10.70.120.19', '10.70.120.22', '10.70.120.27', '10.70.120.29', '10.70.120.74', '10.70.120.77', '10.70.120.79', '10.70.120.90', '10.70.126.101', '10.70.126.11', '10.70.126.12', '10.70.126.13', '10.70.126.14', '10.70.126.6', '10.70.127.129', '10.70.2.199', '10.70.2.240', '10.70.2.41', '10.70.2.83', '10.70.5.53' ,
 '100.26.180.208', '100.26.196.198', '18.204.1.217', '18.207.243.246', '18.207.254.142',
 '18.232.181.196', '18.232.51.233', '3.215.183.229', '3.227.211.222', '3.227.3.154',
 '3.229.142.101', '3.231.228.17', '3.234.210.11', '3.234.244.120', '3.235.148.55',
 '3.83.185.177', '34.204.204.179', '34.236.190.95', '34.239.162.127', '34.239.162.155',
 '34.239.168.37', '34.86.124.154', '34.86.27.235', '34.86.94.122', '35.170.73.177',
 '35.170.75.224', '35.170.76.139', '35.172.100.79', '35.172.121.21', '35.172.223.65',
 '35.173.35.220', '35.174.60.54', '35.175.120.124', '35.175.136.62', '35.175.201.205',
 '35.186.177.30', '35.186.178.40', '35.186.182.213', '35.186.182.226', '35.186.190.145',
 '35.188.230.133', '35.194.65.228', '35.194.77.116', '35.194.93.180', '35.199.32.82',
 '35.199.37.11', '35.199.38.117', '35.199.58.46', '35.199.59.195', '35.221.16.109',
 '35.221.38.227', '35.221.48.58', '35.221.51.21', '35.221.6.207', '35.230.164.30',
 '35.230.168.165', '35.230.172.170', '35.236.196.41', '35.236.200.245', '35.236.201.104',
 '35.236.201.50', '35.236.213.211', '35.236.218.173', '35.236.230.181', '35.236.231.58',
 '35.245.11.48', '35.245.117.192', '35.245.127.204', '35.245.135.215', '35.245.139.88',
 '35.245.140.87', '35.245.162.47', '35.245.168.193', '35.245.177.170', '35.245.179.96',
 '35.245.195.239', '35.245.197.200', '35.245.197.76', '35.245.201.45', '35.245.204.76',
 '35.245.217.213', '35.245.218.83', '35.245.221.45', '35.245.225.20', '35.245.231.141',
 '35.245.232.255', '35.245.237.180', '35.245.27.212', '35.245.29.234', '35.245.42.2',
 '35.245.45.168', '35.245.46.109', '35.245.50.229', '35.245.54.75', '35.245.57.231',
 '35.245.62.67', '35.245.63.74', '35.245.77.7', '35.245.87.54', '35.245.95.245',
 '18.207.224.226', '3.233.226.247', '34.204.176.41', '18.232.73.84', '3.234.243.33',
 '18.207.240.101', '18.207.135.85', '3.233.219.116',
 '34.200.252.124', '35.188.243.156', '35.236.229.250', '35.245.77.223',
 '34.226.204.124',
 '35.172.18.176',
 '104.196.216.152'
);


select domain, count(*) as cnt from rdns group by domain order by cnt desc limit 20;

select substr(ip,0,8) as unknown_sub, count(*)
from rdns
where domain='Unknown'
group by unknown_sub
order by count(*) desc
limit 10;

select substr(ip,0,12) as unknown_sub, count(*)
from rdns
where domain='Unknown'
group by unknown_sub
order by count(*) desc
limit 10;

.width 200
-- gsutil cp /tmp/rdns.jsonl gs://logmon_cfg/rdns.jsonl
select "gsutil cp /tmp/rdns.jsonl gs://logmon_cfg/rdns.jsonl" as hint;
select "bq rm strides_analytics.rdns" as hint2;
select "bq load --source_format=NEWLINE_DELIMITED_JSON --autodetect strides_analytics.rdns gs://logmon_cfg/rdns.jsonl" as hint3;

.headers off
.output /tmp/rdns.jsonl
select distinct json(json_object('ip',ip,'domain',domain)) from rdns;
