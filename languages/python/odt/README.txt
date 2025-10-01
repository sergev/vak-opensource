Here is a ready-to-run Python script that:

  * Parses a single Markdown table (pipes-style with header separator row like | --- | :---: | ---: |),
  * Preserves column alignments (left/center/right),
  * Creates an ODT file using odfpy,
  * Adds a simple styled header (bold + background color) and alternate row striping.

Save the script as md_table_to_odt.py and make it executable if you like.
Install dependency:

    pip install odfpy

Example input table.md (must use the pipe/headers separator):

| Name   | Age | City      |
|:-------|:---:|----------:|
| Alice  |  30 | Helsinki  |
| Bob    |  25 | Tampere   |
| Clara  |  28 | Oulu      |

Command:

    python md_table_to_odt.py table.md output.odt
