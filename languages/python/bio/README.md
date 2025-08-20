No single, comprehensive dictionary of codon sequences for all known proteins exists in a centralized, universally accessible form, but there are extensive databases and resources that come close by cataloging protein sequences and their corresponding genetic codes. Here’s a breakdown:

- **Protein and Codon Basics**: Proteins are encoded by DNA or RNA sequences, with codons (triplets of nucleotides) specifying amino acids. The genetic code is nearly universal, so the codon-to-amino-acid mapping is well-established (e.g., AUG codes for methionine). However, the specific codon sequences for proteins vary by organism and gene.

- **Existing Databases**:
  - **UniProt**: The UniProt Knowledgebase (UniProtKB) is a major resource with millions of protein sequences, many linked to their coding DNA/RNA sequences. You can access nucleotide sequences for proteins via cross-references to databases like EMBL/GenBank/DDBJ. It’s not a "dictionary" of codons per se but provides raw sequence data.
  - **GenBank/ENA/DDBJ**: These nucleotide sequence databases store DNA/RNA sequences, including coding regions (CDS) for proteins. You can extract codon sequences by analyzing the CDS annotations.
  - **Ensembl**: Offers genomic data with coding sequences for proteins across many species. You can retrieve codon sequences for genes via their API or web interface.
  - **PDB (Protein Data Bank)**: Focuses on protein structures but often includes links to coding sequences. Less comprehensive for codon sequences than UniProt or GenBank.
  - **Specialized Databases**: Some organism-specific databases (e.g., FlyBase for Drosophila, SGD for yeast) provide detailed codon-to-protein mappings for model organisms.

- **Codon Usage Databases**: Resources like the **Codon Usage Database** (Kazusa) or **HIVE-CUT** provide codon usage tables, showing which codons are preferentially used for each amino acid in different organisms. These aren’t protein-specific but help understand codon biases in protein-coding genes.

- **Challenges**:
  - **Scale**: There are millions of known proteins across species, and new ones are discovered regularly. A single "dictionary" would be massive and constantly updated.
  - **Context**: Codon sequences depend on the organism, as codon usage varies (e.g., humans vs. bacteria). A universal dictionary would need to account for this.
  - **Access**: Raw sequence data exists, but extracting codon sequences for all proteins requires computational tools to parse databases like GenBank or Ensembl.

- **Practical Approach**: To get codon sequences for a specific protein, you’d typically:
  1. Search UniProt for the protein sequence.
  2. Find the associated gene’s coding sequence in GenBank or Ensembl.
  3. Extract the codon sequence by aligning the nucleotide sequence to the protein’s amino acid sequence.
