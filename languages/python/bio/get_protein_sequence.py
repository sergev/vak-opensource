import requests
import sys

def get_uniprot_protein_data(accession):
    """
    Retrieve protein sequence and name from UniProt for a given accession.
    Args:
        accession (str): UniProt accession number (e.g., 'P01308').
    Returns:
        tuple: (protein_name, protein_sequence) or (None, None) if failed.
    """
    url = f"https://rest.uniprot.org/uniprotkb/{accession}?format=json"

    try:
        response = requests.get(url)
        response.raise_for_status()
        data = response.json()

        # Extract protein name
        protein_name = "N/A"
        if "proteinDescription" in data and "recommendedName" in data["proteinDescription"]:
            protein_name = data["proteinDescription"]["recommendedName"].get("fullName", {}).get("value", "N/A")

        # Extract sequence
        protein_sequence = data.get("sequence", {}).get("value", "N/A")

        print(f"UniProt data retrieved for {accession}: {protein_name}")
        return protein_name, protein_sequence

    except requests.exceptions.HTTPError as e:
        print(f"HTTP Error for {accession}: {e}")
        return None, None
    except Exception as e:
        print(f"Unexpected error for {accession}: {e}")
        return None, None

def save_sequence(accession, protein_name, protein_sequence):
    """
    Save protein sequence to a FASTA file.
    Args:
        accession (str): UniProt accession number.
        protein_name (str): Protein name.
        protein_sequence (str): Protein sequence.
    """
    filename = accession + ".fasta"
    with open(filename, "w", encoding="utf-8") as f:
        if protein_sequence and protein_sequence != "N/A":
            f.write(f">{accession} | {protein_name} | UniProt Protein Sequence\n{protein_sequence}\n")
        else:
            f.write(f">{accession} | {protein_name} | Protein Sequence Unavailable\n")

    print(f"Saved sequence to {filename}")

def main():
    if len(sys.argv) != 2:
        print("Usage: python get_protein_sequence.py <UniProt_accession>")
        sys.exit(1)

    accession = sys.argv[1]
    print(f"Fetching protein sequence for UniProt accession: {accession}")

    # Get protein data from UniProt
    protein_name, protein_sequence = get_uniprot_protein_data(accession)

    if protein_name is None:
        print(f"Failed to retrieve data for {accession}. Exiting.")
        sys.exit(1)

    # Save result
    save_sequence(accession, protein_name, protein_sequence)

if __name__ == "__main__":
    main()
