import requests
from Bio import Entrez
from Bio import SeqIO
import sys
import time

# Set your email for NCBI Entrez (required)
Entrez.email = "your.email@example.com"  # Replace with your email

def get_uniprot_protein_data(accession):
    """
    Retrieve protein sequence and cross-references from UniProt for a given accession.
    Args:
        accession (str): UniProt accession number (e.g., 'P01308').
    Returns:
        tuple: (protein_name, protein_sequence, refseq_transcript, gene_id) or None if failed.
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

        # Extract RefSeq transcript (only NM_ accessions) and GeneID
        refseq_transcript = None
        gene_id = None
        for xref in data.get("uniProtKBCrossReferences", []):
            if xref.get("database") == "RefSeq" and xref.get("id", "").startswith("NM_"):
                refseq_transcript = xref.get("id", "").split(".")[0]  # Get NM_ accession without version
            elif xref.get("database") == "GeneID":
                gene_id = xref.get("id")

        print(f"UniProt data retrieved: RefSeq={refseq_transcript}, GeneID={gene_id}")
        return protein_name, protein_sequence, refseq_transcript, gene_id

    except requests.exceptions.HTTPError as e:
        print(f"HTTP Error for {accession}: {e}")
        return None, None, None, None
    except Exception as e:
        print(f"Unexpected error for {accession}: {e}")
        return None, None, None, None

def get_codon_sequence(refseq_transcript, gene_id):
    """
    Retrieve the mRNA/codon sequence from NCBI using RefSeq transcript or GeneID.
    Args:
        refseq_transcript (str): RefSeq transcript accession (e.g., 'NM_000207').
        gene_id (str): NCBI GeneID (e.g., '3630').
    Returns:
        tuple: (mRNA_sequence, transcript_id) or (None, None) if failed.
    """
    try:
        # Try RefSeq transcript first (only valid NM_ IDs)
        if refseq_transcript and refseq_transcript.startswith("NM_"):
            print(f"Fetching mRNA sequence for RefSeq transcript: {refseq_transcript}")
            handle = Entrez.efetch(db="nucleotide", id=refseq_transcript, rettype="fasta", retmode="text")
            record = SeqIO.read(handle, "fasta")
            handle.close()
            print(f"Successfully retrieved mRNA sequence for {refseq_transcript}")
            return str(record.seq), refseq_transcript

        # Fallback to GeneID
        if gene_id:
            print(f"Falling back to GeneID: {gene_id}")
            handle = Entrez.elink(dbfrom="gene", db="nucleotide", id=gene_id, linkname="gene_nucleotide")
            link_data = Entrez.read(handle)
            handle.close()

            for linkset in link_data[0].get("LinkSetDb", []):
                for link in linkset.get("Link", []):
                    transcript_id = link.get("Id")
                    print(f"Trying transcript: {transcript_id}")
                    try:
                        handle = Entrez.efetch(db="nucleotide", id=transcript_id, rettype="fasta", retmode="text")
                        record = SeqIO.read(handle, "fasta")
                        handle.close()
                        print(f"Successfully retrieved mRNA sequence for {transcript_id}")
                        return str(record.seq), transcript_id
                    except Exception as e:
                        print(f"Error fetching {transcript_id}: {e}")

            print(f"No valid transcripts found for GeneID {gene_id}")
            return None, None

        print("No valid RefSeq transcript or GeneID available for codon sequence")
        return None, None

    except Exception as e:
        print(f"Unexpected error fetching codon sequence: {e}")
        return None, None

def save_sequences(accession, protein_name, protein_sequence, codon_sequence, transcript_id, filename="protein_codon_sequences.fasta"):
    """
    Save protein and codon sequences to a FASTA file.
    Args:
        accession (str): UniProt accession number.
        protein_name (str): Protein name.
        protein_sequence (str): Protein sequence.
        codon_sequence (str): mRNA/codon sequence.
        transcript_id (str): RefSeq transcript ID or None.
        filename (str): Output file name.
    """
    with open(filename, "w", encoding="utf-8") as f:
        if protein_sequence and protein_sequence != "N/A":
            f.write(f">{accession} | {protein_name} | UniProt Protein Sequence\n{protein_sequence}\n")
        else:
            f.write(f">{accession} | {protein_name} | Protein Sequence Unavailable\n")

        if codon_sequence and transcript_id:
            f.write(f">{transcript_id} | {protein_name} | RefSeq mRNA Sequence\n{codon_sequence}\n")
        else:
            f.write(f">{accession} | {protein_name} | mRNA Sequence Unavailable\n")

    print(f"Saved sequences to {filename}")

def main():
    if len(sys.argv) != 2:
        print("Usage: python get_protein_and_codon_sequence.py <UniProt_accession>")
        sys.exit(1)

    accession = sys.argv[1]
    print(f"Fetching data for UniProt accession: {accession}")

    # Get protein data from UniProt
    protein_name, protein_sequence, refseq_transcript, gene_id = get_uniprot_protein_data(accession)

    if protein_name is None:
        print(f"Failed to retrieve data for {accession}. Exiting.")
        sys.exit(1)

    # Get codon sequence from NCBI
    codon_sequence, transcript_id = get_codon_sequence(refseq_transcript, gene_id)

    # Save results
    save_sequences(accession, protein_name, protein_sequence, codon_sequence, transcript_id)

if __name__ == "__main__":
    main()
