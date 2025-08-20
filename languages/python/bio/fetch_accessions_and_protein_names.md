Here is a Biopython script that searches the Entrez database for accession numbers matching a specific query (e.g., for a gene, organism, or term) within a manageable scope, as Entrez limits result sizes and requires specific queries. This script will focus on the **Nucleotide** database and retrieve accession numbers for a user-defined search term, with pagination to handle large results.

### How It Works:
1. **Dependencies**: Uses Biopython’s `Entrez` module to query NCBI’s Entrez system.
2. **Query**: Searches the Nucleotide database (`db="nucleotide"`) with a user-defined query (e.g., `"human[Organism] AND insulin[Gene Name]"`).
3. **Pagination**: Handles large result sets by fetching in batches (`batch_size=500`) using Entrez’s `WebEnv` and `QueryKey` for efficiency.
4. **Accession Extraction**: Uses `esummary` to retrieve accession numbers (`AccessionVersion`) for each record.
5. **Rate Limiting**: Includes a `time.sleep(0.34)` to respect NCBI’s 3 requests/second limit (without an API key).
6. **Output**: Prints each accession number and returns a list of them.

### Prerequisites:
- Install Biopython: `pip install biopython`
- Replace `"your.email@example.com"` with your actual email (NCBI requirement).
- Optionally, get an NCBI API key for higher rate limits (10 requests/second) and add it with `Entrez.api_key = "your_api_key"`.

### Example Output:
For the query `"human[Organism] AND insulin[Gene Name]"`:
```
Total records found: 1234
Accession: NM_000207.3
Accession: NM_001185097.2
...
Total accessions retrieved: 100
```

### Important Notes:
- **Scope Limitation**: The Entrez database is vast (e.g., Nucleotide has >1 billion records). Fetching *all* accessions is infeasible due to API limits and computational constraints. This script uses a query to narrow the scope (e.g., insulin-related sequences in humans).
- **Customization**: Modify the `query` parameter to target specific records (e.g., `"Escherichia coli[Organism]"`, `"BRCA1[Gene Name]"`). Use NCBI’s search syntax (see [NCBI Search Fields](https://www.ncbi.nlm.nih.gov/books/NBK49540/)).
- **Max Results**: The script limits to `max_results` (default 1000) to avoid overwhelming the system. Adjust as needed, but large values may take time.
- **Database Choice**: This targets the Nucleotide database. Change `db` to `"protein"` or others (e.g., `"gene"`, `"biosample"`) for different data types.
- **Error Handling**: Handles network issues or invalid queries, but you may need to debug specific errors (e.g., query syntax).

### Alternatives:
- For all accessions in a specific database, you’d need to query broad terms (e.g., `"all[Filter]"`) and iterate over all results, but this could take days/weeks and hit API limits. Consider downloading NCBI’s pre-compiled datasets (e.g., FTP dumps) for offline processing.
- Use NCBI’s EDirect tools for more efficient bulk queries if Biopython is too slow.

Update:

To print both accession numbers and protein names from the Entrez Nucleotide database, we need to modify the previous script to extract the protein name associated with each record. In the Nucleotide database, protein names are often found in the `CDS` (coding sequence) feature of GenBank records, under qualifiers like `product` or `protein_id`. Since fetching full GenBank records for each accession can be slow, we’ll first try to extract protein names from the `esummary` response (e.g., via the `Title` field, which often includes the protein name) and fall back to parsing the full GenBank record if needed. The script will still use Biopython and handle pagination for large result sets.

### Example Output:
For the query `"human[Organism] AND insulin[Gene Name]"`:
```
Total records found: 1234
Accession: NM_000207.3, Protein Name: insulin preproprotein
Accession: NM_001185097.2, Protein Name: insulin isoform 1 precursor
...
Total records retrieved: 100
```

### Notes:
- **Database**: The script targets the Nucleotide database (`db="nucleotide"`), as it aligns with the previous request to fetch codon sequences. If you want protein names from the Protein database, change `db="protein"` and adjust parsing (e.g., use `Name` or `Title` fields directly).
- **Performance**: Fetching full GenBank records for protein names is slow due to additional API calls. For large `max_results`, consider sticking to `esummary` if `Title` provides sufficient information.
- **Query Specificity**: The example query `"human[Organism] AND insulin[Gene Name]"` is specific to insulin in humans. Modify the `query` for other genes or organisms (e.g., `"BRCA1[Gene Name]"`, `"Escherichia coli[Organism]"`).
- **Limitations**: Some records may lack a clear protein name in `Title` or `CDS` features, resulting in "No protein name found in CDS". You can refine the query or database to reduce such cases.
- **Alternative**: If you need protein names for all records in a database, NCBI’s FTP datasets (e.g., GenBank or RefSeq) are better for bulk processing, as Entrez API has limits.
