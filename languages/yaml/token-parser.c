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

    /* Token-based parser. */
    yaml_token_t token;
    do {
        yaml_parser_scan(&parser, &token);
        switch (token.type) {
        /* Stream start/end */
        case YAML_STREAM_START_TOKEN: puts("STREAM START"); break;
        case YAML_STREAM_END_TOKEN:   puts("STREAM END");   break;

        /* Token types (read before actual token) */
        case YAML_KEY_TOKEN:   printf("(Key)   "); break;
        case YAML_VALUE_TOKEN: printf("(Value) "); break;

        /* Block delimeters */
        case YAML_BLOCK_SEQUENCE_START_TOKEN: puts("---Start Block---"); break;
        case YAML_BLOCK_ENTRY_TOKEN:          puts("---Block Item---");  break;
        case YAML_BLOCK_END_TOKEN:            puts("---End Block---");   break;
        case YAML_FLOW_SEQUENCE_START_TOKEN:  puts("---Start List---");  break;
        case YAML_FLOW_SEQUENCE_END_TOKEN:    puts("---End List---");    break;
        case YAML_FLOW_ENTRY_TOKEN:           puts("---List Item---");   break;
        case YAML_FLOW_MAPPING_START_TOKEN:   puts("---Start Array---"); break;
        case YAML_FLOW_MAPPING_END_TOKEN:     puts("---End Array---");   break;

        /* Data */
        case YAML_BLOCK_MAPPING_START_TOKEN:  puts("[Block mapping]");            break;
        case YAML_SCALAR_TOKEN:  printf("scalar %s \n", token.data.scalar.value); break;

        /* Others */
        case YAML_NO_TOKEN:                   puts("[no token]");          break;
        case YAML_VERSION_DIRECTIVE_TOKEN:    puts("[version directive]"); break;
        case YAML_TAG_DIRECTIVE_TOKEN:        puts("[tag directive]");     break;
        case YAML_DOCUMENT_START_TOKEN:       puts("[document start]");    break;
        case YAML_DOCUMENT_END_TOKEN:         puts("[document end]");      break;
        case YAML_ALIAS_TOKEN:                puts("[alias]");             break;
        case YAML_ANCHOR_TOKEN:               puts("[anchor]");            break;
        case YAML_TAG_TOKEN:                  puts("[tag]");               break;
        }
        if (token.type != YAML_STREAM_END_TOKEN)
            yaml_token_delete(&token);
    } while (token.type != YAML_STREAM_END_TOKEN);
    yaml_token_delete(&token);

    /* Cleanup */
    yaml_parser_delete(&parser);
    fclose(fh);
    return 0;
}
