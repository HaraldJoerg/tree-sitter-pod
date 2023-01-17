#include <tree_sitter/parser.h>

enum TokenType {
  TOKEN_EOL,
  TOKEN_START_DIRECTIVE,
  TOKEN_START_PLAIN,
  TOKEN_START_VERBATIM,
};

void *tree_sitter_pod_external_scanner_create() { return NULL; }

void tree_sitter_pod_external_scanner_destroy(void *payload) {}

void tree_sitter_pod_external_scanner_reset(void *payload) {}

unsigned int tree_sitter_pod_external_scanner_serialize(void *payload, char *buffer) { return 0; }

void tree_sitter_pod_external_scanner_deserialize(void *payload, const char *buffer, unsigned int n) {}

bool tree_sitter_pod_external_scanner_scan(
  void *payload,
  TSLexer *lexer,
  const bool *valid_symbols
) {
  int c = lexer->lookahead;

  if(valid_symbols[TOKEN_EOL]) {
    /* Consumes a single \r?\n, also true at EOF */
    if(lexer->eof(lexer)) {
      lexer->result_symbol = TOKEN_EOL;
      return true;
    }

    if(c == '\r') {
      lexer->advance(lexer, true);
      c = lexer->lookahead;
    }
    if(c == '\n') {
      lexer->advance(lexer, true);
      lexer->result_symbol = TOKEN_EOL;
      return true;
    }
  }

  if(valid_symbols[TOKEN_START_DIRECTIVE] ||
     valid_symbols[TOKEN_START_PLAIN] ||
     valid_symbols[TOKEN_START_VERBATIM]) {
    if(lexer->eof(lexer))
      return false;

    uint32_t column = lexer->get_column(lexer);

    if(column > 0)
      return false;

    switch(c) {
      case 0: // EOF
        return false;

      case '=':
        lexer->result_symbol = TOKEN_START_DIRECTIVE;
        return true;

      case '\n':
      case '\r':
        return false;

      case ' ':
      case '\t':
        lexer->result_symbol = TOKEN_START_VERBATIM;
        return true;
    }

    lexer->result_symbol = TOKEN_START_PLAIN;
    return true;
  }

  return false;
}
