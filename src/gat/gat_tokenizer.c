/*
 * Copyright 2017, Bal Chettri
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "gat_tokenizer.h"
#include "gat_core.h"
#include "gat_err.h"
#include <malloc.h>

/* disable warnings */
#ifdef WIN32
#pragma warning (disable:4996) /* deprecated */
#endif

#define _tx(t) t

int gat_is_whitespace_token (const gat_token *token) {
    return token->length == 0 && token->string[0] != _tx('\'');
}

unsigned gat_tokenize_line (gat *ga) {
    unsigned int i;

    /* reset token counter and mark the start of the token */   
    gat_tokenize (ga);

#if 0/*_DEBUG*/
    if (ga->pass == 1) {
        gat_dump_tokens (ga->arr_raw_tokens, ga->num_tokens);
    }
#endif

    /* set token strings */
    for (i = 0; i < ga->num_tokens; i++) {
        ga->arr_tokens[i] = ga->arr_raw_tokens[i].string;
    }

    return ga->num_tokens;
}

size_t gat_tokenize (gat *ga) {
    char *head, *tail;
    int flag_sym;
    int flag_white;
    int flag_quote;
    char ch, quote_char;
    gat_token_type token_type;
    
    gat_token *token;
    const size_t max_tokens = GAT_MAX_TOKENS;

    ga->num_tokens = 0;
    token = ga->arr_raw_tokens;

    /* init vars */
    flag_sym = 0;
    flag_quote = 0;
    quote_char = '\"';

    head = ga->str_line;

    while (1) {
        /* skip white space */
        while ( *head &&  gat_is_white(*head) ) {
            ++head;
        }

        /* passed the line? */  
        if ( *head == _tx('\0') ) {
            ++head;
            break;
        }

        /* check for token limit */
        if (ga->num_tokens == max_tokens ) {
            gat_error (ga, GAT_ERR_TOO_MANY_TOKENS, "too many tokens");
            break;
        }

        /* set tail pointer to the next character after head */
        tail = head + 1;    

        /* check for symbol */
        flag_sym = gat_is_symbol(*head);
        /* cehck for white space */
        flag_white = gat_is_white(*head);
        /* check for string quote '"' character */
        flag_quote = (*head == quote_char);

        /* determine token type */
        if (flag_quote) {
            token_type = GAT_TOK_STRING;
        } else if (flag_white) {
            token_type = GAT_TOK_WHITE;
        } else if (flag_sym) {
            token_type = GAT_TOK_SYMBOL;
        } else {
            token_type = GAT_TOK_WORD;
        }

        /* find the end of token */
        while (*tail) {
            /* get the character in buffer */
            ch = *tail;

            /* check for a string quote */          
            if (ch == quote_char) {
                /* if the quote flag is already set, this marks the end of quoted string. */
                if (flag_quote) {
                    /* reset the quote flag */
                    flag_quote = 0;
                    /* skip tail to the next character after the end quote */
                    ++tail;
                    break;
                }
                /* we have a new quoted string ahead, set the quote flag. */
                flag_quote = 1;             
                /* token ends before the quote */
                break;
            }

            if (!flag_quote) {
                /* token ends at white space or a symbol if not within a quoted string */
                if ( (gat_is_white(ch) && !flag_white) || (gat_is_symbol(ch)) ) {
                    break;
                }

                /* token ends at non-white character if the last token was a symbol if not 
                within a quoted string */
                if ( (flag_sym && !gat_is_white(ch)) ) {
                    break;
                }
            }
            
            ++tail; /* point to next character */
        } /* !flag_quote */
        
        /* add the token */
        {
            /* unquote string token if required */
            size_t length = tail - head;
            if (token_type == GAT_TOK_STRING) {
                ++head;
                length-= 2;
            }
            /* free existing token if required */
            if (token->string && token->length < length) {
                free (token->string);
                token->string = NULL;
            }
            /* allocate new token memory only if required */
            if (!token->string) {
                token->string = (char *)malloc (sizeof(char) * (length + 1));
            }
            memcpy (token->string, head, sizeof(char) * length);
            token->string [length] = '\0';
            /* strcpy (token->string, head); */
            token->length = length;
            token->type = token_type;
        }

        /* increment the token counter */
        ++ga->num_tokens;

        ++token;
    
        /* move the head pointer to beginning of the next token */
        head = tail;
    }

    /* check if string quote is open */
    if (flag_quote) {
        gat_error (ga, 1, "unterminated string");
        return -1;
    }

    /* return number of tokens parsed */
    return ga->num_tokens;
}

/* free all raw tokens. note that this does not reset the token strings */
void gat_free_tokens (gat *ga) {
    int i;
    for (i = 0; i < GAT_MAX_TOKENS; i++) {
        if (ga->arr_raw_tokens[i].string != NULL) {
            free (ga->arr_raw_tokens[i].string);
            ga->arr_raw_tokens[i].string = NULL;
            ga->arr_raw_tokens[i].length = 0;
            ga->arr_raw_tokens[i].type = GAT_TOK_INVALID;
        }
    }
}

#if _DEBUG
void gat_dump_token (const gat_token *token) {
    char *temp;
    if (token->length == 0) {
        temp = (char *)malloc (sizeof(char) * 2);
    } else {
        temp = (char *)malloc (sizeof(char) * token->length + 1);
    }
    
    if (token->length == 0) {
        temp[0] = token->type == GAT_TOK_WHITE ? _tx(' ') : _tx('\0');
        temp[1] = '\0';
    } else {
        strncpy (temp, token->string, token->length);       
        temp[token->length] = 0;
    }

    switch (token->type) {
    case GAT_TOK_STRING:
        printf ("STRING", temp);
        break;
    case GAT_TOK_SYMBOL:
        printf ("SYMBOL", temp);
        break;
    case GAT_TOK_WHITE:
        printf ("WHITE ", temp);
        break;
    default:
        printf ("WORD  ", temp);
        break;
    }

    printf ("  ~%s~  ", temp);
    printf ("%lu", token->length);
    
    free (temp);
}

void gat_dump_tokens (const gat_token *tokens, size_t length) {
    size_t i;
    for (i=0; i < length; i++) {
        gat_dump_token (&tokens[i]);
        printf (_tx("\n"));
    }
}
#endif
