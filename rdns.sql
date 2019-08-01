update rdns set domain='lrz.de' where ip='129.187.44.107';
update rdns set domain='sdjnptt.net.cn' where ip='119.188.52.171';
update rdns set domain='guangzhou.gd.cn' where ip='218.19.145.19';
update rdns set domain='osaka-u.ac.jp' where ip='133.1.63.238';
update rdns set domain='u-tokyo.ac.jp' where ip like '202.175.1%';
update rdns set domain='cdc.gov' where ip like '158.111.%';
update rdns set domain='Uni-Bielefeld.DE' where ip like '129.70.51.%';
update rdns set domain='net.edu.cn' where ip='202.120.44.184';
update rdns set domain='net.edu.cn' where ip='202.120.45.162';
update rdns set domain='zi.uzh.ch' where ip='130.60.102.70';
update rdns set domain='umab.umd.edu' where ip like '134.192.%';
update rdns set domain='cstnet.cn' where ip='202.127.20.232';
update rdns set domain='cstnet.cn' where ip like '124.16.%';
update rdns set domain='chinanet.cn.net'
where ip like '222.178.%' or ip like '218.9%' or ip like '59.50.85.%';
update rdns set domain='pku.edu.cn' where ip like '162.105.%';
update rdns set domain='cstnet.cn' where ip='159.226.67.133';
update rdns set domain='crg.edu' where ip='84.88.66.194';
update rdns set domain='cityu.edu.hk' where ip like '144.214.37.%';
update rdns set domain='ChinaNetCenter' where ip like '220.243.135.%';
update rdns set domain='ChinaNetCenter' where ip like '220.243.136.%';
update rdns set domain='mdc-berlin.de' where ip like '141.80.%';
update rdns set domain='yale.edu' where ip like '192.31.2.%';
update rdns set domain='duke.edu' where ip like '152.16.%';
update rdns set domain='utexas.edu' where ip like '206.76.%' or
    ip like'206.77.%';
update rdns set domain='Chinamobile.com' where ip like '120.221.%';
update rdns set domain='Chinamobile.com' where ip like '36.1%';
update rdns set domain='China Unicom' where ip like '119.188.52.%'
    or ip like '14.204.%' or ip like '14.205.%'
    or ip like '60.12.%';
update rdns set domain='cshl.edu' where ip like '143.48.%';
update rdns set domain='jlu.edu.cn (Norman Berthune University of Medical Sciences)' where ip like '202.198.%';
update rdns set domain='einstein.yu.edu' where ip like '47.19.%';
update rdns set domain='jhu.edu (Johns Hopkins Medical Institutions)'
    where ip like '162.129.%';
update rdns set domain='Chung-ang University' where ip like '165.194.%';
update rdns set domain='China Telecom' where ip like '183.63.%';
update rdns set domain='China Telecom' where ip like '171.2%';
update rdns set domain='Oklahoma State University' where ip like '192.31.83.%';
update rdns set domain='China Telecom' where ip like '119.138.%';
update rdns set domain='fhcrc.org (Fred Hutchinson Cancer Research Center)'
    where ip like '140.107.%';
update rdns set domain='nus.edu.sg (National University of Singapore)'
    where ip like '137.132.%';
update rdns set domain='Tarbiat-Modares University (modares.ac.ir)'
    where domain='unknown' and ip like '194.225.1%';
update rdns set domain='MITRE (mitre.org)' where domain='unknown'
    and ip like '192.80.55.%';
update rdns set domain='Children''s Hospital of Philadelphia (chop.edu)'
    where ip like '159.14.%';

update rdns set domain='Universitaet Basel (unibas.ch)'
    where ip like '131.152.%';
update rdns set domain='University of Massachusetts Amherst (umass.edu)'
    where ip like '205.172.16%';
update rdns set domain='University of Georgia (uga.edu)'
    where ip like '128.192.%';
update rdns set domain='Shanghai Jiaotong University (net.edu.cn)'
    where ip like '202.120.4%';
update rdns set domain='South Dakota State University (sdstate.edu)'
    where ip like '137.216.%';

update rdns set domain='clemson.edu' where ip like '205.186.62.%';
update rdns set domain='Longwood Medical (harvard.edu)'
where ip like '134.174.%';

update rdns set domain='poznan.pl' where ip like '150.%';
update rdns set domain='ucsd.edu (UCSD) ' where ip like '169.228.%';

update rdns set domain='georgetown.edu (Georgetown University)'
where domain like '%georgetown.edu%';
update rdns set domain='msu.edu (Michigan State University)'
where domain like '%msu.edu%';

update rdns set domain='dtu.dk (Technical University of Denmark)'
where domain like '%dtu.dk%';

update rdns set domain='Japan National Institute of Genetics (nig.ac.jp)'
where ip like '133.39.%';

update rdns set domain='Korea Telecom (or.kr)'
where ip like '218.15%';

update rdns set domain='Fundacio Privada Centre de Regulacio Genomica'
where ip like '84.88.66.%';

update rdns set domain='Paris VII Denis Diderot University'
where ip like '81.194.28.%';

update rdns set domain='Connecticut Education Network (uconn.edu)'
where ip like '162.221.11.%';

update rdns set domain='Penn State University (psu.edu)'
where ip like '128.118.%';

update rdns set domain='Marine Biological Laboratory'
where ip like '192.152.118.%';

update rdns set domain='Seoul National University'
where ip like '147.47.%';

update rdns set domain='Wuhan Hua Zhong Augulture University'
where ip like '211.69.141.%';

update rdns set domain='University of California, Davis (ucdavis.edu)'
where ip like '128.120.%';

update rdns set domain='University of Chicago (uchiacgo.edu)'
where ip like '128.135.%';
update rdns set domain='University of Hawaii (hawaii.edu)'
where ip like '168.105.%';

update rdns set domain='BostonGene (bostongene.com)' where ip = '104.130.59.32';

update rdns set domain='Helmholtz Zentrum Muenchen' where ip like '146.107.%';

update rdns set domain='nlm.nih.gov (NIH.gov)' where domain='unknown' and
    ip like '10.10.8%' or
    ip like '130.14.%';
update rdns set domain='nih.gov (NIH.gov)' where domain='unknown' and
    ip like '128.231.%' or
    ip like '156.40.2%';
update rdns set domain='xlate.ufl.edu (UFL.edu)' where domain='unknown' and
    ip like '128.227.%';

update rdns set domain='IANA Reserved' where
    ip like '0.%' or
    ip like '10.%' or
    ip like '127.%' or
    ip like '169.254.%' or
    ip like '172.16.%' or
    ip like '192.0.%' or
    ip like '192.168.%';


update rdns set domain='Science Park Watergraafsmeer (surfsara.nl)'
where ip like '145.100.4%';
update rdns set domain='Baylor College of Medicine'
where ip like '128.249.%';
update rdns set domain='Rothamsted Research Institute'
where ip like '149.155.2%';
update rdns set domain='Meiji University (meiji.ac.jp)'
where ip like '133.26.%';
update rdns set domain='The Rockefeller University (rockefeller.edu)'
where ip like '128.85.%' or ip like '129.85.%';
update rdns set domain='University of South Florida (usf.edu)'
where ip like '131.247.%';
update rdns set domain='Celgene Corporation'
where ip like '38.99.110.%';

update rdns set domain='googleusercontent.com (GCP)'
where domain='unknown' and ( ip like '66.249.%' or ip like '35.2%');
update rdns set domain='googleusercontent.com (GCP)'
where domain='bc.googleusercontent.com (GCP)' or
    domain='bc.googleusercontent.com';

update rdns set domain='Hackathon - Known Virus Team (GCP)' where ip='35.231.190.137';
update rdns set domain='Hackathon - Novel Virus Team (GCP)' where ip='34.73.156.113';
update rdns set domain='Hackathon - Novel Virus Team (GCP)' where ip='35.231.230.192';
update rdns set domain='Hackathon - Novel Virus Team (GCP)' where ip='35.243.208.79';
update rdns set domain='Hackathon - Novel Virus Team (GCP)' where ip='34.73.133.228';
update rdns set domain='Hackathon - Novel Virus Team (GCP)' where ip='34.73.180.208';
update rdns set domain='Hackathon - Project Testing Team (GCP)' where ip='34.73.135.146';
update rdns set domain='Hackathon - Computational Scaling and Indexing Team (GCP)' where ip in ('35.237.148.23', '35.196.81.205');
update rdns set domain='Hackathon - Machine Learning Team (GCP)' where ip='35.245.82.131';

update rdns set domain='Hackathon - aa-rnaseq-clustering' where ip='35.196.70.3';
update rdns set domain='Hackathon - aa-rnaseq-metadata' where ip='35.190.160.141';
update rdns set domain='Hackathon - aa-rnaseq-splice-variant' where ip='35.196.107.66';
update rdns set domain='Hackathon - aa-rnaseq-sre-and-var' where ip='35.196.72.10';
update rdns set domain='Hackathon - aa-rnaseq-visualization' where ip='35.196.10.168';
update rdns set domain='Hackathon - aa-rnaseq-test' where ip='35.212.2.36';
update rdns set domain='Hackathon - aa-rnaseq-gan-gpu' where ip='35.196.150.8';
update rdns set domain='Hackathon - aa-pangenome-data-upload' where ip='35.235.88.60';
update rdns set domain='Hackathon - aa-pangenome-data-upload' where ip='35.227.101.5';
update rdns set domain='Hackathon - vkt-mongodb02' where ip='33.245.126.160';
update rdns set domain='Hackathon - vkt-mongodb02' where ip='35.245.126.160';



select ip, sum(bytes)
    from sra_sessions
    where domain='unknown'
    group by ip
    order by sum(bytes) desc
    limit 10;

