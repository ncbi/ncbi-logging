delete from public;
\copy public FROM public_fix.csv WITH CSV HEADER QUOTE AS '"'

UPDATE accs SET acc_desc=scientificname FROM public
    WHERE
    run=acc OR
    run=split_part(acc,'.',1);

UPDATE accs SET acc_desc='Non-redundant protein sequence'
    WHERE acc_part='nr' or acc LIKE '%nr_v5%';
UPDATE accs SET acc_desc='Nucleotide collection' WHERE acc_part='nt'
    OR acc LIKE '%nt_v5%';
UPDATE accs SET acc_desc='Reference proteins' WHERE acc_part='refseq_protein'
    OR acc LIKE '%refseq_protein_v5%';
UPDATE accs SET acc_desc='Model Organisms' WHERE acc_part='landmark';
UPDATE accs SET acc_desc='UniProtKB/Swiss-Prot' WHERE acc_part='swissprot' or
    acc LIKE '%swissprot_v5%';
UPDATE accs SET acc_desc='Patented protein sequence' WHERE acc_part='pat';
UPDATE accs SET acc_desc='Protein Data Bank' WHERE acc_part='pdb' or
    acc LIKE '%pdb_v5%';
UPDATE accs SET acc_desc='Metagenomic proteins' WHERE acc_part='env_nr';
UPDATE accs SET acc_desc='Transcriptome Shotgun Assembly proteins' WHERE
    acc_part='tsa_nr';
UPDATE accs SET acc_desc='Whole-genome shotgun contigs' WHERE acc_part='wgs';
UPDATE accs SET acc_desc='Reference Genomic Sequences' WHERE acc_part='genomic';
UPDATE accs SET acc_desc='Reference Sequences' WHERE acc_part='refseq';
UPDATE accs SET acc_desc='Genomic assemblies (Human/Mouse)'
    WHERE acc_part='GPIPE';
UPDATE accs SET acc_desc='Genomic assembly (Human)'
    WHERE acc LIKE '%GCF_000001405%';
UPDATE accs SET acc_desc='Genomic assembly (Mouse)'
    WHERE acc LIKE '%GCF_000001635%';
UPDATE accs SET acc_desc='Refseq Protein' WHERE acc LIKE '%refseq_protein%';
UPDATE accs SET acc_desc='Landmark' WHERE acc LIKE '%landmark_v5%';
UPDATE accs SET acc_desc='Viral RefSeq genomes'
    WHERE acc LIKE '%ref_viruses_rep_genomes_v5%';
UPDATE accs SET acc_desc='Reference RNA sequences'
    WHERE acc LIKE '%refseq_rna_v5%';
UPDATE accs SET acc_desc='Refseq viroids representative genomes'
    WHERE acc LIKE '%ref_viroids_rep_genomes_v5%';
UPDATE accs SET acc_desc='rRNA_typestrains/prokaryotic_16S_ribosomal_RNA'
    WHERE acc LIKE '%_16S_%';
UPDATE accs SET acc_desc='Validated viral genomes' WHERE acc LIKE
    '%NCBI_VIV_%';
UPDATE accs SET acc_desc='Taxonomy BLAST databases' WHERE acc LIKE
    '%taxdb%';
UPDATE accs set acc_desc='Blast DB Manifest' where acc='blastdb-manifest';
UPDATE accs set acc_desc='Blast DB Metadata' where acc like 'fuse%';
UPDATE accs set acc_desc='Blast DB Latest' where acc like 'latest-dir%';
UPDATE accs set acc_desc='Conserved Domain Database' where acc like 'cdd%';

