#ifndef _ABPOA_SEQ_H
#define _ABPOA_SEQ_H
#include "abpoa.h"
#include "kseq.h"

#if (defined(_WIN32) || defined(_WIN64))
#include <io.h>
#include <process.h>
#else
#include <unistd.h>
#endif

KSEQ_INIT(int, read)

#ifdef __cplusplus
extern "C" {
#endif

abpoa_seq_t *abpoa_realloc_seq(abpoa_seq_t *abs);
void abpoa_cpy_str(abpoa_str_t *str, char *s, int l);
abpoa_seq_t *abpoa_init_seq(void);
void abpoa_free_seq(abpoa_seq_t *abs);
int abpoa_read_seq(abpoa_seq_t *abs, kseq_t *kseq);
#ifdef __cplusplus
}
#endif


#endif
