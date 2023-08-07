#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "abpoa.h"
#include "abpoa_graph.h"
#include "abpoa_align.h"
#include "abpoa_seq.h"
#include "utils.h"
#if (defined(_WIN32) || defined(_WIN64))
#include "../include/getopt9/include/getopt.h"
#else
#include <getopt.h>
#endif

char NAME[20] = "abPOA";
char PROG[20] = "abpoa";
#define _ba BOLD UNDERLINE "a" NONE
#define _bb BOLD UNDERLINE "b" NONE
#define _bP BOLD UNDERLINE "P" NONE
#define _bO BOLD UNDERLINE "O" NONE
#define _bA BOLD UNDERLINE "A" NONE
char DESCRIPTION[200] = _ba "daptive " _bb "anded " _bP "artial " _bO "rder " _bA "lignment profile process";
char VERSION[20] = "1.4.1.3";
char CONTACT[30] = "gaoy1@chop.edu";

int need_prealign;

const struct option abpoa_long_opt [] = {
    { "align-mode", 1, NULL, 'm' },

    { "match", 1, NULL, 'M' },
    { "mismatch", 1, NULL, 'X' },
    { "gap-open", 1, NULL, 'O' },
    { "gap-ext", 1, NULL, 'E' },

    { "extra-b", 1, NULL, 'b' },
    { "extra-f", 1, NULL, 'f' },
    { "zdrop", 1, NULL, 'z' },
    { "bonus", 1, NULL, 'e' },

    { "seeding", 0, NULL, 'S'},
    { "k-mer", 1, NULL, 'k' },
    { "window", 1, NULL, 'w' },
    { "min-poa-win", 1, NULL, 'n' },
    { "prealign", 0, NULL, 'p'},

    { "amino-acid", 0, NULL, 'c'},

    { "verbose", 0, NULL, 'V' },

    { "output", 1, NULL, 'o' },
    { "blosum", 0, NULL, 'B' },
    { "hoxd", 0, NULL, 'H'},
    { "list", 0, NULL, 'l' },
    { "center", 0, NULL, 'C' },

    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'v' },

    { 0, 0, 0, 0}
};

int abpoa_usage(void)
{
#if (defined(_WIN32) || defined(_WIN64))
    // solution from https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
    DWORD dwMode;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hOutput, &dwMode);
    dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOutput, dwMode);
#endif
    err_printf("\n");
    err_printf("%s: %s \n\n", PROG, DESCRIPTION);
    err_printf("Version: %s\t", VERSION);
	err_printf("Contact: %s\n\n", CONTACT);
    err_printf("Usage: %s [options] > cons.fa\n\n", PROG);
    err_printf("Options:\n");
    err_printf("  Alignment:\n");
    err_printf("    -m --aln-mode   INT     alignment mode [%d]\n", ABPOA_GLOBAL_MODE);
    err_printf("                              %d: global, %d: local, %d: extension\n", ABPOA_GLOBAL_MODE, ABPOA_LOCAL_MODE, ABPOA_EXTEND_MODE);
    err_printf("    -M --match      INT     match score [%d]\n", ABPOA_MATCH);
    err_printf("    -X --mismatch   INT     mismatch penalty [%d]\n", ABPOA_MISMATCH);
    err_printf("    -O --gap-open INT(,INT) gap opening penalty (O1,O2) [%d,%d]\n", ABPOA_GAP_OPEN1, ABPOA_GAP_OPEN2);
    err_printf("    -E --gap-ext  INT(,INT) gap extension penalty (E1,E2) [%d,%d]\n", ABPOA_GAP_EXT1, ABPOA_GAP_EXT2);
    err_printf("                            %s provides three gap penalty modes, cost of a g-long gap:\n", NAME);
    err_printf("                            - convex (default): min{O1+g*E1, O2+g*E2}\n");
    err_printf("                            - affine (set O2 as 0): O1+g*E1\n");
    err_printf("                            - linear (set O1 as 0): g*E1\n");
    err_printf("    -B --blosum             use BLOSUM62 scoring matrix for %sprotein%s sequence alignment [False]\n", RED, NONE);
    err_printf("    -H --hoxd               use HOXD70 scoring matrix for %sDNA/RNA%s sequence alignment [False]\n", RED, NONE);
    err_printf("  Adaptive banded DP:\n");
    err_printf("    -b --extra-b    INT     first adaptive banding parameter [%d]\n", ABPOA_EXTRA_B);
    err_printf("                            set b as < 0 to disable adaptive banded DP\n");
    err_printf("    -f --extra-f  FLOAT     second adaptive banding parameter [%.2f]\n", ABPOA_EXTRA_F);
    err_printf("                            the number of extra bases added on both sites of the band is\n");
    err_printf("                            b+f*L, where L is the length of the aligned sequence\n");
    err_printf("  Minimizer-based seeding and partition (only effective in global alignment mode):\n");
    err_printf("    -S --seeding            enable minimizer-based seeding and anchoring [False]\n");
    err_printf("    -k --k-mer       INT    minimizer k-mer size [%d]\n", ABPOA_MMK);
    err_printf("    -w --window      INT    minimizer window size [%d]\n", ABPOA_MMW);
    err_printf("    -n --min-poa-win INT    min. size of window to perform POA [%d]\n", ABPOA_MIN_POA_WIN);
    err_printf("    -p --prealign           use center sequences make POA [False]\n");
    err_printf("  Input/Output:\n");
    err_printf("    -c --amino-acid         input sequences are amino acid (default is nucleotide) [False]\n");
    err_printf("    -o --output     FILE    output to FILE [stdout]\n");
    err_printf("    -l --list       FILE    profile list FILE [NULL, %s%srequired%s]\n", BOLD, RED, NONE);
    err_printf("    -C --center     FILE    center sequences FILE [NULL, %s%srequired%s]\n", BOLD, RED, NONE);
    err_printf(" Others: \n");
    err_printf("    -V --verbose            verbose some output\n");
    err_printf("    -h --help               print this help usage information\n");
    err_printf("    -v --version            show version number\n");


    err_printf("\n");
    return 1;
}

int abpoa_msa_center(abpoa_t *ab, abpoa_para_t *abpt, char *read_fn, int** order)
{
    abpoa_reset(ab, abpt, 1024);
    abpoa_seq_t *abs = ab->abs;
    int exist_n_seq = abs->n_seq;
    // read seq from read_fn
    FILE* readfp = xopen(read_fn, "r"); kseq_t *ks = kseq_init(fileno(readfp));
    int i, j, n_seq = abpoa_read_seq(abs, ks);

    // always reset graph before perform POA
    int max_len = 0;
    for (i = 0; i < abs->n_seq; ++i) {
        if (abs->seq[i].l > max_len) max_len = abs->seq[i].l;
    }

    // detect u and t
    int has_st = 0, has_su = 0;

    // set seqs, seq_lens
    extern char ab_char26_table[256];
    uint8_t **seqs = (uint8_t**)_err_malloc(n_seq * sizeof(uint8_t*)); int *seq_lens = (int*)_err_malloc(n_seq * sizeof(int));
    int **weights = (int**)_err_malloc(n_seq * sizeof(int*));
    char ch;
    for (i = 0; i < n_seq; ++i) {
        seq_lens[i] = abs->seq[exist_n_seq+i].l;
        seqs[i] = (uint8_t*)_err_malloc(sizeof(uint8_t) * seq_lens[i]);
        weights[i] = (int*)_err_malloc(sizeof(int) * seq_lens[i]);
        for (j = 0; j < seq_lens[i]; ++j)
        {
            ch = abs->seq[exist_n_seq+i].s[j];
            seqs[i][j] = ab_char26_table[(int)ch];
            if(ch == 'U' || ch == 'u') ++ has_su;
            else if(ch == 'T' || ch == 't') ++ has_st;
        }
        for (j = 0; j < seq_lens[i]; ++j) weights[i][j] = 1;
    }
    if(abpt->m == 5) // nuc
    {
        if(has_st && has_su) err_fatal_core(__func__, "DNA sequence has U and T. Please check the sequences, use --amino-acid or -c if sequences are protein.\n");
        else if(has_su)
        {
            _err_simple_func_printf("detected RNA sequences");
            abpt->has_u = 1;
        }
        else
        {
            _err_simple_func_printf("detected DNA sequences");
            abpt->has_u = 0;
        }
    }
    else _err_simple_func_printf("detected Protein sequences");
    
    abpt->disable_seeding = 0; abpt->progressive_poa = 1;
    // sequence pos to node id
    int *tpos_to_node_id = (int*)_err_calloc(max_len, sizeof(int)), *qpos_to_node_id = (int*)_err_calloc(max_len, sizeof(int));
    // seeding, build guide tree, and partition into small windows
    *order = (int*)_err_malloc(sizeof(int) * n_seq); // guide tree order -> input order
    ab_u64_v par_anchors = {0, 0, 0}; int *par_c = (int*)_err_calloc(n_seq, sizeof(int));
    _err_simple_func_printf("Building guide tree...");
    abpoa_build_guide_tree_partition(seqs, seq_lens, n_seq, abpt, *order, &par_anchors, par_c);
    _err_simple_func_printf("Done.");
    if(need_prealign)
    {
        _err_simple_func_printf("Aligning center sequences by progressive mode...");
        abpoa_anchor_poa(ab, abpt, seqs, weights, seq_lens, par_anchors, par_c, tpos_to_node_id, qpos_to_node_id, *order, exist_n_seq, n_seq);
        _err_simple_func_printf("Done.");
    }
    else abpoa_reset(ab, abpt, 1024);
    free(tpos_to_node_id); free(qpos_to_node_id); free(par_c);
    if (par_anchors.m > 0) free(par_anchors.a);

    kseq_destroy(ks); fclose(readfp);
    for (i = 0; i < n_seq; ++i) {
        free(seqs[i]); free(weights[i]);
    } free(seqs); free(weights); free(seq_lens);
    return n_seq;
}

int abpoa_profile_add(abpoa_t *ab, abpoa_para_t *abpt, char *read_fn)
{
    abpoa_seq_t *abs = ab->abs;
    int exist_n_seq = abs->n_seq;
    // read seq from read_fn
    FILE* readfp = xopen(read_fn, "r"); kseq_t *ks = kseq_init(fileno(readfp));
    int i, j, n_seq = abpoa_read_seq(abs, ks);

    // always reset graph before perform POA
    int max_len = 0;
    for (i = 0; i < abs->n_seq; ++i) {
        if (abs->seq[i].l > max_len) max_len = abs->seq[i].l;
    }
    // set seqs, seq_lens
    extern char ab_char26_table[256];
    uint8_t **seqs = (uint8_t**)_err_malloc(n_seq * sizeof(uint8_t*)); int *seq_lens = (int*)_err_malloc(n_seq * sizeof(int));
    int **weights = (int**)_err_malloc(n_seq * sizeof(int*));
    for (i = 0; i < n_seq; ++i) {
        seq_lens[i] = abs->seq[exist_n_seq+i].l;
        seqs[i] = (uint8_t*)_err_malloc(sizeof(uint8_t) * seq_lens[i]);
        weights[i] = (int*)_err_malloc(sizeof(int) * seq_lens[i]);
        for (j = 0; j < seq_lens[i]; ++j) seqs[i][j] = ab_char26_table[(int)abs->seq[exist_n_seq+i].s[j]];
        for (j = 0; j < seq_lens[i]; ++j) weights[i][j] = 1;
    }

    _err_simple_func_printf("Adding...");
    abpoa_poa(ab, abpt, seqs, weights, seq_lens, exist_n_seq, n_seq);

    kseq_destroy(ks); fclose(readfp);
    for (i = 0; i < n_seq; ++i) {
        free(seqs[i]); free(weights[i]);
    } free(seqs); free(weights); free(seq_lens);
    _err_simple_func_printf("Done.");
    return 0;
}

void abpoa_output_profilelist_file(abpoa_t *ab, abpoa_para_t *abpt, int nseq_center, FILE *out_fp)
{
    extern char ab_char256_table[];
    abpoa_generate_rc_msa(ab, abpt);
    if(abpt->has_u) ab_char256_table[3] = 'U';
    if(abpt->m == 27) ab_char256_table[26] = '-';
    else if(abpt->m == 5) ab_char256_table[4] = '-';
    else err_fatal(__func__, "NOT DETERMINED matrix size. Program will exit.\n");
    int i, j;
    abpoa_seq_t *abs = ab->abs; abpoa_cons_t *abc = ab->abc;
    if (abc->msa_len <= 0) return;
    for (i = nseq_center; i < abs->n_seq; ++i) {
        if (abs->name[i].l > 0) {
            if (abs->is_rc[i]) fprintf(out_fp, ">%s_reverse_complement\n", abs->name[i].s);
            else fprintf(out_fp, ">%s\n", abs->name[i].s);
        } else {
            fprintf(out_fp, ">Seq_%d\n", i+1);
        }
        for (j = 0; j < abc->msa_len; ++j) fprintf(out_fp, "%c", ab_char256_table[abc->msa_base[i][j]]);
        fprintf(out_fp, "\n");
    }
    if(abpt->has_u) ab_char256_table[3] = 'T'; // revert change
    if(abpt->m == 27) ab_char256_table[26] = 'N';
    else ab_char256_table[4] = '*';
}

int check_spaces(char* file, abpoa_t *ab)
{
    FILE *fp = xopen(file, "r");
    kseq_t *ks = kseq_init(fileno(fp));
    int msa_len = ab->abc->msa_len, seqs = ab->abs->n_seq, i, j, has_space_first = 0, all_space, this_space;
    abpoa_free(ab);
    char** name, **seq; name = (char**)_err_malloc(seqs * sizeof(char*)); seq = (char**)_err_malloc(seqs * sizeof(char*));
    int *space_list; space_list = (int*)_err_malloc(msa_len * sizeof(int));
    for(i = 0; i < seqs; ++ i)
    {
        kseq_read(ks);
        name[i] = strdup(ks->name.s);
        seq[i] = strdup(ks->seq.s);
    }
    kseq_destroy(ks);
    fclose(fp);
    fp = xopen(file, "wb");
    for(j = 0; j < msa_len; ++ j)
    {
        all_space = 1;
        for(i = 0; i < seqs; ++ i)
        {
            if(seq[i][j] != '-')
            {
                all_space = 0;
                break;
            }
        }
        if(all_space) space_list[has_space_first ++] = j;
    }

    for(i = 0; i < seqs; ++ i)
    {
        fputc('>', fp); fputs(name[i], fp); fputc('\n', fp);
        this_space = 0;
        for(j = 0; j < msa_len; ++ j)
        {
            if(seq[i][j] != '-') fputc(seq[i][j], fp);
            else
            {
                if(space_list[this_space] == j)
                    ++ this_space;
                else fputc('-', fp);
            }
        }
        fputc('\n', fp);
        assert(this_space == has_space_first);
    }
    fclose(fp);
    for(i = 0; i < seqs; ++ i) { free(name[i]); free(seq[i]); } free(name); free(seq); free(space_list);
    return has_space_first;
}

int abpoa_profile_main(char *profilelist, char *center, abpoa_para_t *abpt)
{
    int nseq_center, i, *ptree_order = NULL;
    double realtime0 = realtime();
    abpoa_t *ab = abpoa_init();
    // center file
    nseq_center = abpoa_msa_center(ab, abpt, center, &ptree_order);
    // list process
    FILE *list_fp = xopen(profilelist, "r"), *final_fp;
    char read_fn[1024], **all_seqs, *this_seq;
    all_seqs = (char**)_err_malloc(sizeof(char*) * (nseq_center + 1));
    i = 0;
    while (fgets(read_fn, sizeof(read_fn), list_fp))
    {
        read_fn[strlen(read_fn)-1] = '\0';
        all_seqs[i ++] = strdup(read_fn);
    }
    fclose(list_fp);
    // add by order
    for(i = 0; i < nseq_center; ++ i)
    {
        this_seq = all_seqs[ptree_order[i]];
        err_func_printf(__func__, "Processing file %s...", this_seq);
        abpoa_profile_add(ab, abpt, this_seq);
    }
    // output
    err_func_printf(__func__, "Output to file %s...", all_seqs[0]);
    final_fp = xopen(all_seqs[0], "wb");
    abpoa_output_profilelist_file(ab, abpt, need_prealign ? nseq_center : 0, final_fp);
    fclose(final_fp);
    if(abpt->verbose)
    {
        _err_simple_func_printf("Checking sequence has spaces...");
        err_func_printf(__func__, "found %d column(s) has spaces and removed", check_spaces(all_seqs[0], ab));
        _err_simple_func_printf("Done.");
    }
    else check_spaces(all_seqs[0], ab);
    // clean
    for(i = 0; i < nseq_center; ++ i) { free(all_seqs[i]); } free(all_seqs); free(ptree_order);
	err_func_printf(__func__, "Real time: %.3f sec; CPU: %.3f sec; Peak RSS: %.3f GB.", realtime() - realtime0, cputime(), peakrss() / 1024.0 / 1024.0 / 1024.0);
    return 0;
}

int main(int argc, char **argv) {
    int c, m; char *s; abpoa_para_t *abpt = abpoa_init_para();
    char* profilelist = NULL, *center = NULL;
    abpt->out_cons = 0; abpt->out_msa = 1; abpt->out_fq = 0; abpt->out_gfa = 0; // profile align default values
    abpt->incr_fn = NULL;
    need_prealign = 0;
    while ((c = getopt_long(argc, argv, "m:M:X:O:E:b:f:z:e:Sk:w:n:pco:BHl:C:hvV", abpoa_long_opt, NULL)) >= 0) {
        switch(c)
        {
            case 'm': m = atoi(optarg);
                      if (m != ABPOA_GLOBAL_MODE && m != ABPOA_EXTEND_MODE && m != ABPOA_LOCAL_MODE) {
                          err_printf("Unknown alignment mode: %d.\n", m); return abpoa_usage();
                      } abpt->align_mode=m; break;
            case 'M': abpt->match = atoi(optarg); break;
            case 'X': abpt->mismatch = atoi(optarg); break;
            case 'O': abpt->changed_gap_pen = 1; abpt->gap_open1 = strtol(optarg, &s, 10); if (*s == ',') abpt->gap_open2 = strtol(s+1, &s, 10); break;
            case 'E': abpt->changed_gap_pen = 1; abpt->gap_ext1 = strtol(optarg, &s, 10); if (*s == ',') abpt->gap_ext2 = strtol(s+1, &s, 10); break;

            case 'b': abpt->wb = atoi(optarg); break;
            case 'f': abpt->wf = atof(optarg); break;
            case 'z': abpt->zdrop = atoi(optarg); break;
            case 'e': abpt->end_bonus= atoi(optarg); break;

            case 'S': abpt->disable_seeding = 0; break;
            case 'k': abpt->k = atoi(optarg); break;
            case 'w': abpt->w = atoi(optarg); break;
            case 'n': abpt->min_w = atoi(optarg); break;

            case 'p': need_prealign = 1; break;
            case 'c': abpt->m = 27; abpt->mat = (int*)_err_realloc(abpt->mat, abpt->m * abpt->m * sizeof(int)); break;
            case 'o': if (strcmp(optarg, "-") != 0) {
                          if (freopen(optarg, "wb", stdout) == NULL)
                              err_fatal(__func__, "Failed to open the output file %s", optarg);
                      } break;

            case 'B': abpt->use_score_matrix = 2; break;
            case 'H': abpt->use_score_matrix = 3; break;
            case 'l': profilelist = strdup(optarg); break;
            case 'C': center = strdup(optarg); break;

            case 'V': abpt->verbose ++; break;
            case 'h': return abpoa_usage();
            case 'v': printf("%s\n", VERSION); goto End; break;
            default:
                      err_printf("Error: unknown option: %s.\n", optarg);
                      return abpoa_usage();
                      break;
        }
    }

    if (argc != optind) return abpoa_usage();
    if (center == NULL || profilelist == NULL)
    {
        _err_simple_func_printf("Profile list or center file is not determined. See usage for more information.");
        return abpoa_usage();
    }

    abpoa_post_set_para(abpt);
	fprintf(stderr, "[%s] CMD: ", __func__);
    for (c = 0; c < argc; ++c)
        fprintf(stderr, " %s", argv[c]);
    fprintf(stderr, "\n");
    abpoa_profile_main(profilelist, center, abpt);

End:
    abpoa_free_para(abpt);
    free(profilelist);
    free(center);
    return 0;
}
