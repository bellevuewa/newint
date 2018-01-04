#ifndef _NEWINT_H
#define _NEWINT_H

#include "stdio.h"

#define RT_REDUCTION_FACTOR 0.3
// p1.c
int main();
void read_bdata(char* addr, char* node, char* turn, char* adj, struct LEGSVOL* lg);
void check_bdata(struct LEGSVOL* lg);
bool file_exist(char* cs);
void zero_out();

//p2.c
int match(int x, struct TMVOL a[], int num);
void check_link(struct LEGSVOL* lg, struct LEGSVOL* lg0, struct LEGSVOL* lg1, struct LEGSVOL* lsv, int scen);
int total(struct LEGSVOL* pt, int from, int to);
void a_d_balance(struct LEGSVOL* lg, int n);
int is_unique_id(struct TMVOL* b, int* m, int* n);
int is_unique_emid(struct TMVOL* lg, int* m, int* n);
void sort_emid(struct TMVOL v[], int left, int right);
void sort_id(struct TMVOL v[], int left, int right);
void sort_legvol(struct LEGSVOL* l, int left, int right);
void swap_legvol(struct LEGSVOL* l, int i, int j);
void swap_emme2(struct TMVOL v[], int i, int j);
void read_emme2(char filename[], struct LEGSVOL* s, int scen);
int number(struct LEGSVOL* t, struct LEGSVOL* d0, struct LEGSVOL* d1, int i);
void switch_s_e();
void get_leg_vol(struct TMVOL t[], struct LEGSVOL* df, int scen);
int module(int x, int m);
void get_tmvol(double* mpt, int n, int k);
void iteration(double* mbpt, int n, int x, int num, double acpt_error, int scen);
int diff_small(double* mpt, int n, int o_d, double error);
int is_ints_id(int x, struct TMVOL v[], int n);
int is_ints_emid(int x, struct TMVOL v[], int n);
int get_leg(int x, struct TMVOL t[], int n);
char* upper(char s[]);
void str_copy(char s[], char t[]);
void add_adj(char* handadj);
void read_mdata(char* addr, char* node, char* filename, struct LEGSVOL* lg1, int code);
bool stringcompare(char* str1, char* str2);

//p3.c
void opt_los(char* address, char* geometry, int scen);
void take_away(int phase, int* pt);
void cal_los_all(char* address, char* geometry, struct LEGSVOL ssv[], int pst1, int pst2);
void cal_los(char* address, char* geometry, int scen);
int check_vol_lan();
void get_turns(char* cs, int i);
int crt_phasing(int p1, int p2);
void get_lane();
int check_lt();
void assign();
int pce_cat(int i, int p);
int pct_lt(double i, int j);
int get_case(int i);
void lan_separate(double lt[], int i, int a, int b, int c);
void int_los(double l, int phase1, int phase2, char* los);
int int_los1(double l, int phase1, int phase2);
int phase_num(int n);
int critical_vol(int i, int j, int phase, int pos);
void lan_share(double lt[], int i, int l, int m, int n);
void rt_lt_share(double lt[], int i);
int opdir(int i);
double rt_critical(int i, int op, int phase);
void rt_turn();
void convert_lan(double geo[]);
double airq_signal(double geo[], int phasing);
int delay(double vc);
double airq_no_signal(char* cs);
double unctrl(int n, double vol[]);
void get_type(char* cs, int* i, int* j);

//p4.c
void menu_0();
void menu_1(char cs[], struct LEGSVOL* lg, struct LEGSVOL* lg0, struct LEGSVOL* lg1, struct LEGSVOL* l,
struct LEGSVOL* lo, char* cs1, char* cs2, char* cs3, char* cs4, char* cs5, char* address,
	char* geometry, char* title, int pos);
void menu_2(char* cs, struct LEGSVOL* lg, char* address, char* geometry, char* title, int pos);
void menu_3(char* cs, struct LEGSVOL sv[], char* title);
void menu_4(int redt);
void ls_interm(char* cs, struct LEGSVOL* pt0, struct LEGSVOL* pt1, struct LEGSVOL*pt2, struct LEGSVOL* p,
	char* cs1, char* cs2, char* cs3, char* cs4, char* cs5, char* title);
void ls_rst(struct LEGSVOL* pt, char* cs, char* address, char* geometry, char* title, int pos);
void ls_error(char* cs);
void prn_s1(struct LEGSVOL* pt, char* cs, char* title);
void get_range(int* n1, int* n2, char* fname, int length, int* m);
void title_s1(FILE* fp, char* cs, int page, char* title);
void compute_los(char* cs, char* address, char* geometry, char* title, int scen);
void prn_los_opt(char* cs, int from, int to, char* title, char* fname, int m);
void title_los_opt(FILE* fp, char* cs, int page, char* title);
void prn_los(char* cs, int from, int to, char* title, char* fname, int m);
void prn_los_all(struct LEGSVOL ssv[], char* title);
void title_los(FILE* fp, char* cs, int page, char* title);
void title_los_all(FILE* fp, char* cs, int page);
void prn_data(int from, int to, char* cs, char* fname, int m, char* title);
void title_turns(FILE* fp, char* cs, int page, char* title);
void leg_sum(struct TMVOL t[], struct LEGSVOL* l);
void prn_output(char* address, char* geometry, char* title);
void title_output(FILE* fp, char* cs, int page);
int getline(char* line, int maxn);
char int_to_char(int n);

#endif _NEWINT_H




