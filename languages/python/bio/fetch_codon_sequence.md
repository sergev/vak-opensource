Here is a Python script using Biopython to fetch a codon sequence for a specific protein from a database like GenBank. This example retrieves the coding sequence (CDS) for a protein, specifically the human insulin protein, by accessing its GenBank record and extracting the codon sequence.

### How It Works:
1. **Dependencies**: The script uses Biopython’s `Entrez` module to query NCBI’s GenBank and `SeqIO` to parse the record.
2. **Accession ID**: Uses `NM_000207`, the GenBank accession for human insulin mRNA. You can replace it with another accession ID for a different protein.
3. **Fetching**: Queries GenBank for the record, extracts the coding sequence (CDS) for the specified gene (e.g., "INS" for insulin).
4. **Output**: Returns the codon (nucleotide) sequence and the translated protein sequence for verification.
5. **Error Handling**: Catches issues like invalid accession IDs or missing CDS features.

### Prerequisites:
- Install Biopython: `pip install biopython`
- Replace `"your.email@example.com"` with your actual email (NCBI requires this for Entrez access).

### Example Output (simplified):
For human insulin (`NM_000207`):
- **Codon Sequence**: AUG... (nucleotide sequence of the CDS)
- **Protein Sequence**: MALWM... (translated amino acid sequence)

### Notes:
- You can find accession IDs in databases like UniProt or NCBI Gene. For example, search "insulin" in NCBI’s Gene database to get `NM_000207`.
- If you want a different protein, provide its GenBank accession ID and gene name, and I can tailor the script.
- The script assumes the protein has a defined CDS in the GenBank record. Some records may require additional parsing if multiple CDS exist.
