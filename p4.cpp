/* ' Turning movement analysis -- g4.c submenus and output
/* 03/06/92
 Jun Ma
*/

#include "stdafx.h"
#include "a.h"
#include "newint.h"

void menu_0() {
	system("clear");
	printf("\n");
	printf("    *******************************************************************\n");
	printf("    *  INT  														  *\n");
	printf("    *                     CITY OF BELLEVUE							  *\n");
	printf("    *                  PLANNING DEPARTMENT							  *\n");
	printf("    *																  *\n");
	printf("    *																  *\n");
	printf("    *         ESTIMATION OF FUTURE TURNING MOVEMENT					  *\n");
	printf("    *				AVERAGE VEHICLE DELAY							  *\n");
	printf("    *						AND										  *\n");
	printf("    *			INTERSECTION CAPACITY ANALYSIS (212)				  *\n");
	printf("    *																  *\n");
	printf("    *																  *\n");
	printf("    *					Created by									  *\n");
	printf("    *				Dave Tallent & Jun Ma							  *\n");
	printf("    *						1991									  *\n");
	printf("    *					Version IV									  *\n");
	printf("    *******************************************************************\n");

	system("sleep 4");
	system("clear");
}

void menu_1(char cs[], struct LEGSVOL* lg, struct LEGSVOL* lg0, struct LEGSVOL* lg1, struct LEGSVOL* l,
struct LEGSVOL* l0, char* cs1, char* cs2, char* cs3, char* cs4, char* cs5, char* address,
	char* geometry, char* title, int pos) {

	char c[10];
	int i, n;
	system("clear");
	printf("\n");
	printf("\n");
	printf("		%s\n", upper(cs));
	printf("\n");
	printf("\n");
	printf("		1 - List Error Messages\n");
	printf("\n");
	printf("		2 - List Input Files & Calculations\n");
	printf("\n");
	printf("		3 - Output Final Results\n");
	printf("\n");
	printf("		4 - Exit\n");
	printf("\n");

	i = 1;
	while (i > 0) {
		printf("Enter: ");
		scanf_s("%s", c);
		if (c[0] >= '1' && c[0] <= '4')
			break;
	}

	n = atoi(c);
	switch (n) {
	case 1:
		ls_error(cs);
		break;
	case 2:
		ls_interm(cs, lg, lg0, lg1, l, cs1, cs2, cs3, cs4, cs5, title);
		break;
	case 3:
		ls_rst(l0, cs, address, geometry, title, pos);
		break;
	case 4:
		return;
		break;
	}

	menu_1(cs, lg, lg0, lg1, l, l0, cs1, cs2, cs3, cs4, cs5, address, geometry, title, pos);
}


void menu_2(char* cs, struct LEGSVOL* lg, char* address, char* geometry, char* title, int pos) {
	int i;
	char cs0[10];

	system("clear"); 
	printf("\n"); 
	printf("\n");

	printf("		1 - List Error Messages\n");
	printf("\n");
	printf("		2 - Output Final Results\n");
	printf("\n");
	printf("		3 - Exit\n");
	printf("\n");
	printf("Enter: ");
	scanf_s("%s", cs0); 
	i = atoi(cs0);
	while (!(i >= 1 && i <= 3)) {
		printf("Enter:"); 
		scanf_s("%s", cs0); 
		i = atoi(cs0);
	}

	switch (i) {
	case 1:
		ls_error(cs);
		break;
	case 2:
		ls_rst(lg, cs, address, geometry, title, pos);
		break;
	case 3:
		return;
		break;
	}
	menu_2(cs, lg, address, geometry, title, pos);
}
	
void menu_3(char* cs, struct LEGSVOL sv[], char* title) {
	int n;

	system("clear"); 
	printf("\n"); 
	printf("\n");

	printf("				Level OF SERVICE FOR ALL SCENARIOS\n");
	printf("\n");
	printf("\n");
	printf("		1 - Error Messages\n");
	printf("\n");
	printf("		2 - List Level of Service\n");
	printf("\n");
	printf("		3 - Exit\n");
	printf("\n");
	printf("Enter: ");
	scanf_s("%d", &n );

	while (!(n <= 3 && n >= 1)) {
		printf("Enter :"); 
		scanf_s("%d", &n);
	}

	switch (n) {
	case 1:
		ls_error(cs);
		break;
	case 2:
		prn_los_all(sv, title);
		break;
	case 3:
		return;
		break;
	}

	menu_3(cs, sv, title);
}

void menu_4(int redt) {
	char cs[5];
	int i;

	system("clear");
	printf("\n");
	printf("	RIGHT TURN REDUCTION MENU\n");
	printf("\n");
	printf("\n");
	printf("	1. Change User Defined Rt Turn Volume Reduction ");
	printf("(%3.0f%%)\n", fabs(100 * rt_redt));
	printf("\n");
	printf("    2. Consider Futher Reduction for Exclusive RT Turn Lane\n");
	printf("		with respect to LT Turn phasing.");
	if (redt > 0)
		printf("(Y)\n");
	else printf("(N)\n");

	printf("\n");
	printf("    3. Exit\n");
	printf("\n");
	printf("Enter: ");
	scanf_s("%s", cs);
	while (!(cs[0] >= '1' && cs[0] <= '3')) {
		printf("Enter: ");
		scanf_s("%s", cs);
	}

	if (cs[0] == '3')
		return;

	i = atoi(cs);
	switch (i) {
	case 1:
		printf("Enter reduction percentage (0-1): \n");
		scanf_s("%s", cs);
		rt_redt = atof(cs);
		while (!(rt_redt >= 0 && rt_redt <= 1)) {
			printf("Enter a decimal number: \n");
			scanf_s("%s", cs);
			rt_redt = atof(cs);
		}
		break;
	case 2:
		redt = -redt;
		break;
	}

	if (redt > 0)
		rt_redt = fabs(rt_redt);
	else rt_redt = -fabs(rt_redt);

	menu_4(redt);
}


void ls_interm(char* cs, struct LEGSVOL* pt0, struct LEGSVOL* pt1, struct LEGSVOL*pt2, struct LEGSVOL* p,
	char* cs1, char* cs2, char* cs3, char* cs4, char* cs5, char* title) {
	int i, j, n;
	char line[80];
	struct LEGSVOL a[maxline], *pt;

	system("clear");
	printf("\n");
	printf("\n");
	printf("				%s\n", upper(cs));
	printf("\n");
	printf("\n");
	printf("		1. %s\n", cs1);
	printf("\n");
	printf("		2. %s\n", cs2);
	printf("\n");
	printf("		3. %s\n", cs3);
	printf("\n");
	printf("		4. %s\n", cs4);
	printf("\n");
	printf("		5. %s\n", cs5);
	printf("\n");
	printf("		6. Exit\n");
	printf("\n");
	printf("Enter: ");

	scanf_s("%s", line);
	n = atoi(line);

	while (!(n >= 1 && n <= 6)) {
		printf(" Enter:\n");
		scanf_s("%s", line);
		n = atoi(line);
	}

	switch (n) {
	case 1:
		prn_s1(pt0, cs1, title);
		break;
	case 2:
		prn_s1(pt1, cs2, title);
		break;
	case 3:
		prn_s1(pt2, cs3, title);
		break;
	case 4:
		pt = &a[0];
		for (i = 0; i < maxid; i++)
		for (j = 0; j <= 7; j++)
			(*(pt + i)).vol[j] = (*(pt2 + i)).vol[j] - (*(pt1 + i)).vol[j];
		prn_s1(pt, cs4, title);
		break;
	case 5:
		prn_s1(p, cs5, title);
		break;
	case 6:
		return;
		break;
	}
	ls_interm(cs, pt0, pt1, pt2, p, cs1, cs2, cs3, cs4, cs5, title);
}


void ls_rst(struct LEGSVOL* pt, char* cs, char* address, char* geometry, char* title, int pos) {
	int n, n1, n2, m, *pn1, *pn2, *pm; 
	char line[40], fname[40];

	system("clear");
	printf("\n"); 
	printf("\n");
	printf("		%s\n", upper(cs));
	printf("\n"); 
	printf("\n");
	printf("		1. List Link Volumes");
	if (pos == 1)
		printf(" (Inc. 1st Hand Adjustments)\n");
	else if (pos == 3)
		printf(" (Inc. 1st & 2nd Hand Adjustments)\n");
	else if (pos == 4)
		printf(" (Inc. 1st Hand Adjustments)\n");
	else if (pos == 5)
	if (strcmp(upper(cs), "XXXX -> FLXX -> FLFN") == 0)
		printf(" (Inc. 1st & 3rd Hand Adjustments)\n");
	else
		printf(" (Inc. 1st & 2nd Hand Adjustment)\n");
	else
		printf("\n");
	printf("\n");

	printf("		2. List Turning Movement Volumes");
	if (pos == 1)
		printf(" (Inc. 1st Hand Adjustments)\n");
	else if (pos == 3)
		printf(" (Inc. 1st & 2nd Hand Adjustments)\n");
	else if (pos == 4)
		printf(" (Inc. 1st Hand Adjustments)\n");
	else if (pos == 5)
	if (strcmp(upper(cs), "XXXX -> FLXX -> FLFN") == 0)
		printf(" (Inc. 1st & 3rd Hand Adjustments)\n");
	else
		printf(" (Inc. 1st & 2nd Hand Adjustment)\n");
	else
		printf("\n");

	printf("\n");
	printf("		3. List Level of Service\n");
	printf("\n");
	printf("		4. Output Report File (Address, Geometry, Volume & LOS)\n");
	printf("\n");
	printf("		5. Exit\n");
	printf("\n");

	printf("Enter:"); 
	scanf_s("%s", line); 
	n = atoi(line);
	while (!(n >= 1 && n <= 5)) {
		printf("Enter:\n");
		scanf_s("%s", line);
		n = atoi(line);
	}

	if (n == 5) return;

	switch (n) {
	case 1:
		prn_s1(pt, cs, title);
		break;
	case 2:
		pn1 = &n1;
		pn2 = &n2;
		pm = &m;
		get_range(pn1, pn2, fname, pm);
		prn_data(n1, n2, cs, fname, m, title);
		break;
	case 3:
		compute_los(cs, address, geometry, title, pos);
		break;
	case 4:
		cal_los(address, geometry, pos);
		prn_output(address, geometry, title);
		break;
	}
	ls_rst(pt, cs, address, geometry, title, pos);
}

void ls_error(char* cs) {
	char fname[20];
	int n1, n2, lnum, *pn1, *pn2, *pm, i, j, k, m;
	FILE *fp;

	pn1 = &n1;
	pn2 = &n2;
	pm = &m;
	get_range(pn1, pn2, fname, pm);

	if (m == 2){
		lnum = row_p;
		fopen_s(&fp, fname, "w");
	}
	else {
		fp = stdout; 
		lnum = row_s;
	}

	k = 30;
	fprintf(fp, "%s\n", upper(cs));
	fprintf(fp, "Int | Counted/ |  Center  | Scen. A/D | Leg/Node | Turn Vol |\n");
	fprintf(fp, " #  | Modeled  |Node Check|  Balance  | Vol Check| Balancing|\n"); 
	fprintf(fp, "-------------------------------------------------------------");

	for (i = n1; i < n2; i++) {
		if (!(badv[i].flag[0] == 0 && badv[i].flag[1] == 0 && badv[i].flag[2] == 0 &&
			badv[i].flag[3] == 0 && badv[i].flag[4] == 0 && badv[i].flag[5] == 0)){
			k++;
			if (m == 1 && k > row_s) {
				k = getchar();
				k = 1;
			}
			fprintf(fp, "%3d |", badv[i].id);
			for (j = 1; j <= 5; j++)
			if (badv[i].flag[j] != 0)
				fprintf(fp, "%6d	|", badv[i].flag[j]);
			else
				fprintf(fp, "%10s|", " ");

			if (badv[i].flag[0] != 0)
				fprintf(fp, "%6d   ", badv[i].flag[0]);
			fprintf(fp, "\n");
		}
	}
	if (m == 1)
		k = getchar();
	if (m == 2)
		fclose(fp);
	return;
}

void prn_s1(struct LEGSVOL* pt, char* cs, char* title) {
	char fname[20];
	int n1, n2, lnum, *pn1, *pn2, *pm, i, j, m, linenum, pagenum; 
	FILE *fp;

	pn1 = &n1;
	pn2 = &n2;
	pm = &m;
	get_range(pn1, pn2, fname, pm);

	if (m == 2) {
		lnum = row_p;
		fopen_s(&fp, fname, "w");
	}
	else {
		lnum = row_s;
		fp = stdout;
	}

	linenum = getchar();
	linenum = 0;
	pagenum = 1;
	for (j = 0; j < 2; j++)
		fprintf(fp, "\n");
	title_s1(fp, cs, pagenum, title);

	for (i = n1; i <= n2; i++) {
		linenum++;
		if (linenum > lnum) {
			if (m == 1)
				linenum = getchar();
			if (m == 2)
			for (j = 0; j < 4; j++)
				fprintf(fp, "\n");
			linenum = 1;
			pagenum++;
			title_s1(fp, cs, pagenum, title);
		}

		fprintf(fp, "%4d (%4d) ", badv[i].id, badv[i].emid); 
		fprintf(fp, "%6d", (*(pt + i)).vol[0]); 
		fprintf(fp, "%6d", (*(pt + i)).vol[4]); 
		fprintf(fp, "%6d", (*(pt + i)).vol[5]); 
		fprintf(fp, "%6d", (*(pt + i)).vol[1]); 
		fprintf(fp, "%6d", (*(pt + i)).vol[2]); 
		fprintf(fp, "%6d", (*(pt + i)).vol[6]); 
		fprintf(fp, "%6d", (*(pt + i)).vol[7]); 
		fprintf(fp, "%6d", (*(pt + i)).vol[3]); 
		fprintf(fp, "%7d %7d\n", total(pt + i, 0, 3), total(pt + i, 4, 7));
	}
	if (m == 2)
		fclose(fp);
	if (m == 1){
		fprintf(fp, "\n"); 
		fprintf(fp, "\n"); 
		linenum = getchar();
	}
}

/* ask output device and int# to be printed */
void get_range(int* n1, int* n2, char* fname, int* m) {
	int from, to; 
	char line[30];

	printf("Enter Intersection Number or From,To or All\n");
	while (getline(line, sizeof(line)) > 0) {
		if (sscanf_s(line, "%d,%d", &from, &to) == 2 && from <= to) 
			break;
		else if (sscanf_s(line, "%d", &from) == 1) {
			to = from;
			break;
		}
		else if (strncmp(line, "all", 3) == 0 || strncmp(line, "ALL", 3) == 0) {
			from = badv[0].id;
			to = badv[maxid - 1].id;
			break;
		}
		else
			;
	}

	if (from < 0)				/* if FROM int# is less than 0, take */
		from = badv[0].id;		/* the first int#,					 */
	if (to > badv[maxid-1].id)	/* if TO int# is greater than the    */
		to = badv[maxid-1].id;	/* biggest int#, take the biggest one */
	*n1=is_ints_id(from, badv, maxid); 
	*n2=is_ints_id(to, badv, maxid);

	while (*n1 < 0  && from < to) {
		from++;
		*n1 = is_ints_id(from, badv, maxid);
	}

	while (*n2 < 0 && from < to) {
		to--;
		*n2 = is_ints_id(to, badv, maxid);
	}

	if (*n1 < 0 && *n2 < 0) {
		*n1 = 1;
		*n2 = 0;
	}

	printf("\n");
	printf("	1. Terminal\n");
	printf("	2. Printer\n");

	scanf_s("%s", line); 
	*m = atoi(line);
	while (!(*m == 1 || *m == 2)) {
		printf("Enter :");
		scanf_s("%s", line); 
		*m = atoi(line);
	}

	if (*m == 2){
		printf("Enter file name:\n"); 
		scanf_s("%s", fname);
	}
}

void title_s1(FILE* fp, char* cs, int page, char* title) {
	int i, j, text_len;
	fprintf(fp, "%s\n", cs); 
	text_len = strlen(title); 
	j = 35 - text_len / 2;
	for (i = 0; i <= j; i++)
		fprintf(fp, " ");
	fprintf(fp, "%s\n", title);
	for (i = 0; i<65; i++)
		fprintf(fp, " ");
	fprintf(fp, "Page %d\n", page);

	for (j = 1; j<78; j++) 
		fprintf(fp, "-"); 
	fprintf(fp, "\n");

	fprintf(fp, "	Int#		ND		SA		NA		SD		ED		WA		EA		ED		Dep		Arr\n");
	for (j = 1; j<78; j++) 
		fprintf(fp, "-");
	fprintf(fp, "\n");
}

void compute_los(char* cs, char* address, char* geometry, char* title, int scen) {
	int i, n, n1, n2, m, *pn1, *pn2, *pm;
	char fname[40];
	FILE *fp;

	system("clear");
	printf("\n");
	printf("\n");
	printf("			%s\n", upper(cs));
	printf("\n");
	printf("\n");

	printf("		1 - Using Existing Phasing\n");
	printf("\n");
	printf("		2 - Use Optimal Phasing\n");
	printf("\n");
	printf("		3 - Output to GIS\n");
	printf("\n");
	printf("		4 - Exit\n");
	printf("\n");
	printf("Enter:");
	scanf_s("%d", &n);
	while (!(n >= 1 && n <= 4)) {
		printf("Enter:"); 
		scanf_s("%d", &n);
	}

	if (n == 4) return;

	switch (n) {
	case 1:
		cal_los(address, geometry, scen);
		pn1 = &n1;
		pn2 = &n2;
		pm = &m;
		get_range(pn1, pn2, fname, pm);
		prn_los(cs, n1, n2, title, fname, m);
		break;
	case 2:
		opt_los(address, geometry, scen);
		pn1 = &n1;
		pn2 = &n2;
		pm = &m;
		get_range(pn1, pn2, fname, pm);
		prn_los_opt(cs, n1, n2, title, fname, m);
		break;
	case 3:
		cal_los(address, geometry, scen);
		printf("Enter output filename:");
		scanf_s("%s", fname);
		fopen_s(&fp, fname, "w");
		for (i = 0; i < maxid; i++) {
			fprintf(fp, "%4d", badv[i].id);
			if (badv[i].vc != 0)
				fprintf(fp, "%s%4.2f\n", badv[i].los, badv[i].vc);
			else
				fprintf(fp, "\n");
		}
		fclose(fp);
		break;
	}
	compute_los(cs, address, geometry, title, scen);
}

void prn_los_opt(char* cs, int from, int to, char* title, char* fname, int m) {
	int i, j, linenum, pagenum, lnum;
	char cs1[16], cs2[16];
	FILE *fp;

	if (m == 2) {
		lnum = row_p;
		fopen_s(&fp, fname, "w");
	}
	else {
		lnum = row_s; 
		fp = stdout;
	}
	linenum = getchar();
	linenum = 0;
	pagenum = 1;
	for (j = 0; j < 2; j++)
		fprintf(fp, "\n");
	title_los_opt(fp, cs, pagenum, title);

	for (i = from; i <= to; i++) {
		linenum++;
		if (linenum > lnum) {
			if (m == 1)
				linenum = getchar();
			if (m == 2)
				for (j = 0; j < 4; j++)
					fprintf(fp, "\n");
			linenum = 1;
			pagenum++;
			title_los_opt(fp, cs, pagenum, title);
		}
		for (j = 0; j < 15; j++)
			cs1[j] = badv[i].addr[j];
		cs1[15] = '\0';
		for (j = 28; j < 43; j++)
			cs2[j - 28] = badv[i].addr[j]; 
		cs2[15] = '\0';

		fprintf(fp, " %3d | %15s	%15s |", badv[i].id, cs1, cs2);
		if (badv[i].vc != 0) {
			fprintf(fp, " %s	%4.2f %2.0f | ", badv[i].los, badv[i].vc, badv[i].delay[0]);
			if (badv[i].opt_vc != 0)
				fprintf(fp, "%s	%4.2f %2.0f ", badv[i].opt_los, badv[i].opt_vc, badv[i].delay[1]);
			else
				fprintf(fp, "				");
			if (badv[i].flag[0] != 0)
				fprintf(fp, "|  %d\n", badv[i].flag[0]);
			else
				fprintf(fp, "|\n");
		}
		else
			fprintf(fp, "				|					|\n");

	}

	if (m == 2)
		fclose(fp);
	if (m == 1) {
		fprintf(fp, "\n"); 
		fprintf(fp, "\n"); 
		linenum = getchar(); 
	}
}

void title_los_opt(FILE* fp, char* cs, int page, char* title) {
	int i, j, text_len;
	fprintf(fp, "%s\n", cs); 
	text_len = strlen(title); 
	j = 35 - text_len / 2;
	for (i = 0; i <= j; i++)
		fprintf(fp, " ");
	fprintf(fp, "%s\n", title);

	for (i = 0; i < 65; i++) 
		fprintf(fp, " ");
	fprintf(fp, "Page %d\n", page);

	fprintf(fp, "----------------------------------------------------------------------");
	fprintf(fp, " INT |		INTERSECTION		|	EXISTING	|"); // one column could be missing need to investigate
	fprintf(fp, "  #  | N-S Street	E-W Street	| LOS  V/C  PH  |");
	fprintf(fp, "-----|-------------------------|---------------|");
}

void prn_los(char* cs, int from, int to, char* title, char* fname, int m) {
	int i, j, linenum, pagenum, lnum; FILE *fp;

	if (m == 2){
		lnum = row_p;
		fopen_s(&fp, fname, "w");
	}
	else {
		lnum = row_s;
		fp = stdout;
	}

	linenum = getchar();
	linenum = 0;
	pagenum = 1;
	for (j = 0; j < 2; j++)
		fprintf(fp, "\n");
	title_los(fp, cs, pagenum, title);

	for (i = from; i <= to; i++) {
		linenum++;
		if (linenum > lnum) {
			if (m == 1)
				linenum = getchar();
			if (m == 2)
			for (j = 0; j < 4; j++)
				fprintf(fp, "\n");
			linenum = 1;
			pagenum++;
			title_los(fp, cs, pagenum, title);
		}
		fprintf(fp, "%3d |%s|", badv[i].id, badv[i].addr);
		if (badv[i].vc != 0) {
			fprintf(fp, " %s %4.2f %3.0f ", badv[i].los, badv[i].vc, badv[i].delay[0]);
			if (badv[i].flag[0] != 0)
				fprintf(fp, "%d\n", badv[i].flag[0]);
			else
				fprintf(fp, "\n");
		}
		else
			fprintf(fp, "\n");
	}

	if (m == 2)
		fclose(fp);
	if (m == 1) {
		fprintf(fp, "\n"); 
		fprintf(fp, "\n"); 
		linenum = getchar();
	}
}

void title_los(FILE* fp, char* cs, int page, char* title) {
	int i, j, text_len;

	fprintf(fp, "%s\n", cs); 
	text_len = strlen(title); 
	j = 35 - text_len / 2;

	for (i = 0; i <= j; i++)
		fprintf(fp, " ");
	fprintf(fp, "%s\n", title);

	for (i = 0; i <= 65; i++) 
		fprintf(fp, " ");
	fprintf(fp, "Page %d\n", page);

	for (j = 1; j < 80; j++) 
		fprintf(fp, "-");
	fprintf(fp, "\n");

	fprintf(fp, "Int#| North-South Street	East-West Street");
	fprintf(fp, "	| LOS V/C AVD Err\n");
	for (j = 1; j < 80; j++)
		fprintf(fp, "-"); 
	fprintf(fp, "\n");
}

void prn_los_all(struct LEGSVOL ssv[], char* title) {
	int i, j, linenum, pagenum, n1, n2, m, *pn1, *pn2, *pm, lnum;
	char fname[40], cs1[16], cs2[16];
	FILE *fp;
	char int_t_char();

	pn1 = &n1;
	pn2 = &n2;
	pm = &m;
	get_range(pn1, pn2, fname, pm);

	if (m == 2) {
		fopen_s(&fp, fname, "w");
		lnum = row_p;
	}
	else {
		fp = stdout;
		lnum = row_s;
	}

	linenum = getchar();
	linenum = 0;
	pagenum = 1;
	title_los_all(fp, title, pagenum);

	for (i = n1; i <= n2; i++) {
		linenum++;
		if (linenum > lnum) {
			if (m == 2) {
				fprintf(fp, "\n"); 
				fprintf(fp, "\n");
			}
			if (m == 1)
				linenum = getchar();

			linenum = 1;
			pagenum++;
			title_los_all(fp, title, pagenum);
		}

		for (j = 0; j < 15; j++)
			cs1[j] = badv[i].addr[j]; 
		cs1[15] = '\0';

		for (j = 28; j < 43; j++)
			cs2[j - 28] = badv[i].addr[j]; 
		cs2[15] = '\0';

		fprintf(fp, "%3d | %15s %15s |", badv[i].id, cs1, cs2);
		for (j = 0; j < 8; j += 2) {
			if (ssv[i].vol[j + 1] != 0)
				fprintf(fp, " %c %4.2f ", int_to_char(ssv[i].vol[j]), ssv[i].vol[j + 1] / 100.);
			else
				fprintf(fp, "			");
		}

		if (badv[i].flag[0] != 0)
			fprintf(fp, "%2d\n", badv[i].flag[0]); 
		else
			fprintf(fp, "\n");	
	}

	if (m == 2)
		fclose(fp);
	if (m == 1) {
		fprintf(fp, "\n");
		fprintf(fp, "\n");
		linenum = getchar();
	}
}

void title_los_all(FILE* fp, char* cs, int page) {
	int i, j, text_len;

	for (i = 1; i <= 2; i++) 
		fprintf(fp, "\n");
	text_len = strlen(cs); 
	j = 35 - text_len / 2;
	
	for (i = 0; i <= j; i++)
		fprintf(fp, " ");
	fprintf(fp, "%s\n", cs);
	for (i = 0; i < 65; i++)
		fprintf(fp, " ");
	fprintf(fp, "Page %d\n", page);


	fprintf(fp, "----------------------------------------------------------------------");
	fprintf(fp, " INT |		INTERSECTION		|	BASE YR		TestImp	|"); // one column could be missing need to investigate
	fprintf(fp, "  #  | N-S Street	E-W Street	| ACTUAL		XXFN H  |");
	fprintf(fp, "-----|-------------------------|-----------------------|");

}

void prn_data(int from, int to, char* cs, char* fname, int m, char* title) {
	int i, j, linenum, pagenum, lnum; 
	FILE *fp;

	if (m == 2) {
		lnum = row_p;
		fopen_s(&fp, fname, "w");
	}
	else {
		lnum = row_s; 
		fp = stdout;
	}

	linenum = getchar();
	linenum = 0;
	pagenum = 1;
	for (j = 0; j < 3; j++)
		fprintf(fp, "\n");
	title_turns(fp, cs, pagenum, title);

	for (i = from; i <= to; i++) {
		linenum++;
		if (linenum > lnum) {
			if (m == 1)
				linenum = getchar();
			if (m == 2)
				for (j = 0; j < 6; j++)
					fprintf(fp, "\n");
			linenum = 1;
			pagenum++;
			title_turns(fp, cs, pagenum, title);
		}

		fprintf(fp, "%3d |", badv[i].id);
		for (j = 0; j <= 3; j++)
			fprintf(fp, "%5d%5d%5d ", badv[i].vol[j * 3], badv[i].vol[j * 3 + 1], badv[i].vol[j * 3 + 2]);

		fprintf(fp, "\n");
	}
	if (m == 2)
		fclose(fp);
	if (m == 1) {
		fprintf(fp, "\n");
		fprintf(fp, "\n"); 
		linenum = getchar();
	}
}

void title_turns(FILE* fp, char* cs, int page, char* title) {
	int i, j, text_len;

	fprintf(fp, "%s\n", cs);
	text_len = strlen(title);
	j = 35 - text_len / 2; 
	
	for (i = 0; i <= j; i++)
		fprintf(fp, " ");

	fprintf(fp, "%s\n", title);

	for (i = 0; i < 60; i++) 
		fprintf(fp, " ");
	fprintf(fp, "Page %d\n", page);

	for (j = 1; j < 70; j++) 
		fprintf(fp, "-"); 
	fprintf(fp, "\n");

	fprintf(fp, "Int |	NORTHBOUND	SOUTHBOUND	|");
	fprintf(fp, "EASTBOUND	WESTBOUND \n");
	fprintf(fp, " # 1 LT TH RT	LT TH RT	LT	");
	fprintf(fp, "TH RT LT TH RT \n"); 
	
	for (j = 1; j < 70; j++)
		fprintf(fp, "-"); 
	fprintf(fp, "\n");
}

void leg_sum(struct TMVOL t[], struct LEGSVOL* l) {
	int n;
	for (n = 0; n < maxid; n++) {
		(*(l + n)).id = t[n].id;
		(*(l + n)).vol[0] = t[n].vol[1] + t[n].vol[11] + t[n].vol[6]; 
		(*(l + n)).vol[4] = t[n].vol[3] + t[n].vol[4] + t[n].vol[5]; 
		(*(l + n)).vol[1] = t[n].vol[4] + t[n].vol[8] + t[n].vol[9]; 
		(*(l + n)).vol[5] = t[n].vol[0] + t[n].vol[1] + t[n].vol[2];
		(*(l + n)).vol[2] = t[n].vol[2] + t[n].vol[3] + t[n].vol[7]; 
		(*(l + n)).vol[6] = t[n].vol[9] + t[n].vol[10] + t[n].vol[11];
		(*(l + n)).vol[3] = t[n].vol[0] + t[n].vol[5] + t[n].vol[10];
		(*(l + n)).vol[7] = t[n].vol[6] + t[n].vol[7] + t[n].vol[8];
	}
}

void prn_output(char* address, char* geometry, char* title) {
	struct CHAR_S {
		char a[7];
	} cs[13];
	FILE *faddr, *fgeo, *fp;
	int i, j, k, linenum, pagenum, lnum;
	char line[100], addr[60], device[10], phase_ns[3], phase_ew[3],
		cs1[20], fname[40];

	lnum = row_p;
	printf("Enter file name:");
	scanf_s("%s", fname);
	fopen_s(&fp, fname, "w");
	linenum = 0;
	pagenum = 1;
	fprintf(fp, "\n");
	fprintf(fp, "\n");
	title_output(fp, title, pagenum);

	fopen_s(&faddr, address, "r"); 
	for (i = 0; i <= 2; i++)
		fgets(line, 100, faddr);
	fopen_s(&fgeo, geometry, "r");
	for (i = 0; i <= 2; i++)
		fgets(line, 100, fgeo);

	while ((fgets(line, 100, faddr)) != NULL) {
		sscanf_s(line, "%5c%56c%3c%10c", cs[0].a, addr, cs1, device); 
		cs1[3] = '\0';
		device[10] = '\0';
		t[0][0] = atof(cs[0].a);
		linenum++;

		if (linenum > lnum) {
			for (i = 0; i < 4; i++)
				fprintf(fp, "\n");
			linenum = 1;
			pagenum++;
			title_output(fp, title, pagenum);
		}
		
		fgets(line, 100, fgeo);
		sscanf_s(line, "%5c%3c%3c%5c%7c%5c%5c%7c%5c%5c%7c%5c%5c%7c%5c", cs[0].a, phase_ns, phase_ew, cs[1].a,
			cs[2].a, cs[3].a, cs[4].a, cs[5].a, cs[6].a, cs[7].a, cs[8].a, cs[9].a, cs[10].a, cs[11].a, cs[12].a);
		if (t[0][0] != atof(cs[0].a)) {
			printf("Check node number in GEOMETRY. Addr#=%3.0f Geo#=%3.0f\n", t[0][0], atof(cs[0].a));
			exit(0);
		}

		for (i = 1; i <= 4; i++)
			for (j = 1; j <= 3; j++)
				lan[i][j] = atof(cs[(i - 1) * 3 + j].a);
		k = is_ints_id((int)t[0][0], badv, maxid); 
		if (k < 0) {
			printf("Check int.# in the %s\n", geometry);
			exit(0);
		}

		phase[1] = atoi(phase_ns); 
		phase[2] = atoi(phase_ew);
		fprintf(fp, "%3.0f |%56s| %3s %10s |%2d %2d |", t[0][0], badv[k].addr, cs1, device, phase[1], phase[2]);
		for (i = 1; i <= 4; i++)
			for (j = 1; j <= 3; j++)
				if (lan[i][j] > 9999)
					fprintf(fp, "****");
				else
					fprintf(fp, "%4.0f", lan[i][j]);
		fprintf(fp, "|");
		for (i = 0; i <= 11; i++)
			fprintf(fp, "%5d", badv[k].vol[i]);
		fprintf(fp, "|");

		if (badv[k].vc != 0) {
			fprintf(fp, " %s %4.2f %3.0f", badv[k].los, badv[k].vc, badv[k].delay[0]);
			if (badv[k].flag[0] != 0)
				fprintf(fp, " %d\n", badv[k].flag[0]);
			else
				fprintf(fp, "\n");
		}
		else
			fprintf(fp, "\n");
	}

	fclose(faddr); 
	fclose(fgeo);
	fclose(fp);
}

void title_output(FILE* fp, char* cs, int page) {
	int i, j, text_len;

	text_len = strlen(cs);
	j = 90 - text_len / 2;
	for (i = 0; i <= j; i++)
		fprintf(fp, " ");
	fprintf(fp, "%s\n", cs);

	for (i = 0; i < 200; i++)
		fprintf(fp, " ");
	fprintf(fp, "Page %d\n", page);
	fprintf(fp, "-------------------------------------------------------------------------------------------\n");
	fprintf(fp, "INT |							INTERSECTION												\n");
	fprintf(fp, " #  |		North-South Street							East-West Street					\n");
	fprintf(fp, "----|--------------------------------------------------------------------------------------\n");
}

/*get input from standard input device and return the length fo the input string. */
int getline(char* line, int maxn) {
	if ((fgets(line, maxn, stdin)) == NULL) 
		return 0;
	else
		return strlen(line);
}

char int_to_char(int n) {
	switch (n) {
	case 0:
		return ' ';
		break;
	case 1:
		return 'A';
		break;
	case 2:
		return 'B';
		break;
	case 3:
		return 'C';
		break;
	case 4:
		return 'D';
		break;
	case 5:
		return 'E';
		break;
	case 6:
		return 'F';
		break;
	}
	
	return 'O';
}


