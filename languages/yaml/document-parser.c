#include <stdio.h>
#include <err.h>
#include <yaml.h>

void print_node(yaml_node_t *node, int i)
{
}

int main(void)
{
    /* Initialize parser */
    yaml_parser_t parser;
    if (!yaml_parser_initialize(&parser))
        errx(1, "Failed to initialize parser!");

    /* Set input file */
    const char *filename = "input.yml";
    FILE *fh = fopen(filename, "r");
    if (! fh) {
        perror(filename);
        exit(1);
    }
    yaml_parser_set_input_file(&parser, fh);

    /* Document-based parser. */
    yaml_document_t document;
    if (!yaml_parser_load(&parser, &document))
        errx(1, "Parser error %d\n", parser.error);

    /* Print the document contents. */
    int i;
    for (i = 1; ; i++) {
        yaml_node_t *node = yaml_document_get_node(&document, i);
        if (!node)
            break;

        printf("Node %d: ", i);
        switch (node->type) {
        case YAML_NO_NODE:       printf("Empty\n"); break;
        case YAML_SEQUENCE_NODE: printf("List\n");  break;
        case YAML_MAPPING_NODE:  printf("Array\n"); break;
        case YAML_SCALAR_NODE:   printf("Scalar %s\n", node->data.scalar.value); break;
        }
    }
    yaml_document_delete(&document);

    /* Cleanup */
    yaml_parser_delete(&parser);
    fclose(fh);
    return 0;
}
