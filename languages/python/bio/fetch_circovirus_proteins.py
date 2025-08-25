#!/usr/bin/env python3

from Bio import Entrez, SeqIO
import time

# Set email for Entrez (NCBI requirement)
Entrez.email = "your.email@example.com"  # Replace with your email

def fetch_accessions_and_protein_names(db, query, max_results=1000):
    """
    Fetch accession numbers and protein names from Entrez database for a given query.

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
        results = []

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

                # Fetch summary for IDs to get accession numbers and titles
                id_list = batch_results["IdList"]
                if id_list:
                    summary_handle = Entrez.esummary(db=db, id=",".join(id_list))
                    summary_results = Entrez.read(summary_handle)
                    summary_handle.close()

                    # Extract accession numbers and protein names
                    for doc in summary_results:
                        try:
                            accession = doc.get("AccessionVersion", doc.get("Caption", "Unknown"))
                            # Try to get protein name from Title field
                            protein_name = doc.get("Title", "Unknown")

                            # If Title is vague, fetch full GenBank record for CDS product
                            if protein_name == "Unknown" or "complete cds" in protein_name.lower():
                                try:
                                    gb_handle = Entrez.efetch(db=db, id=doc["Id"], rettype="gb", retmode="text")
                                    gb_record = SeqIO.read(gb_handle, "genbank")
                                    gb_handle.close()
                                    for feature in gb_record.features:
                                        if feature.type == "CDS" and "product" in feature.qualifiers:
                                            protein_name = feature.qualifiers["product"][0]
                                            break
                                    else:
                                        protein_name = "No protein name found in CDS"
                                except Exception as e:
                                    protein_name = f"Failed to fetch GenBank record: {str(e)}"

                            if accession != "Unknown":
                                results.append({"accession": accession, "protein_name": protein_name})
                                print(f"Accession: {accession}, Protein Name: {protein_name}")
                        except (TypeError, KeyError) as e:
                            print(f"Error parsing summary for ID {doc.get('Id', 'unknown')}: {str(e)}")
                            continue

                # Respect NCBI rate limits (3 requests/sec without API key)
                time.sleep(0.34)

            except Exception as e:
                print(f"Error fetching batch at start {start}: {str(e)}")
                continue

        print(f"\nTotal records retrieved: {len(results)}")
        return results

    except Exception as e:
        print(f"Failed to fetch records: {str(e)}")
        return []

# Example: Fetch accession numbers and protein names for circovirus
if __name__ == "__main__":
    query = "circovirus[Organism]"  # Example query
    max_results = 100  # Limit for demonstration
    result_list = fetch_accessions_and_protein_names(db="nucleotide", query=query, max_results=max_results)
