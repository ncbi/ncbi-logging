#!/usr/bin/env bash

# shellcheck source=/home/vartanianmh/strides/strides_env.sh
. "$HOME/strides/strides_env.sh"

tar -cvaf "sra_meta.$DATE.tgz" ./*.csv

# 1.2M
wget -q -a rna_seq.log -O - 'https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?save=efetch&db=sra&rettype=runinfo&term=(((("transcriptomic"[Source]) OR "est"[Strategy]) OR "fl cdna"[Strategy]) OR "rna seq"[Strategy]) OR "cdna"[Selection]' | grep -v "^$"  > rna-seq.csv

# 353K
wget -q -a foodborne.log -O - 'https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?save=efetch&db=sra&rettype=runinfo&term=(Salmonella enterica[orgn] OR Escherichia coli[orgn] OR Shigella[orgn] OR Listeria monocytogenes[orgn] OR Campylobacter coli[orgn] OR Campylobacter jejuni[orgn]) AND "genomic"[Source] AND "strategy whole genome sequencing"[Properties]' | grep -v "^$"  > foodborne.csv

# 591K
wget -q -a carb.log -O - 'https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?save=efetch&db=sra&rettype=runinfo&term=(Clostridium difficile[orgn] OR Escherichia coli[orgn] OR Shigella[orgn] OR Acinetobacter baumannii[orgn] OR Pseudomonas aeruginosa[orgn] OR Enterobacter[orgn] OR Klebsiella pneumoniae[orgn] OR Neisseria gonorrhoeae OR Campylobacter coli[orgn] OR Campylobacter jejuni[orgn] OR Enterococcus faecium[orgn] OR Enterococcus faecalis[orgn] OR Pseudomonas aeruginosa[orgn] OR Salmonella enterica[orgn] OR Staphylococcus aureus[orgn] OR Streptococcus pneumoniae[orgn] OR Mycobacterium tuberculosis[orgn] OR Streptococcus agalactiae[orgn] OR Streptococcus pyogenes[orgn]) AND "genomic"[Source] AND "strategy whole genome sequencing"[Properties]' | grep -v "^$"  > carb.csv

#1.2M
wget -q -a metagenomes.log -O - 'https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?save=efetch&db=sra&rettype=runinfo&term="metagenomic"[Source]' | grep -v "^$"  > metagenomes.csv

#49K
wget -q -a viral.log -O - 'https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?save=efetch&db=sra&rettype=runinfo&term="viruses"[orgn]' | grep -v "^$"  > viral.csv

#6.5M
cp public_fix.csv "public_fix.csv.bak"
wget -q -a public.log -O - 'https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?save=efetch&db=sra&rettype=runinfo&term="public"[Access]' | grep -v "^$" | ~/strides/public_fix.py  > public_fix.csv 2>> public_fix.log
LC=$(wc -l < public_fix.csv)
if [ "$LC" -lt 5000000 ]; then
    echo "Short file: $LC"
    cp "public_fix.csv.bak"  public_fix.csv
fi

date
