#!/usr/bin/env python3

from Bio import Entrez
import time

# Set email for Entrez (NCBI requirement)
Entrez.email = "your.email@example.com"  # Replace with your email

def fetch_all_accessions(db="nucleotide", query="human[Organism]", max_results=1000):
    """
    Fetch accession numbers from Entrez database for a given query.

    Parameters:
    - db: Entrez database to search (e.g., 'nucleotide', 'protein')
    - query: Search term (e.g., 'human[Organism]', 'insulin')
    - max_results: Maximum number of accessions to retrieve
    """
    try:
        # Step 1: Perform an initial search to get the total count
        handle = Entrez.esearch(db=db, term=query, retmax=0, usehistory="y")
        search_results = Entrez.read(handle)
        handle.close()

        total_count = int(search_results["Count"])
        print(f"Total records found: {total_count}")
        if total_count == 0:
            return []

        # Limit results to max_results
        retmax = min(max_results, total_count)
        accessions = []

        # Step 2: Fetch results in batches using WebEnv and QueryKey
        batch_size = 500  # Entrez limits retmax to 10,000 per request
        for start in range(0, retmax, batch_size):
            try:
                handle = Entrez.esearch(
                    db=db,
                    term=query,
                    retstart=start,
                    retmax=batch_size,
                    webenv=search_results["WebEnv"],
                    query_key=search_results["QueryKey"]
                )
                batch_results = Entrez.read(handle)
                handle.close()

                # Fetch summary for IDs to get accession numbers
                id_list = batch_results["IdList"]
                if id_list:
                    summary_handle = Entrez.esummary(db=db, id=",".join(id_list))
                    summary_results = Entrez.read(summary_handle)
                    summary_handle.close()

                    # Extract accession numbers, handling variable response structure
                    for doc in summary_results:
                        try:
                            accession = doc.get("AccessionVersion", doc.get("Caption", "Unknown"))
                            if accession != "Unknown":
                                accessions.append(accession)
                                print(f"Accession: {accession}")
                        except (TypeError, KeyError) as e:
                            print(f"Error parsing summary for ID {doc.get('Id', 'unknown')}: {str(e)}")
                            continue

                # Respect NCBI rate limits (3 requests/sec without API key)
                time.sleep(0.34)

            except Exception as e:
                print(f"Error fetching batch at start {start}: {str(e)}")
                continue

        print(f"\nTotal accessions retrieved: {len(accessions)}")
        return accessions

    except Exception as e:
        print(f"Failed to fetch accessions: {str(e)}")
        return []

# Example: Fetch accession numbers for human nucleotide sequences
if __name__ == "__main__":
#   query = "human[Organism] AND insulin[Gene Name]"  # Example query
    query = "human[Organism]"  # Example query
    max_results = 100  # Limit for demonstration
    accession_list = fetch_all_accessions(db="nucleotide", query=query, max_results=max_results)
