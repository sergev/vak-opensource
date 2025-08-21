import requests
import time
import json

def get_human_swissprot_proteins(batch_size=500):
    """
    Retrieve a list of all human protein records from UniProt/SwissProt.
    Args:
        batch_size (int): Number of records to fetch per request.
    Returns:
        list: List of dictionaries containing protein details (accession, name, gene).
    """
    proteins = []
    url = "https://rest.uniprot.org/uniprotkb/search"
    query = "taxonomy_id:9606 reviewed:true"
    params = {
        "query": query,
        "format": "json",
        "fields": "accession,protein_name,gene_names",  # Fetch specific fields
        "size": batch_size
    }

    try:
        # Initial request to get total count and first batch
        response = requests.get(url, params=params)
        response.raise_for_status()
        data = response.json()

        # Get total number of records from x-total-results header
        total_count = int(response.headers.get("x-total-results", 0))
        print(f"Total human protein records in UniProt/SwissProt: {total_count}")

        # Process first batch
        for protein in data.get("results", []):
            proteins.append(parse_protein(protein))

        # Handle pagination using cursor-based navigation
        while "Link" in response.headers:
            next_link = response.headers.get("Link", "")
            if 'rel="next"' not in next_link:
                break

            # Extract next page URL from Link header
            next_url = next_link.split(";")[0].strip("<>")
            response = requests.get(next_url)
            response.raise_for_status()
            data = response.json()

            # Process batch
            for protein in data.get("results", []):
                proteins.append(parse_protein(protein))

            print(f"Fetched {len(proteins)} of {total_count} proteins")
            time.sleep(0.5)  # Respect API rate limits

        return proteins

    except requests.exceptions.RequestException as e:
        print(f"Error during API request: {e}")
        return proteins
    except ValueError as e:
        print(f"Error parsing response: {e}")
        return proteins

def parse_protein(protein_data):
    """
    Parse protein data to extract accession, protein name, and gene name.
    Args:
        protein_data (dict): JSON data for a single protein.
    Returns:
        dict: Parsed protein details.
    """
    accession = protein_data.get("primaryAccession", "N/A")
    protein_name = protein_data.get("proteinDescription", {}).get("recommendedName", {}).get("fullName", {}).get("value", "N/A")
    gene_names = protein_data.get("genes", [{}])[0].get("geneName", {}).get("value", "N/A")

    return {
        "accession": accession,
        "protein_name": protein_name,
        "gene_name": gene_names
    }

def save_proteins_to_file(proteins, filename="human_swissprot_proteins.tsv"):
    """
    Save the list of proteins to a TSV file.
    Args:
        proteins (list): List of protein dictionaries.
        filename (str): Output file name.
    """
    with open(filename, "w", encoding="utf-8") as f:
        # Write header
        f.write("Accession\tProtein Name\tGene Name\n")
        # Write protein data
        for protein in proteins:
            f.write(f"{protein['accession']}\t{protein['protein_name']}\t{protein['gene_name']}\n")
    print(f"Saved {len(proteins)} proteins to {filename}")

def main():
    proteins = get_human_swissprot_proteins(batch_size=500)
    save_proteins_to_file(proteins)

if __name__ == "__main__":
    main()
