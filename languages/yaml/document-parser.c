/*
 * Demo of YAML file parser, using libyaml.
 *
 * Compile:
 *      cc document-parser.c -lyaml -o document-parser
 */
#include <stdio.h>
#include <err.h>
#include <yaml.h>

void print_level(int level)
{
    while (level-- > 0)
        printf("    ");
}

void print_node(yaml_document_t *document, yaml_node_t *node, int level)
{
    yaml_node_item_t *item;
    yaml_node_pair_t *pair;

    print_level(level);
    switch (node->type) {
    case YAML_NO_NODE:
        printf("Empty\n");
        break;

    case YAML_SEQUENCE_NODE:
        printf("List [\n");
        for (item = node->data.sequence.items.start;
             item < node->data.sequence.items.top; item++)
        {
            yaml_node_t *value = document->nodes.start + *item - 1;

            print_node(document, value, level + 1);
        }
        print_level(level);
        printf("]\n");
        break;

    case YAML_MAPPING_NODE:
        printf("Map {\n");
        for (pair = node->data.mapping.pairs.start;
             pair < node->data.mapping.pairs.top; pair++)
        {
            yaml_node_t *key = document->nodes.start + pair->key - 1;
            yaml_node_t *value = document->nodes.start + pair->value - 1;

            if (key->type != YAML_SCALAR_NODE) {
                err(1, "Key #%d must be scalar!", pair->key);
            }
            print_level(level);
            printf("%s:\n", key->data.scalar.value);
            print_node(document, value, level + 1);
        }
        print_level(level);
        printf("}\n");
        break;

    case YAML_SCALAR_NODE:
        printf("Scalar %s\n", node->data.scalar.value);
        break;
    }
}

int main(void)
{
    /*
     * Initialize parser.
     */
    yaml_parser_t parser;
    if (!yaml_parser_initialize(&parser))
        errx(1, "Failed to initialize parser!");

    /*
     * Set input file.
     */
    const char *filename = "input.yml";
    FILE *fh = fopen(filename, "r");
    if (! fh) {
        err(1, "%s", filename);
    }
    yaml_parser_set_input_file(&parser, fh);

    /*
     * Document parser.
     */
    yaml_document_t document;
    if (!yaml_parser_load(&parser, &document))
        errx(1, "Parser error %d\n", parser.error);

    /*
     * Print the document contents.
     */
    yaml_node_t *root = yaml_document_get_root_node(&document);
    if (!root) {
        errx(0, "Input is empty.\n");
    }
    print_node(&document, root, 0);

    /*
     * Cleanup.
     */
    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    fclose(fh);
    return 0;
}
