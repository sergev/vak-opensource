#include <stdio.h>
#include <err.h>
#include <yaml.h>

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

    /* Event-based parser. */
    yaml_event_t event;
    do {
        if (!yaml_parser_parse(&parser, &event))
            errx(1, "Parser error %d\n", parser.error);

        switch (event.type) {
        case YAML_NO_EVENT:             puts("No event!");            break;

        /* Stream start/end */
        case YAML_STREAM_START_EVENT:   puts("STREAM START");         break;
        case YAML_STREAM_END_EVENT:     puts("STREAM END");           break;

        /* Block delimeters */
        case YAML_DOCUMENT_START_EVENT: puts("---Start Document---"); break;
        case YAML_DOCUMENT_END_EVENT:   puts("---End Document---");   break;
        case YAML_SEQUENCE_START_EVENT: puts("---Start List---");     break;
        case YAML_SEQUENCE_END_EVENT:   puts("---End List---");       break;
        case YAML_MAPPING_START_EVENT:  puts("---Start Array---");    break;
        case YAML_MAPPING_END_EVENT:    puts("---End Array---");      break;

        /* Data */
        case YAML_ALIAS_EVENT:  printf("Got alias (anchor %s)\n", event.data.alias.anchor); break;
        case YAML_SCALAR_EVENT: printf("Got scalar '%s'\n", event.data.scalar.value); break;
        }
        if (event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);

    } while (event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);

    /* Cleanup */
    yaml_parser_delete(&parser);
    fclose(fh);
    return 0;
}
