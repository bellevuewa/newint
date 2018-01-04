/*
	Newint is a post processing program that is being used by the COB since 1991. The original newint
	and other companion programs were developed in Unix environment, by Jun Ma and David Tallent, in 1991. Since 2013,
	Hu Dong started migrate the Unix programs to Windows system. Some typos/logic errors were corrected during the 
	migration.


	Hu Dong, City of Bellevue. 2013.
	v1.0  Duplicate of Newint in UNIX environment.
	v1.1  Expand the 300 intersectino limits to 1000. Fix newint output format problem to match UFOSNET requirement.
*/

#include "stdafx.h"
#include "a.h"
#include "newint.h"
#include "dos.h"
#include "windows.h"
#include "string.h"


int main()
{
	struct LEGSVOL *lsv, *lsv0, *lg, *lg0, *lg1, *lg2;
	FILE *fp;
	char linerd[max], filename[40], addr1[40], addr2[40], node1[40], node2[40], turn1[40], geo1[40], geo2[40], title[40], modxxxx[40], modxxfn[40], modflxx[40], modflfn[40], handadj1[40], handadj2[40], handadj3[40];
	int n, i;
	errno_t err = 0;
	
	menu_0();

	rt_redt = RT_REDUCTION_FACTOR;
	gTotIntAval = 0;
	printf("Enter file name: ");
	scanf_s("%s", filename, sizeof(filename));
	while ((err = fopen_s(&fp, filename, "r")) > 0) {
		printf("%s doesn't exist\n", filename);
		printf("Enter:");
		scanf_s("%s", filename, sizeof(filename));
	}

	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", title, _countof(title)); // skip the title info (one line) in the input file string
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", addr1, _countof(addr1)); // address for base year
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", addr2, _countof(addr2)); // address for future year
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", geo1, _countof(geo1)); // geo for base year
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", geo2, _countof(geo2)); // geo for future year
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", node1, _countof(node1)); // link file for base year
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", node2, _countof(node2)); // link file for future year
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", turn1, _countof(turn1)); // base year TM counts
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", handadj1, _countof(handadj1)); // link vol hand adjustment due to errors in base year counts
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", handadj2, _countof(handadj2)); // link vol hand adjustment due to new network from base year land use
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", handadj3, _countof(handadj3)); // link vol hand adjustment due to new network from new land use
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modxxxx, _countof(modxxxx)); // model volume base year land use on base year network
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modxxfn, _countof(modxxfn)); // model volume base year land use on future network
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modflxx, _countof(modflxx)); // model volume future land use on base year network
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modflfn, _countof(modflfn)); // model volume future land use on future network
	fclose(fp);

	n = 1;
	while (n > 0) {
		system("cls");
		printf("               MAIN MENU\n");
		printf("\n");
		printf("\n");
		printf("         1.  xxxx\n");
		printf("            - Base Year TMC \n");
		printf("         2. xxxx -> xxFN\n");
		printf("            - Base Year TMC Modified to Reflect Futurn Network\n");
		printf("         3. xxxx - FLxx\n");
		printf("            - Base Year TMC Modified to Reflect Future Land Use\n");
		printf("         4. xxx -> xxFN -> FLFN\n");
		printf("            - Base Year TMC Modified to Reflect Future Network and Land Use\n");
		printf("         5. xxx -> FLxx -> FLFN\n");
		printf("            - Base Year TMC Modified to Reflect Future Land Use and Network\n");
		printf("         6. Output LOS Summary for All Scenarios\n");
		printf("\n");
		printf("         7. Right Turn Reduction\n");
		printf("\n");
		printf("         8. Exit\n");

		printf("Enter: ");
		scanf_s("%d", &n);
		while (n > 8 || n <= 0) {
			printf("Enter: ");
			scanf_s("%d", &n);
		}
		if (n == 8) exit(0);
		zero_out();
		lg = &base[0];
		read_bdata(addr1, node1, turn1, handadj1, lg);
		check_bdata(lg);
		a_d_balance(lg, 1);				/* BADV in emid order */
		sort_legvol(lg, 0, maxid - 1);	// *lg in id order

		switch (n) {
		case 1:	// scen. - base year count
			sort_id(badv, 0, maxid - 1);
			menu_2("Base Year Actual Turning Movement Counts", lg, addr1, geo1, title, 1); //The last parameter can only be either 1 or 4. Need further investigation.
			break;
		case 2:	/* scen. - xxFN */
			if (!(file_exist(modxxxx) == 1 && file_exist(modxxfn) == 1)) {
				printf("THERE IS NO MODxxxx or MODxxFN ! \n");
				Sleep(5000);
				break;
			}
			lg0 = &sv0[0];
			lg1 = &sv1[0];
			lsv = &sv[0];
			read_emme2(modxxxx, lg0, 2);
			a_d_balance(lg0, 2);
			sort_legvol(lg0, 0, maxid - 1);
			read_mdata(addr2, node2, modxxfn, lg1, 3);
			a_d_balance(lg1, 3);
			sort_legvol(lg1, 0, maxid - 1);	/* *lgl in id order */
			sort_id(badv, 0, maxid - 1);	/* badv in id order */
			check_link(lg, lg0, lg1, lsv, 3);	/* diff. network */
			get_leg_vol(badv, lsv, 3);
			add_adj(handadj2);
			lsv0 = &sv2[0];
			leg_sum(badv, lsv0);
			menu_1("xxxx -> xxFN", lg, lg0, lg1, lsv, lsv0,
				"BADV (Inc. First Hand Adjustments)",
				"MADV xxxx (s0)", "MADV xxFN (s1)", "DELTA_1 (s1 - s0)",
				"BADV + (Si - sO) or [s1/s0]", addr2, geo2, title, 3);
			break;

		case 3:	/* scen -FLxx */
			if (!(file_exist(modxxxx) == 1 && file_exist(modflxx) == 1)) {
				printf("THERE IS NO MODxxxx or MODFLxx ! \n");
				Sleep(5000);
				break;
			}
			lg0 = &sv0[0];
			lg1 = &sv1[0];
			lsv = &sv[0];
			read_emme2(modxxxx, lg0, 2);
			a_d_balance(lg0, 2);
			sort_legvol(lg0, 0, maxid - 1);
			read_emme2(modflxx, lg1, 4);
			a_d_balance(lg1, 4);
			sort_legvol(lg1, 0, maxid - 1);
			sort_id(badv, 0, maxid - 1);
			check_link(lg, lg0, lg1, lsv, 4);	/* same network */
			get_leg_vol(badv, lsv, 4);
			lsv0 = &sv2[0];
			leg_sum(badv, lsv0);
			menu_1("xxxx -> FLxx", lg, lg0, lg1, lsv, lsv0,
				"BADV (Inc. First Hand Adjustments)",
				"MADV xxxx (s0)", "MADV FLxx (s3)", "DELTA_3 (s3 - s0)",
				"BADV + (s3 - sO) or [s3/s0]", addr1, geo1, title, 4);
			break;
		case 4:
			if (!(file_exist(modxxxx) == 1 && file_exist(modxxfn) == 1 &&
				file_exist(modflfn) == 1)) {
				printf("THERE IS NO MODxxxx , or MODxxFN, or MODFLFN ! \n");
				Sleep(5000);
				break;
			}
			lg0 = &sv0[0];
			lg1 = &sv1[0];
			lsv = &sv[0];
			read_emme2(modxxxx, lg0, 2);// lg0: modxxxx
			a_d_balance(lg0, 2);
			sort_legvol(lg0, 0, maxid - 1);
			read_mdata(addr2, node2, modxxfn, lg1, 3); // lg1: modxxfn
			a_d_balance(lg1, 3);
			sort_legvol(lg1, 0, maxid - 1);	// *lg1 in id order
			sort_id(badv, 0, maxid - 1);	// badv in id order
			check_link(lg, lg0, lg1, lsv, 3);	// diff. network
			get_leg_vol(badv, lsv, 3);
			add_adj(handadj2);

			lg = &base[0];
			leg_sum(badv, lg);
			lg2 = &sv0[0];
			sort_emid(badv, 0, maxid - 1);
			read_emme2(modflfn, lg2, 5);
			a_d_balance(lg2, 5);
			sort_legvol(lg2, 0, maxid - 1);
			sort_id(badv, 0, maxid - 1);
			check_link(lg, lg1, lg2, lsv, 5);
			get_leg_vol(badv, lsv, 5);
			lsv0 = &sv2[0];
			leg_sum(badv, lsv0);
			menu_1("xxxx -> xxFN -> FLFN", lg, lg1, lg2, lsv, lsv0,
				"PTxxFN (Inc. Second Hand Adjustmnets)",
				"MADV xxFN (sl)", "MADV FLFN (s2)", "DELTA_2 (s2 - Si)",
				"PTxxFN + (s2 - Si) or [s2/si]", addr2, geo2, title, 5);
			break;
		case 5:
			if (!(file_exist(modxxxx) == 1 && file_exist(modflxx) == 1 &&
				file_exist(modflfn) == 1)) {
				printf("THERE IS NO MODxxxx, or MODFLxx, or MODFLFN! \n");
				Sleep(5000);
				break;
			}

			lg0 = &sv0[0];
			lg1 = &sv1[0];
			lsv = &sv[0];
			read_emme2(modxxxx, lg0, 2);
			a_d_balance(lg0, 2);
			sort_legvol(lg0, 0, maxid - 1);
			read_emme2(modflxx, lg1, 4);
			a_d_balance(lg1, 4);
			sort_legvol(lg1, 0, maxid - 1);
			sort_id(badv, 0, maxid - 1);
			check_link(lg, lg0, lg1, lsv, 4);	// same network
			get_leg_vol(badv, lsv, 4);

			lg = &base[0];
			leg_sum(badv, lg);
			lg2 = &sv0[0];
			read_mdata(addr2, node2, modflfn, lg2, 5);
			a_d_balance(lg2, 5);
			sort_legvol(lg2, 0, maxid - 1);
			sort_id(badv, 0, maxid - 1);
			check_link(lg, lg1, lg2, lsv, 5);	/* diff network */
			get_leg_vol(badv, lsv, 5);
			add_adj(handadj3);
			lsv0 = &sv2[0];
			leg_sum(badv, lsv0);
			menu_1("xxxx -> FLxx -> FLFN", lg, lg1, lg2, lsv, lsv0,
				"PTFLxx", "MADV FLxx (s3)", "MADV FLFN (s2)",
				"DELTA_4 (s2 - s3)",
				"PTFLxx + (s2 - s3) or [s2/s3]", addr2, geo2, title, 5);
			break;

		case 6:
			sort_id(badv, 0, maxid - 1);
			cal_los_all(addr1, geo1, sv2, 0, 1);	// base year actual count 
			if (file_exist(modxxxx) == 1 && file_exist(modxxfn) == 1) {
				sort_emid(badv, 0, maxid - 1);
				lg0 = &sv0[0];
				lg1 = &sv1[0];
				lsv = &sv[0];
				read_emme2(modxxxx, lg0, 2);
				a_d_balance(lg0, 2);
				sort_legvol(lg0, 0, maxid - 1);
				read_mdata(addr2, node2, modxxfn, lg1, 3);
				a_d_balance(lg1, 3);
				sort_legvol(lg1, 0, maxid - 1);
				sort_id(badv, 0, maxid - 1);
				check_link(lg, lg0, lg1, lsv, 3);	/* diff. network */
				get_leg_vol(badv, lsv, 3);
				add_adj(handadj2);
				cal_los_all(addr2, geo2, sv2, 2, 3);	/* modxxfn */

				if (file_exist(modflfn) == 1) {
					lg = &base[0];
					leg_sum(badv, lg);
					lg2 = &sv0[0];
					sort_emid(badv, 0, maxid - 1);
					read_emme2(modflfn, lg2, 5);
					a_d_balance(lg2, 5);
					sort_legvol(lg2, 0, maxid - 1);
					sort_id(badv, 0, maxid - 1);
					check_link(lg, lg1, lg2, lsv, 5);
					get_leg_vol(badv, lsv, 5);
					cal_los_all(addr2, geo2, sv2, 6, 7);	/* modflfn */
				}
				else {
					for (i = 0; i < maxid; i++)
						sv2[i].vol[7] = 0;
				}
			}
			else {
				for (i = 0; i < maxid; i++)
					sv[i].vol[3] = 0;
			}

			if (file_exist(modflxx) == 1) {
				lg = &base[0];
				read_bdata(addr1, node1, turn1, handadj1, lg);
				lg0 = &sv0[0];
				lg1 = &sv1[0];
				lsv = &sv[0];
				read_emme2(modxxxx, lg0, 2);
				a_d_balance(lg0, 2);
				sort_legvol(lg0, 0, maxid - 1);
				read_emme2(modflxx, lg1, 4);
				a_d_balance(lg1, 4);
				sort_legvol(lg1, 0, maxid - 1);
				sort_id(badv, 0, maxid - 1);
				check_link(lg, lg0, lg1, lsv, 4);	// same network
				get_leg_vol(badv, lsv, 4);
				cal_los_all(addr1, geo1, sv2, 4, 5);	/* modflxx */
				read_mdata(addr2, node2, modxxfn, lg1, 3);
				sort_id(badv, 0, maxid - 1);
			}
			else {
				for (i = 0; i < maxid; i++)
					sv2[1].vol[5] = 0;
			}
			menu_3("LOS for All Scenarios", sv2, title);
			break;
		case 7:
			menu_4(-1);	/* default: 30% without extra reduction */
			break;
		}
	}
}

/* read base year actual data: address,grometry,turning movement volumes
and the first hand adjustment*/
void read_bdata(char* addr, char* node, char* turn, char* adj, struct LEGSVOL* lg) {
	int i, j, n, *n1, *n2, error;
	FILE *faddr, *fnode, *fvol;
	char linerd[100];
	errno_t err = 0;

	struct x_type {
		char a[7];
	} x[20]; // x[0] is not used.
	error = 1;
	if (addr[0] == '0' || node[0] == '0' || turn[0] == '0' || adj[0] == '0') {
		printf("Check base year data file names\n");
		exit(0);
	}

	if ((err = fopen_s(&faddr, addr, "r")) > 0) {	/* address */
		printf("%s doesn't exist. ", upper(addr));
		exit(0);
	}

	// skip the first three lines in base year address file *addr
	for (i = 0; i <= 2; i++)
		fgets(linerd, 100, faddr);

	if ((err = fopen_s(&fnode, node, "r")) > 0) {	/* nodes */
		printf("%s doesn't exist. ", upper(node));
		exit(0);
	}

	// skip the first three lines in base year link file *node
	for (i = 0; i <= 2; i++)
		fgets(linerd, max, fnode);

	if ((err = fopen_s(&fvol, turn, "r")) > 0) {	/* turns */
		printf("%s doesn't exist. ", upper(turn));
		exit(0);
	}

	// skip the first three lines in base year TM file *turn
	for (i = 0; i <= 2; i++)
		fgets(linerd, max, fvol);
	i = 0;

	// read data files: address. link. turn 
	// address file controls reading progress.
	while ((fgets(linerd, max, faddr)) != NULL) {
		gTotIntAval++;
		if (gTotIntAval > TOTINT) {
			printf_s("Total intersections exceeds maximum allowed.");
			Sleep(3000);
			exit(1);
		}

		// read intersection ID and address
		sscanf_s(linerd, "%5s%56c", x[1].a, _countof(x[1].a), badv[i].addr, _countof(badv[i].addr));
		badv[i].id = atoi(x[1].a);
		for (j = 1; j <= 6; j++)
			strcpy_s(x[j].a, " ");

		// read link file for the i-th intersection
		// intersection-ID at-node north-node south-node east-node west-node
		if ((fgets(linerd, max, fnode)) != NULL) {
			sscanf_s(linerd, "%5c%6c%6c%6c%6c%6c", x[1].a, sizeof(x[1].a), x[2].a, sizeof(x[2].a),
				x[3].a, sizeof(x[3].a), x[4].a, sizeof(x[4].a),
				x[5].a, sizeof(x[5].a), x[6].a, sizeof(x[6].a));
			n = atoi(x[1].a);
			if (badv[i].id != n) {
				printf("Check node # in %s. Addr#=%d Node#=%d\n", upper(node), badv[i].id, n);
				exit(0);
			}
			badv[i].emid = atoi(x[2].a); // at-node
			for (j = 0; j < 4; j++)
				badv[i].ints_id[j] = atoi(x[j + 3].a); 
		}
		else {
			printf("Check %s and %s\n", upper(addr), upper(node));
			exit(0);
		}

		// read turn volumes for the i-th intersection
		for (j = 1; j <= 13; j++)
			strcpy_s(x[j].a, " ");
		if ((fgets(linerd, max, fvol)) != NULL) {
			// read TM volumes.
			// intersection-ID NBL NBT NBR SBL SBT SBR EBL EBT EBR WBL WBT WBR
			sscanf_s(linerd, "%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c", x[1].a, sizeof(x[1].a), 
				x[2].a, sizeof(x[2].a), x[3].a, sizeof(x[3].a), x[4].a, sizeof(x[4].a),
				x[5].a, sizeof(x[5].a), x[6].a, sizeof(x[6].a), x[7].a, sizeof(x[7].a), 
				x[8].a, sizeof(x[8].a), x[9].a, sizeof(x[9].a), x[10].a, sizeof(x[10].a),
				x[11].a, sizeof(x[11].a), x[12].a, sizeof(x[12].a), 
				x[13].a, sizeof(x[13].a));
			n = atoi(x[1].a); // intersction ID
			for (j = 0; j < 12; j++)
				badv[i].vol[j] = atoi(x[j + 2].a);
			if (badv[i].id != n) {
				printf("Check node # in %s. Addr#=%d Turns#=%d\n",
					upper(turn), badv[i].id, n);
				exit(0);
			}
		}
		else
		for (j = 0; j < 12; j++)
			badv[i].vol[j] = 0;
		i++;
	}

	maxid = i; // maximum intersections processed
	fclose(faddr);
	fclose(fnode);
	fclose(fvol);

	// check if two int. have the same int. # and sort int. in ascending
	// order according to the int. #
	n1 = &i; // max number of intersections 
	n2 = &j; // number of movements
	sort_id(badv, 0, maxid - 1);
	if (is_unique_id(badv, n1, n2) == 0) {
		printf("%d(%d) %d(%d) have the same intersection #\n",
			badv[i].id, badv[i].emid, badv[j].id, badv[j].emid);
		exit(0);
	}

	/* hand adjustment due to base year count error */
	add_adj(adj);

	/* check if two int. have the same emme/2 node id. # and sort int. in
	ascending order according to emme/2 int. */
	n1 = &i;
	n2 = &j;
	sort_emid(badv, 0, maxid - 1);
	if (is_unique_emid(badv, n1, n2) == 0) {
		printf("%d(%d) %d(%d) have the same EMME/2 #\n", badv[i].emid, badv[i].id, badv[j].emid, badv[j].id);
		exit(0);
	}

	// compute link volumes of base year actual counts, write them into BASE 
	leg_sum(badv,lg);
	sort_legvol(lg, 0, maxid - 1);
}


/* check link level volumes and generate flags 
		101: if not counted and not modeled
		102: if not counted but modeled
		103: if counted but not modeled
*/
void check_bdata(struct LEGSVOL* lg) {
	int i, s;
	if (gTotIntAval > TOTINT) {
		printf_s("Total number of intersections exceed maximum allowed.");
		Sleep(3000);
		exit(1);
	}

	for (i = 0; i < maxid - 1; i++) {
		s = total(lg + 1, 0, 7); // calculate total intersection volumes (total departure + total arrival)
		if (badv[i].emid < 9999) {
			if (s == 0)					/*	not counted but modeled */
				badv[i].flag[1] = 102;
		}
		else
		if (s == 0)	/* not counted and not modeled */
			badv[i].flag[1] = 101;
		else	/* counted but not modeled */
			badv[i].flag[1] = 103;
	}
}

bool file_exist(char* cs) {
	if (cs[0] == '0')
		return 0;
	else
		return 1;
}

/*
Initialize all LEGSVOL and TMVOL variables. 
*/
void zero_out() {
	int i = 0, j = 0;

	for (i = 0; i < TOTINT; i++) {
		// LEGSVOL
		base[i].id = 0;
		sv0[i].id = 0;
		sv1[i].id = 0;
		sv2[i].id = 0;
		sv[i].id = 0;
		for (j = 0; j < 8; j++) {
			base[i].vol[j] = 0;
			sv0[i].vol[j] = 0;
			sv1[i].vol[j] = 0;
			sv2[i].vol[j] = 0;
			sv[i].vol[j] = 0;
		}

		// TMVOL
		badv[i].id = 0;

		// initialize the address
		for (int k = 0; k < sizeof(badv[i].addr); k++)
			badv[i].addr[k] = '\0';

		badv[i].emid = 0;
		for (j = 0; j < sizeof(badv[i].ints_id)/sizeof(int); j++)
			badv[i].ints_id[j] = 0;

		for (j = 0; j < sizeof(badv[i].flag) / sizeof(int); j++)
			badv[i].flag[j] = 0;

		strcpy_s(badv[i].los, "");
		badv[i].vc = 0;
		strcpy_s(badv[i].opt_los, "");
		badv[i].opt_vc = 0;
		for (j = 0; j < 2; j++)
			badv[i].delay[j] = 0;
	}
}

// probably not in use.
int match(int x, struct TMVOL a[], int num) {
	int i;
	for (i = 0; i <= num; i++)
	if (x == a[i].id)
		return i;
	return -1;
}

/* error checking
lg: 	points to base year link volumes
lgO:    point to modeled base year link volumes
lgl:    points to modeled future link volumes
lsv:	points to the result: base+s2-sl
scen:	scenario code
2 - modxxxx
3 - modxxfn
4 - modflxx
5 - modflfn
*/
void check_link(struct LEGSVOL* lg, struct LEGSVOL* lg0, struct LEGSVOL* lg1, struct LEGSVOL* lsv, int scen) {
	int i, j, m;

	for (i = 0; i < maxid; i++) {
		(*(lsv + i)).id = badv[i].id;
		for (j = 0; j <= 7; j++) {
			m = number(lg + i, lg0 + i, lg1 + i, j);

			switch (m) {
			case 1:	/* s0!=0, sl!=0, s2!=0 */
				(*(lsv + i)).vol[j] = (*(lg + i)).vol[j] + (*(lg1 + i)).vol[j] - (*(lg0 + i)).vol[j];
				if ((*(lsv + i)).vol[j] < 0) {
					(*(lsv + i)).vol[j] = (int)((*(lg + i)).vol[j] * (*(lg1 + i)).vol[j] / (*(lg0 + i)).vol[j] + .5);
					badv[i].flag[4] = (badv[i].flag[4] == 0) ? 401 + scen * 10 : badv[i].flag[4];
				}
				break;
			case 2:	/* s0!=0, s1!=0, s2=0	*/
				if ((*(lg + i)).vol[j] > (*(lg0 + i)).vol[j])
					(*(lsv + i)).vol[j] = (*(lg + i)).vol[j] - (*(lg0 + i)).vol[j];
				else
					(*(lsv + i)).vol[j] = 0;

				badv[i].flag[4] = (badv[i].flag[4] == 0) ? 402 + scen * 10 : badv[i].flag[4];
				break;
			case 3:	/* s0!=0, s1=0, s2!=0	*/
				badv[i].flag[4] = (badv[i].flag[4] == 0) ? 403 + scen * 10 : badv[i].flag[4];
				(*(lsv + i)).vol[j] = (*(lg + i)).vol[j] + (*(lg1 + i)).vol[j];
				break;
			case 4:	/* s0!=0, s1=0, s2=0	*/
				(*(lsv + i)).vol[j] = (*(lg + i)).vol[j];
				badv[i].flag[4] = (badv[i].flag[4] == 0) ? 404 + scen * 10 : badv[i].flag[4];
				break;
			case 5:	/* s0=0, sl!=0, s2!=0 */
				(*(lsv + i)).vol[j] = 0;
				badv[i].flag[4] = (badv[i].flag[4] == 0) ? 405 + scen * 10 : badv[i].flag[4];
				break;
			case 6:	/* s0=0, sl!=0, s2=0 */
				(*(lsv + i)).vol[j] = 0;
				badv[i].flag[4] = (badv[i].flag[4] == 0) ? 406 + scen * 10 : badv[i].flag[4];
				break;
			case 7:	/* s0=0, s1=0,s2!=0 */
				(*(lsv + i)).vol[j] = (*(lg1 + i)).vol[j];
				badv[i].flag[4] = (badv[i].flag[4] == 0) ? 407 + scen * 10 : badv[i].flag[4];
				break;
			case 8:	/* s0=0, s1=0, s2=0 */
				(*(lsv + i)).vol[j] = 0;
				break;
			}
		}
	}

}

/* compute the sum of link volumes. Links are defined by FROM and TO */
int total(struct LEGSVOL* pt, int from, int to) {
	int i, t;
	t = 0;
	for (i = from; i <= to; i++)
		t = t + (*pt).vol[i];
	return t; 
}

/* If arrivals and departures of a intersection are not balance, the third
level error flag is set to 300 level
scen =	 1 -- scenario of actual count
2 -- scenario of modxxxx
3 --	modxxFN
4 --	modFLxx
5 --	modFLFN
*/
void a_d_balance(struct LEGSVOL* lg, int scen) {
	int i, tl, t2;
	for (i = 0; i < maxid; i++) {
		if (badv[i].emid < 9999) {
			tl = total(lg + i, 0, 3);
			t2 = total(lg + i, 4, 7);
			if ((abs(tl - t2)) > 4)
				badv[i].flag[3] = (badv[i].flag[3] == 0) ? 300 + scen * 10 : badv[i].flag[3];
		}
	}
}

//If int. number is unique, return 1; otherwise return 0 and *m and *n will point to the objects with the same
// intersection ID
int is_unique_id(struct TMVOL* b, int* m, int* n){
	int i, j;
	for (i = 0; i < maxid - 1; i++) {
		for (j = i + 1; j < maxid; j++) {
			if (badv[i].id == badv[j].id) {
				*m = i;
				*n = j;
				return 0;
			}
		}
	}
	return 1;
}

/* If emme/2 int. number is unique, return 1; otherwise return 0 */
int is_unique_emid(struct TMVOL* lg, int* m, int* n) {
	int i, j;
	for (i = 0; i < maxid - 1; i++) {
		if (badv[i].emid < 9999)
		for (j = i + 1; j < maxid; j++) {
			if (badv[i].emid == badv[j].emid) {
				*m = i;
				*n = j;
				return 0;
			}
		}
	}
	return 1;

}

// quicksort emme/2 node ID number, in ascending order
void sort_emid(struct TMVOL v[], int left, int right) {
	int i, last;
	if (left >= right)
		return;
	swap_emme2(v, left, (left + right) / 2);
	last = left;

	for (i = left + 1; i <= right; i++)
	if (badv[i].emid < badv[left].emid)
		swap_emme2(v, ++last, i);

	swap_emme2(v, left, last);
	sort_emid(v, left, last - 1);
	sort_emid(v, last + 1, right);
}

// quicksort int.number, in ascending order
void sort_id(struct TMVOL v[], int left, int right) {
	int i, last;

	if (left >= right)
		return;

	swap_emme2(v, left, (left + right) / 2);
	last = left;
	for (i = left + 1; i <= right; i++)
		if (badv[i].id < badv[left].id)
			swap_emme2(v, ++last, i);
	
		swap_emme2(v, left, last);
	sort_id(v, left, last - 1);
	sort_id(v, last + 1, right);
}

// quicksort int. number , in ascending order 
void sort_legvol(struct LEGSVOL* l, int left, int right) {
	int i, last;

	if (left >= right)
		return;
	swap_legvol(l, left, (left + right) / 2);
	last = left;

	for (i = left + 1; i <= right; i++)
	if ((*(l + i)).id < (*(l + left)).id)
		swap_legvol(l, ++last, i);
	swap_legvol(l, left, last);
	sort_legvol(l, left, last - 1);
	sort_legvol(l, last + 1, right);
}

// swap two objects identified by l+i and l+j
void swap_legvol(struct LEGSVOL* l, int i, int j) {
	struct LEGSVOL a;
	int n;

	a.id = (*(l + i)).id;
	(*(l + i)).id = (*(l + j)).id;
	(*(l + j)).id = a.id;
	for (n = 0; n <= 7; n++) {
		a.vol[n] = (*(l + i)).vol[n];
		(*(l + i)).vol[n] = (*(l + j)).vol[n];
		(*(l + j)).vol[n] = a.vol[n];
	}
}

void swap_emme2(struct TMVOL v[], int i, int j) {
	struct TMVOL a;
	int n;

	a.id = v[i].id;
	v[i].id = v[j].id;
	v[j].id = a.id;

	strcpy_s(a.addr, v[i].addr);
	strcpy_s(v[i].addr, v[j].addr);
	strcpy_s(v[j].addr, a.addr);

	a.emid = v[i].emid;
	v[i].emid = v[j].emid;
	v[j].emid = a.emid;

	for (n = 0; n <= 3; n++) {
		a.ints_id[n] = v[i].ints_id[n];
		v[i].ints_id[n] = v[j].ints_id[n];
		v[j].ints_id[n] = a.ints_id[n];
	}

	for (n = 0; n <= 11; n++) {
		a.vol[n] = v[i].vol[n];
		v[i].vol[n] = v[j].vol[n];
		v[j].vol[n] = a.vol[n];
	}

	for (n = 0; n < 6; n++) {
		a.flag[n] = v[i].flag[n];
		v[i].flag[n] = v[j].flag[n];
		v[j].flag[n] = a.flag[n];
	}

	strcpy_s(a.los, v[i].los);
	strcpy_s(v[i].los, v[j].los);
	strcpy_s(v[j].los, a.los);

	a.vc = v[i].vc;
	v[i].vc = v[j].vc;
	v[j].vc = a.vc;

	strcpy_s(a.opt_los, v[i].opt_los);
	strcpy_s(v[i].opt_los, v[j].opt_los);
	strcpy_s(v[j].opt_los, a.opt_los);

	a.opt_vc = v[i].opt_vc;
	v[i].opt_vc = v[j].opt_vc;
	v[j].opt_vc = a.opt_vc;

	for (n = 0; n <= 1; n++) {
		a.delay[n] = v[i].delay[n];
		v[i].delay[n] = v[j].delay[n];
		v[j].delay[n] = a.delay[n];
	}
}

// read link volumes from emme/2 batchout file 
void read_emme2(char filename[], struct LEGSVOL* s, int scen) {
	int frnd, tond, s0, m, m1, i, j, n;
	char linerd[40];
	FILE *fp;
	errno_t err = 0;

	if ((err = fopen_s(&fp, filename, "r")) > 0) {
		printf("%s doesn't exit. Enter: ", upper(filename));
		exit(0);
	}

	//initialization
	for (i = 0; i < maxid; i++) {
		(*(s + i)).id = badv[i].id;
		for (j = 0; j <= 7; j++)
			(*(s + i)).vol[j] = 0;
	}

	fgets(linerd, 40, fp); //skip the header
	while ((fgets(linerd, 40, fp)) != NULL) {
		sscanf_s(linerd, "%d %d %d", &frnd, &tond, &s0);
		m = is_ints_emid(frnd, badv, maxid);
		n = is_ints_emid(tond, badv, maxid);
		if (m >= 0) {
			m1 = get_leg(tond, badv, m);
			if (m1 >= 0)
				(*(s + m)).vol[m1] = s0;
		}
		if (n >= 0) {
			m1 = get_leg(frnd, badv, n);
			if (m1 >= 0)
				(*(s + n)).vol[4 + m1] = s0;
		}
	}

	fclose(fp);

	for (i = 0; i < maxid; i++)
	if (total(s + i, 0, 7) == 0 && badv[i].emid < 9999)
		badv[i].flag[2] = (badv[i].flag[2] == 0) ? 200 + scen * 10 : badv[i].flag[2];
}

// find out corresponding error case 
int number(struct LEGSVOL* t, struct LEGSVOL* d0, struct LEGSVOL* d1, int i) {
	if ((*t).vol[i] != 0)
		if ((*d0).vol[i] != 0)
			if ((*d1).vol[i] != 0)
				return 1;
			else
				return 2;
		else
			if ((*d1).vol[i] != 0)
				return 3;
			else
				return 4;
	else
		if ((*d0).vol[i] != 0)
			if ((*d1).vol[i] != 0)
				return 5;
			else
				return 6;
		else
			if ((*d1).vol[i] != 0)
				return 7;
			else
				return 8;
}

/* switch TM volumes between 2nd and 3rd apporaching groups.
*/
void switch_s_e() {
	int i, j, temp;
	for (i = 0; i < maxid; i++) {
		for (j = 3; j <= 5; j++) {
			temp = badv[i].vol[j];
			badv[i].vol[j] = badv[i].vol[3 + j];
			badv[i].vol[j + 3] = temp;
		}
	}
}

// compute turning movement 
void get_leg_vol(struct TMVOL t[], struct LEGSVOL* df, int scen) {
	int i, j, k, n, l, h;
	double m[row][row], *mpt;

	switch_s_e();
	mpt = &m[0][0];
	for (k = 0; k < maxid; k++) {
		n = 4;
		l = 0;
		for (i = 1; i <= n; i++) {
			h = module(i + 1, n);
			for (j = 1; j <= n; j++) {
				if (i != j) {
					m[i][h] = badv[k].vol[l++];
					h = module(h + 1, n);
				}
				else m[i][j] = 0;
			}
		}

		m[0][1] = (*(df + k)).vol[1];
		m[0][2] = (*(df + k)).vol[3];
		m[0][3] = (*(df + k)).vol[0];
		m[0][4] = (*(df + k)).vol[2];
		m[1][0] = (*(df + k)).vol[5];
		m[2][0] = (*(df + k)).vol[7];
		m[3][0] = (*(df + k)).vol[4];
		m[4][0] = (*(df + k)).vol[6];

		iteration(mpt, n, 0, k, max_error, scen);
		get_tmvol(mpt, n, k);
	}

	switch_s_e();
}

int module(int x, int m){
	if (x <= m)
		return x;
	else
		return 1;
}

// compute turning movement from the matrix 
void get_tmvol(double* mpt, int n, int k) {
	int i, j, h, l;

	l = 0;
	for (i = 1; i <= n; i++) {
		h = module(i + 1, n);
		for (j = 1; j <= n; j++) {
			if (i != j) {
				badv[k].vol[l++] = (int)floor(*(mpt + i*row + h) + 0.5);
				h = module(h + 1, n);
			}
		}
	}
}

/* based on the turning percentage of the base year, split link volumes
to turning movement iteratively until the difference between the result
and the volumes desired is less than a predtermined value or the total
number of iterations is more than 10
*/
void iteration(double* mbpt, int n, int x, int num, double acpt_error, int scen) {
	int i, j;
	double ratio, o[6], d[6];

	x++;
	if (x > 50) {
		acpt_error += 0.01;
		x = 0;
		if (acpt_error > 2 * max_error) {
			badv[num].flag[5] = 500 + scen * 10 + (int)(acpt_error * 100);
			return;
		}
	}

	for (i = 1; i <= n; i++) {
		o[i] = 0;
		for (j = 1; j <= n; j++)
			o[i] = o[i] + (*(mbpt + i*row + j));
	}

	for (i = 1; i <= n; i++) {
		if (o[i] == 0)
			ratio = 0.;
		else
			ratio = (*(mbpt + i*row + 0)) / o[i];
		for (j = 1; j <= n; j++)
			//*(mbpt + i*row + j) = (int)(ratio * (*(mbpt + i*row + j)));
			*(mbpt + i*row + j) = ratio * (*(mbpt + i*row + j));
	}

	if (diff_small(mbpt, n, 0, acpt_error) == 1) {
		badv[num].flag[5] = (acpt_error <= max_error) ? 0 : 500 + scen * 10 + (int)(acpt_error * 100);
		return;
	}

	for (i = 1; i <= n; i++) {
		d[i] = 0;
		for (j = 1; j <= n; j++)
			d[i] = d[i] + (*(mbpt + j*row + i));
	}

	for (i = 1; i <= n; i++) {
		if (d[i] == 0)
			ratio = 0;
		else
			ratio = (*(mbpt + 0 * row + i)) / d[i];
		for (j = 1; j <= n; j++)
			*(mbpt + j*row + i) = ratio * (*(mbpt + j*row + i));
	}

	if (diff_small(mbpt, n, 1, acpt_error) == 1) {
		badv[num].flag[5] = (acpt_error <= max_error) ? 0 : 500 + scen * 10 + (int)(acpt_error * 100);
		return;
	}

	iteration(mbpt, n, x, num, acpt_error, scen);
}

// if the defference is less than a predtermined value, return 1;
// otherwise return 0
int diff_small(double* mpt, int n, int o_d, double error) {
	int i, j, x, o, d;
	double sum, del;

	x = 1;
	for (i = 1; i <= n; i++) {
		sum = 0;
		if (o_d == 0) {
			o = 0;
			d = i;
			for (j = 1; j <= n; j++)
				sum += (*(mpt + j*row + i));
		}
		else {
			o = i;
			d = 0;
			for (j = 1; j <= n; j++)
				sum += (*(mpt + i*row + j));
		}
		if (*(mpt + o*row + d) == 0)
			del = sum;
		else
			del = fabs(sum / (*(mpt + o*row + d)) - 1);
		if (del > error)
			x = 0;
	}

	return x;
}

// if number x is a intersection number, return the position of
// the int.; otherwise return -1
int is_ints_id(int x, struct TMVOL v[], int n) {
	int low, high, mid;

	low = 0;
	high = n - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		if (x < v[mid].id)
			high = mid - 1;
		else if (x > v[mid].id)
			low = mid + 1;
		else
			return mid;
	}
	return -1;
}

// if number x is a emme/2 node id. number, return the position of the int.
//  Dtherwise return -1
int is_ints_emid(int x, struct TMVOL v[], int n) {
	int low, high, mid;

	low = 0;
	high = n - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		if (x < v[mid].emid)
			high = mid - 1;
		else if (x > v[mid].emid)
			low = mid + 1;
		else
			return mid;
	}
	return -1;
}

// find out which leg the node x is 
int get_leg(int x, struct TMVOL t[], int n) {
	int i;
	for (i = 0; i <= 3; i++) {
		if (t[n].ints_id[i] == x)
			return i;
	}
	return -1;
}

// change string S to uppercase 
char* upper(char s[]) {
	int i;
	int len = strlen(s);
	
	//char* cap = new char[len + 1];
	for (i = 0; i < len; i++)
		//cap[i] = toupper(s[i]);
		s[i] = toupper(s[i]);
		
	//cap[i] = '\0';
	//for (i = 0; i < len; i++)
	//	*(s + i) = cap[i];

	//delete cap;
	return s;
}

// copy string t to string s without the newline '\n'
void str_copy(char s[], char t[]) {
	int i;
	i = 0;
	while (t[i] != '\n') {
		s[i] = t[i];
		i++;
	}
	s[i] = '\0';
}


void read_mdata(char* addr, char* node, char* filename, struct LEGSVOL* lg1, int code) {
	FILE *faddr, *fnode;
	int i, n;
	char linerd[100], x[10] = {};
	errno_t err = 0;

	sort_id(badv, 0, maxid - 1);
	if ((err = fopen_s(&faddr, addr, "r")) > 0) {
		printf("%s doesn't exist. ", upper(addr));
		exit(0);
	}

	for (i = 0; i <= 2; i++)
		fgets(linerd, 100, faddr);

	if ((err = fopen_s(&fnode, node, "r")) > 0) {
		printf("%s doesn't exist. ", upper(node));
		exit(0);
	}

	for (i = 0; i <= 2; i++)
		fgets(linerd, max, fnode);

	i = 0;
	while ((fgets(linerd, max, faddr)) != NULL) {
		sscanf_s(linerd, "%5c%56c", x, badv[i].addr);
		badv[i].id = atoi(x);
		fgets(linerd, max, fnode);
		sscanf_s(linerd, "%d %d %d %d %d %d", &n, &badv[i].emid, &badv[i].ints_id[0],
			&badv[i].ints_id[1], &badv[i].ints_id[2], &badv[i].ints_id[3]);
		if (badv[i].id != n) {
			printf("Check node # in %s. Addr#=%d Node#=%d\n", upper(node), badv[i].id, n);
			exit(0);
		}
		i++;
	}

	maxid = i;
	fclose(faddr);
	fclose(fnode);
	sort_emid(badv, 0, maxid - 1);
	read_emme2(filename, lg1, code);
}

// conduct hand adjustment. Replace TM volumes with hand adjustment volumes.
// handadj: hand adjustment file name
void add_adj(char* handadj) {
	FILE *fp;
	struct x_type {
		char a[5];
	} x[20];
	int i, j, n;
	char linerd[100];
	errno_t err = 0;

	n = 1;
	if ((err = fopen_s(&fp, handadj, "r")) > 0) {
		printf("%s doesn't exist. ", upper(handadj));
		exit(0);
	}
	// skip the headers
	fgets(linerd, max, fp);
	fgets(linerd, max, fp);
	fgets(linerd, max, fp);

	while ((fgets(linerd, max, fp)) != NULL) {
		// Int_ID NBL NBT NBR SBL SBT SBR EBL EBT EBR WBL WBT WBR
		sscanf_s(linerd, "%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c",
			x[0].a, sizeof(x[0].a), x[1].a, sizeof(x[1].a), x[2].a, sizeof(x[2].a), x[3].a, sizeof(x[3].a),
			x[4].a, sizeof(x[4].a), x[5].a, sizeof(x[5].a), x[6].a, sizeof(x[6].a), x[7].a, sizeof(x[7].a),
			x[8].a, sizeof(x[8].a), x[9].a, sizeof(x[9].a), x[10].a, sizeof(x[10].a), x[11].a, sizeof(x[11].a),
			x[12].a, sizeof(x[12].a));
		j = is_ints_id(atoi(x[0].a), badv, maxid);
		if (j == -1) {
			n = 0;
			printf("*** Node %d doesn't exist in the base year file.\n", atoi(x[0].a));
		}
		// pass read volumes to TMVOL structure, replace counts with hand adjustment volumes.
		for (i = 0; i <= 11; i++)
			badv[j].vol[i] = atoi(x[i + 1].a);

		// reset the flags for the j-th intersection to zero.
		for (i = 0; i < 6; i++)
			badv[j].flag[i] = 0;
	}

	fclose(fp);
	//if (n == 0)
		//Sleep(5000);
}

void opt_los(char* address, char* geometry, int scen) {
	struct CHAR_S {
		char a[7];
	} cs[13];
	FILE *faddr, *fgeo;
	int i, j, k, m, n, error, nphase1, nphase2, nsp[10], ewp[10], opt1, opt2, *pt;
	char line[100], addr[60], device[11], phase_ns[3], phase_ew[3],
		cs1[20], los[5], opt_los[5];
	double cv_12, cv_34, vc, opt_vc;
	errno_t err = 0;

	err = fopen_s(&faddr, address, "r");
	for (i = 0; i <= 2; i++)
		fgets(line, 100, faddr);
	err = fopen_s(&fgeo, geometry, "r");
	for (i = 0; i <= 2; i++)
		fgets(line, 100, fgeo);

	while ((fgets(line, 100, faddr)) != NULL) {
		sscanf_s(line, "%5c%56c%3c%10c", cs[0].a, _countof(cs[0].a), addr, _countof(addr), cs1, _countof(cs1),
			device, _countof(device));
		cs1[3] = '\0';
		device[10] = '\0';
		t[0][0] = atof(cs[0].a);

		fgets(line, 100, fgeo);
		sscanf_s(line, "%5c%3c%3c%5c%7c%5c%5c%7c%5c%5c%7c%5c%5c%7c%5c",
			cs[0].a, _countof(cs[0].a), phase_ns, _countof(phase_ns),
			phase_ew, _countof(phase_ew), cs[1].a, _countof(cs[1].a), cs[2].a, _countof(cs[2].a), cs[3].a, _countof(cs[3].a),
			cs[4].a, _countof(cs[4].a), cs[5].a, _countof(cs[5].a), cs[6].a, _countof(cs[6].a), cs[7].a, _countof(cs[7].a),
			cs[8].a, _countof(cs[8].a), cs[9].a, _countof(cs[9].a), cs[10].a, _countof(cs[10].a), cs[11].a, _countof(cs[11].a),
			cs[12].a, _countof(cs[12].a));

		if (t[0][0] != atof(cs[0].a)) {
			printf("Check node number in GEOMETRY. Addr#=%3.Of Geo#=%3.0f\n", t[0][0], atof(cs[0].a));
			exit(0);
		}

		for (i = 1; i <= 4; i++)
			for (j = 1; j <= 3; j++)
				lan[i][j] = atof(cs[(i - 1) * 3 + j].a);

		k = is_ints_id((int)t[0][0], badv, maxid);
		for (i = 1; i <= 4; i++)
			for (j = 1; j <= 3; j++)
				t[i][j] = badv[k].vol[3 * (i - 1) + j - 1];

		if (k < 0) {
			printf("Check int.# in the %s\n", geometry);
			exit(0);
		}

		for (i = 1; i <= 4; i++)
			if (fmod(lan[i][3], 10.) == 9) {	/* last digit is 9 ? */
				lan[i][3] = 0;
				t[i][3] = 0;
			}

		phase[1] = atoi(phase_ns);
		phase[2] = atoi(phase_ew);

		if (strncmp(device, "Signal", 6) == 0) {
			error = 0;
			if (check_vol_lan() != 0)	// lane and volumes are inconsistent
				error = 1;
			if (crt_phasing(phase[1], phase[2]) != 0) /* wrong phasing */
				error = 2;
			get_lane();

			if (phase[1] == 1 || phase[1] == 7 || phase[2] == 1 || phase[2] == 7)
			if (check_lt() == 0)	// need a separated LT lane
				error = 3;

			nphase1 = phase_num(phase[1]);
			nphase2 = phase_num(phase[2]);

			/* assign lane volumes */
			assign();
			for (i = 1; i <= 4; i++)
				t[i][0] = 0;

			rt_turn();

			cv_12 = critical_vol(1, 2, phase[1], k);
			cv_34 = critical_vol(3, 4, phase[2], k);
			int_los(cv_12 + cv_34, phase[1], phase[2], los);

			if (nphase1 + nphase2 == 2)
				vc = (cv_12 + cv_34) / 1500; // capacity definition. Need to investigate these numbers.
			else if (nphase1 + nphase2 == 3)
				vc = (cv_12 + cv_34) / 1425;
			else if (nphase1 + nphase2 >= 4)
				vc = (cv_12 + cv_34) / 1375;
			else;

			strcpy_s(badv[k].los, los);
			badv[k].vc = vc;
			if (badv[i].flag[0] == 0)
				badv[k].flag[0] = (error == 0) ? 0 : 600 + scen * 10 + error;
			badv[k].delay[0] = phase[1] * 10 + phase[2];	// record existing phasing

			opt_vc = vc;

			for (i = 1; i <= 4; i++)
				for (j = 1; j <= 3; j++)
					t[i][j] = badv[k].vol[3 * (i - 1) + j - 1];

			pt = &nsp[0];

			if (lan[3][1] == 0 && lan[3][2] == 0 && lan[3][3] == 0 &&
				t[1][1] <= 2 && t[2][3] <= 2 && t[4][2] <= 2) {
				nsp[0] = 3;
				nsp[1] = 7;
				nsp[2] = 8;
				nsp[3] = 9;
				if (t[2][1] > 90 && lan[2][1] == -1)
					take_away(7, pt);

				if (lan[2][1] == -1)
					take_away(9, pt);
			}
			else if (lan[4][1] == 0 && lan[4][2] == 0 && lan[4][3] == 0 &&
				t[1][3] <= 2 && t[2][1] <= 2 && t[3][2] <= 2) {
				nsp[0] = 3;
				nsp[1] = 7;
				nsp[2] = 8;
				nsp[3] = 9;
				if (t[1][1] > 90 && lan[1][1] == -1)
					take_away(7, pt);
				if (lan[1][1] == -1)
					take_away(9, pt);
			}
			else {
				nsp[0] = 6;
				for (m = 1; m <= 6; m++)
					nsp[m] = m;
				if (t[1][1] > 90 || t[2][1] > 90)
					take_away(1, pt);
				if (t[1][1] > 90 && t[2][1] > 90)
					take_away(3, pt);
				if (lan[1][1] == -1 && lan[2][1] == -1) {
					take_away(4, pt);
					take_away(5, pt);
					take_away(6, pt);
				}
			}

			pt = &ewp[0];
			if (lan[1][1] == 0 && lan[1][2] == 0 && lan[1][3] == 0 &&
				t[2][2] <= 2 && t[3][3] <= 2 && t[4][1] <= 2) {
				ewp[0] = 3;
				ewp[1] = 7;
				ewp[2] = 8;
				ewp[3] = 9;
				if (t[3][1] > 90 && lan[3][1] == -1)
					take_away(7, pt);
				if (lan[3][1] == -1)
					take_away(9, pt);
			}
			else if (lan[2][1] == 0 && lan[2][2] == 0 && lan[2][3] == 0 &&
				t[1][2] <= 2 && t[3][1] <= 2 && t[4][3] <= 2) {
				ewp[0] = 3;
				ewp[1] = 7;
				ewp[2] = 8;
				ewp[3] = 9;
				if (t[4][1] > 90 && lan[4][1] == -1)
					take_away(7, pt);
				if (lan[4][1] == -1)
					take_away(9, pt);
			}
			else {
				ewp[0] = 6;
				for (m = 1; m <= 6; m++)
					ewp[m] = m;
				if (t[3][1] > 90 || t[4][1] > 90)
					take_away(1, pt);
				if (t[3][1] > 90 && t[4][1] > 90)
					take_away(3, pt);

				if (lan[3][1] == -1 && lan[4][1] == -1) {
					take_away(4, pt);
					take_away(5, pt);
					take_away(6, pt);
				}
			}

			for (m = 1; m <= nsp[0]; m++)
			for (n = 1; n <= ewp[0]; n++) {
				phase[1] = nsp[m];
				phase[2] = ewp[n];

				for (i = 1; i <= 4; i++)
					for (j = 1; j <= 3; j++)
						t[i][j] = badv[k].vol[3 * (i - 1) + j - 1];
				nphase1 = phase_num(phase[1]);
				nphase2 = phase_num(phase[2]);
				/* assign lane volumes */
				assign();
				for (i = 1; i <= 4; i++)
					t[i][0] = 0;

				rt_turn();
				cv_12 = critical_vol(1, 2, phase[1], k);
				cv_34 = critical_vol(3, 4, phase[2], k);
				int_los(cv_12 + cv_34, phase[1], phase[2], los);

				if (nphase1 + nphase2 == 2)
					vc = (cv_12 + cv_34) / 1500;  // capacity hard coded. Need to investigate these numbers.
				else if (nphase1 + nphase2 == 3)
					vc = (cv_12 + cv_34) / 1425;
				else if (nphase1 + nphase2 >= 4)
					vc = (cv_12 + cv_34) / 1375;
				else {
					printf("%.0f the number of phasing is incrrect!\n", t[0][0]);
					exit(0);
				}

				if (opt_vc > vc) {
					opt_vc = vc;
					opt1 = phase[1];
					opt2 = phase[2];
					strcpy_s(opt_los, los);
				}
			}

			if (opt_vc < badv[k].vc) {
				strcpy_s(badv[k].opt_los, opt_los);
				badv[k].opt_vc = opt_vc;
				badv[k].delay[1] = opt1 * 10 + opt2; // record optimal phasing
				badv[k].flag[0] = (badv[i].flag[0] != 0) ? badv[i].flag[0] : (error == 0) ? 0 : 600 + scen * 10 + error;
			}
			else
				badv[k].opt_vc = 0;
		}
		else {
			badv[k].vc = 0;
			badv[k].opt_vc = 0;
			badv[k].flag[0] = 0;
		}
	}
	fclose(faddr);
	fclose(fgeo);
}

void take_away(int phase, int* pt) {
	int i, j;
	for (i = 1; i <= *(pt + 0); i++) {
		if (phase == *(pt + i)) {
			for (j = i; j < *(pt + 0); j++)
				*(pt + j) = *(pt + j + 1);
			--(*(pt + 0));
			return;
		}
	}
}

void cal_los_all(char* address, char* geometry, struct LEGSVOL ssv[], int pst1, int pst2) {
	struct CHAR_S {
		char a[7];
	} cs[13];
	FILE *faddr, *fgeo;
	int i, j, k, error, nphase1, nphase2;
	char line[100], addr[60], device[10], phase_ns[3], phase_ew[3],
		cs1[20], los[5];
	double cv_12, cv_34, vc;
	errno_t err = 0;

	err = fopen_s(&faddr, address, "r");
	for (i = 0; i <= 2; i++)
		fgets(line, 100, faddr);
	err = fopen_s(&fgeo, geometry, "r");
	for (i = 0; i <= 2; i++)
		fgets(line, 100, fgeo);

	while ((fgets(line, 100, faddr)) != NULL) {
		sscanf_s(line, "%5c%56c%3c%10c", cs[0].a, _countof(cs[0].a), addr, _countof(addr), cs1, _countof(cs1),
			device, _countof(device));
		t[0][0] = atof(cs[0].a);
		fgets(line, 100, fgeo);
		sscanf_s(line, "%5c%3c%3c%5c%7c%5c%5c%7c%5c%5c%7c%5c%5c%7c%5c", cs[0].a, _countof(cs[0].a), phase_ns, _countof(phase_ns),
			phase_ew, _countof(phase_ew), cs[1].a, _countof(cs[1].a), cs[2].a, _countof(cs[2].a), cs[3].a, _countof(cs[3].a),
			cs[4].a, _countof(cs[4].a), cs[5].a, _countof(cs[5].a), cs[6].a, _countof(cs[6].a), cs[7].a, _countof(cs[7].a),
			cs[8].a, _countof(cs[8].a), cs[9].a, _countof(cs[9].a), cs[10].a, _countof(cs[10].a), cs[11].a, _countof(cs[11].a),
			cs[12].a, _countof(cs[12].a));
		if (t[0][0] != atof(cs[0].a)) {
			printf("Check node number in GEOMETRY. Addr#=%3.0f Geo#=%3.0f\n", t[0][0], atof(cs[0].a));
			exit(0);
		}

		for (i = 1; i <= 4; i++)
		for (j = 1; j <= 3; j++)
			lan[i][j] = atof(cs[(i - 1) * 3 + j].a);

		k = is_ints_id((int)t[0][0], badv, maxid);
		for (i = 1; i <= 4; i++) {
			for (j = 1; j <= 3; j++)
				t[i][j] = badv[k].vol[3 * (i - 1) + j - 1];
		}

		if (k < 0) {
			printf("Check int.# in the %s\n", geometry);
			exit(0);
		}

		for (i = 1; i <= 4; i++) {
			if (fmod(lan[i][3], 10.) == 9){ // last digit is 9 ? 
				lan[i][3] = 0;
				t[i][3] = 0;
			}
		}

		ssv[k].id = badv[k].id;
		phase[1] = atoi(phase_ns);
		phase[2] = atoi(phase_ew);

		if (strncmp(device, "Signal", 6) == 0) {
			error = 0;
			if (check_vol_lan() != 0)	// lane and volumes are inconsistent
				error = 1;
			if (crt_phasing(phase[1], phase[2]) != 0) /* wrong phasing */
				error = 2;
			get_lane();
			// left turn check if no left turn phasing 
			if (phase[1] == 1 || phase[1] == 7 || phase[2] == 1 || phase[2] == 7)
			if (check_lt() == 0)	/* need a separated LT lane */
				error = 3;

			//assign lane volumes 
			assign();
			for (i = 1; i <= 4; i++)
				t[i][0] = 0;
			rt_turn();

			cv_12 = critical_vol(1, 2, phase[1], k);
			cv_34 = critical_vol(3, 4, phase[2], k);
			int_los(cv_12 + cv_34, phase[1], phase[2], los);
			nphase1 = phase_num(phase[1]);
			nphase2 = phase_num(phase[2]);

			if (nphase1 + nphase2 == 2)
				vc = (cv_12 + cv_34) / 1500;	// capacity hard coded. Need to investigate the numbers
			else if (nphase1 + nphase2 == 3)
				vc = (cv_12 + cv_34) / 1425;
			else if (nphase1 + nphase2 >= 4)
				vc = (cv_12 + cv_34) / 1375;
			else
				;

			ssv[k].vol[pst1] = int_los1(cv_12 + cv_34, phase[1], phase[2]);
			ssv[k].vol[pst2] = (int)floor(vc * 100 + 0.5);
			if (badv[i].flag[0] == 0)
				badv[k].flag[0] = (error == 0) ? 0 : 600 + (pst2 + 1) / 2 * 10 + error;
		}
		else {
			ssv[k].vol[pst1] = 0;
			ssv[k].vol[pst2] = 0;
			badv[k].flag[0] = 0;
		}
	}
	fclose(faddr);
	fclose(fgeo);
}

void cal_los (char* address, char* geometry, int scen) {
	struct CHAR_S {
		char a[7];
	} cs[13];
	FILE *faddr, *fgeo;
	int j, k, error, nphase1, nphase2;
	char line[100], addr[60], device[11], phase_ns[4], phase_ew[4],
		cs1[20], los[5];
	double cv_12, cv_34, geo[5], vc, air_q;
	errno_t err = 0;

	err = fopen_s(&faddr, address, "r");
	for (int i = 0; i <= 2; i++)
		fgets(line, 100, faddr);
	err = fopen_s(&fgeo, geometry, "r");
	for (int i = 0; i <= 2; i++)
		fgets(line, 100, fgeo);

	while ((fgets(line, 100, faddr)) != NULL) {
		sscanf_s(line, "%5c%56c%3c%10c", cs[0].a, _countof(cs[0].a), addr, _countof(addr),  cs1, _countof(cs1),
			device, _countof(device));
		cs1[3] = '\0';
		device[10] = '\0';
		t[0][0] = atof(cs[0].a);
		if (t[0][0] == 277)
			int a = 0;
		fgets(line, 100, fgeo);
		sscanf_s(line, "%5c%3c%3c%5c%7c%5c%5c%7c%5c%5c%7c%5c%5c%7c%5c", cs[0].a, _countof(cs[0].a), phase_ns, _countof(phase_ns),
			phase_ew, _countof(phase_ew), cs[1].a, _countof(cs[1].a), cs[2].a, _countof(cs[2].a), cs[3].a, _countof(cs[3].a), 
			cs[4].a, _countof(cs[4].a), cs[5].a, _countof(cs[5].a), cs[6].a, _countof(cs[6].a), cs[7].a, _countof(cs[7].a),
			cs[8].a, _countof(cs[8].a), cs[9].a, _countof(cs[9].a), cs[10].a, _countof(cs[10].a), cs[11].a, _countof(cs[11].a), 
			cs[12].a,_countof(cs[12].a));
		if (t[0][0] != atof(cs[0].a)) {
			printf("Check node number in GEOMETRY. Addr#=%3.0f Geo#=%3.0f\n", t[0][0], atof(cs[0].a));
			exit(0);
		}

		// i: 1 NB 2 SB 3 EB 4 WB
		for (int i = 1; i <= 4; i++) 
			// j: 1 LT 2: TH 3: RT
			for (j = 1; j <= 3; j++)
				// get lane chann for each movement
				lan[i][j] = atof(cs[(i - 1) * 3 + j].a);
		k = is_ints_id((int)t[0][0], badv, maxid);

		// i: 1 NB 2: SB 3: EB 4: WB
		for (int i = 1; i <= 4; i++) 
			// j: 1: LT 2: TH 3: RT
			for (j = 1; j <= 3; j++)
				t[i][j] = badv[k].vol[3 * (i - 1) + j - 1];

		/*printf("int# = %d\n",(int)t[0][0]);*/
		/*
		if (t[0][0]>0) {
		printf("lan[i][j]\n");
		for(i=1;i<=4;i++) {
		for(j=1;j<=3;j++)
		printf("%5.0f ",lan[i][j]);
		printf("\n");
		}
		}
		*/

		if (k < 0) {
			printf("Check int.# in the %s\n", geometry);
			exit(0);
		}

		for (int i = 1; i <= 4; i++) {
			if (fmod(lan[i][3], 10.) == 9) { // last digit is 9? assume RT is free flowing?*************
				lan[i][3] = 0;
				t[i][3] = 0;
			}
		}
		phase[1] = atoi(phase_ns);
		phase[2] = atoi(phase_ew);

		if (strncmp(device, "Signal", 6) == 0) {
			/*
			if(t[0][0] > 0) {
			printf("t[i][j]\n");
			for (i = 1; i <= 4; i++) {
			for(j = 1;j <= 3;j++)
			printf("%5.0f  ", t[i][j]);
			printf("\n");
			}
			}
			*/
			get_lane();
			convert_lan(geo); // convert LT and RT to TH lane
			error = 0;

			if (check_vol_lan() != 0) // lane and volumes are inconsistent
				error = 1;
			if (crt_phasing(phase[1], phase[2]) != 0) /* wrong phasing */
				error = 2;

			/* left turn check if no left turn phasing */
			if (phase[1] == 1 || phase[1] == 7 || phase[2] == 1 || phase[2] == 7)
			if (check_lt() == 0)	// need a separated LT lane 
				error = 3;

			nphase1 = phase_num(phase[1]);
			nphase2 = phase_num(phase[2]);

			air_q = airq_signal(geo, nphase1 + nphase2);

			/* assign lane volumes */
			assign();

			/*
			if(t[0][0]>0) {
			printf("after assign\n");
			for(i=1;i<=4;i++) {
			for(j=1;j<=3;j++)
			printf("%5.0f ",t[i][j]);
			printf("\n");
			}
			}
			*/

			for (int i = 1; i <= 4; i++)
				t[i][0] = 0;
			rt_turn();

			/*
			if(t[0][0]>0) (
			printf("after right-turn\n");
			for(i=1;i<=4;i++) {
			for(j=1;j<=3;j++)
			printf("%5.0f ",t[i][j]);
			printf("\n");
			}
			}
			*/

			cv_12 = critical_vol(1, 2, phase[1], k);
			cv_34 = critical_vol(3, 4, phase[2], k);
			int_los(cv_12 + cv_34, phase[1], phase[2], los);

			if (nphase1 + nphase2 == 2)
				vc = (cv_12 + cv_34) / 1500;	// capacity hard coded. Need to investigate on these numbers.
			else if (nphase1 + nphase2 == 3)
				vc = (cv_12 + cv_34) / 1425;
			else if (nphase1 + nphase2 >= 4)
				vc = (cv_12 + cv_34) / 1375;
			else {
				printf("%.0f the number of phasing is incrrect!\n", t[0][0]);
				exit(0);
			}

			strcpy_s(badv[k].los, los);
			badv[k].vc = vc;
			if (badv[k].flag[0] == 0)
				badv[k].flag[0] = (error == 0) ? 0 : 600 + scen * 10 + error;
			badv[k].delay[0] = air_q;
		}
		else {
			air_q = airq_no_signal(device);
			strcpy_s(badv[k].los, "0\0");
			badv[k].vc = 0;
			badv[k].flag[0] = 0;
			badv[k].delay[0] = 10;
		}
	}
	fclose(faddr);
	fclose(fgeo);
}

// check consistency between lane chann and volume.
int check_vol_lan() {
	int i, j, n;

	n = 0;
	for (i = 1; i <= 4; i++) { // NB SB EB WB
		for (j = 1; j <= 3; j++) // LT TH RT
			// if no lane for a movement but has volume
			// ignore the volume and return error number 1
			if (lan[i][j] == 0 && t[i][j] > 0)	{
				t[i][j] = 0;
				n = 1;
		}
	}
	return n;
}

void get_turns(char *cs, int i) {
	switch (i) {
	case 1:
		strcpy_s(cs, sizeof(cs), "left turn");
		break;
	case 2:
		strcpy_s(cs, sizeof(cs), "through");
		break;
	case 3:
		strcpy_s(cs, sizeof(cs), "right turn");
		break;
	}
}

/* check if phasings are correct. phasing 7-9 means T intersection
if phasing is correct, return 0; otherwise return 1
*/
int crt_phasing(int p1, int p2) {
	int i, j;
	i = 0;
	j = 0;

	// T intersection but has conflicted turn lanes
	if (p1 == 7 || p1 == 8 || p1 == 9)
	if (!((t[1][3] == 0 && t[2][1] == 0) || (t[1][1] == 0 && t[2][3] == 0)))
		i = 1;

	// protected phase but no exclusive LT lane
	if (p1 == 4 || p1 == 5 || p1 == 6 || p1 == 9)
	if (!(lan[1][1] >= 0 && lan[2][1] >= 0))
		i = 1;

	// T intersection but has conflicted turn lanes
	if (p2 == 7 || p2 == 8 || p2 == 9)
	if (!((t[3][1] == 0 && t[4][3] == 0) || (t[3][3] == 0 && t[4][1] == 0)))
		j = 1;

	// protected phase but no exclusive LT lane
	if (p2 == 4 || p2 == 5 || p2 == 6 || p2 == 9)
	if (!(lan[3][1] >= 0 && lan[4][1] >= 0))
		j = 1;

	if (i == 0 && j == 0)
		return 0;
	else
		return 1;
}

/* recode lane configurations.through lane is coded as a positive number.
left and right turns may be positive or negative. negative number means it has
one land shared with others. positive number means it is separated.
*/
void get_lane() {
	int i, j, k, l1[4], l2[4], lf_lan;

	for (k = 1; k <= 4; k++) { // k: 1 NB 2: SB 3: EB 4: WB
		for (j = 1; j <= 3; j++) // j: 1: LT 2: TH 3: RT
			l1[j] = (int)lan[k][j];
		for (i = 1; i <= 3; i++) {
			if (l1[i] != 0)
				l2[i] = 1;
			else
				l2[i] = 0;
			j = 10;
			while (l1[i] / j != 0) {
				l2[i]++;
				j = j * 10;
			}
		}

		j = 1;
		while (l1[2] / j != 0) {
			lf_lan = l1[2] / j;
			j = j * 10;
		}
		i = (int)fmod((double)l1[1], 10.);
		if (i == lf_lan && l1[2] != 0)
			l2[1] = -l2[1];
		j = 1;
		while (l1[3] / j != 0) {
			lf_lan = l1[3] / j;
			j = j * 10;
		}

		i = (int)fmod((double)l1[2], 10.);
		if (i == lf_lan && l1[2] != 0)
			l2[3] = -l2[3];
		if (l1[2] == 0 && l1[1] != 0 && l1[3] != 0) {
			j = 1;
			while (l1[3] / j != 0) {
				lf_lan = l1[3] / j;
				j = j * 10;
			}
			i = (int)fmod(l1[1], 10.);
			if (i == lf_lan) {
				l2[3] = -l2[3];
				l2[1] = -l2[1];
			}
		}
		if (l1[2] == 0 && l1[1] != 0 && l1[3] != 0) {
			j = 1;
			while (l1[3] > j * 10)
				j = j * 10;
			if (fmod((double)l1[1], 10.) == l1[3] / j) {
				l2[1] = -l2[1];
				l2[2] = 1;
				l2[3] = -l2[3];
			}
		}

		for (i = 1; i <= 3; i++)
			lan[k][i] = l2[i];
	}
}

/* if a separated left turn phasing is needed return 0; otherwise return 1 */
int check_lt() {
	int i, j;
	double opv[5], e[5], total, ratio[5];
	total = 0;

	// calculate total intersection volume
	for (i = 1; i <= 4; i++) // NB SB EB WB
		for (j = 1; j <= 3; j++) // LT TH RT
			total = total + t[i][j];

	opv[1] = t[2][2] + t[2][3]; // SB TH RT
	opv[2] = t[1][2] + t[1][3]; // NB TH RT
	opv[3] = t[4][2] + t[4][3]; // WB TH RT
	opv[4] = t[3][2] + t[3][3]; // EB TH RT

	ratio[1] = (opv[1] + opv[2] + t[1][1] + t[2][1]) / total; // NB-SB %
	ratio[3] = (opv[3] + opv[4] + t[3][1] + t[4][1]) / total; // EB-WB %
	ratio[2] = ratio[1];
	ratio[4] = ratio[3];

	// calculate critical left turn volume.
	for (i = 1; i <= 4; i++) {
		e[i] = floor(ratio[i] * 1200 - opv[i] + 0.5);
		if (e[i]<0)
			e[i] = 0;
		// if left volume > critical lt volume + 90, it needs a LT phase
		if (t[i][1] > (e[i] + 90))
			return 0;
	}

	return 1;
}

void assign() {
	int i, j, a[5];
	double lt[5], opv[5];

	opv[1] = t[2][2] + t[2][3];
	opv[2] = t[1][2] + t[1][3];
	opv[3] = t[4][2] + t[4][3];
	opv[4] = t[3][2] + t[3][3];

	for (i = 1; i <= 4; i++) {
		lt[i] = t[i][1];
		pct[i] = 1;
	}

	for (i = 1; i <= 2; i++) {
		if (lan[i][1] == -1) {
			a[i] = pce_cat(i, phase[1]);
			pct[i] = pct_lt(opv[i], a[i]) / 100.;
			t[i][1] = floor(pct[i] * t[i][1] + 0.5);
		}
	}

	for (i = 3; i <= 4; i++) {
		if (lan[i][1] == -1) {
			a[i] = pce_cat(i, phase[2]);
			pct[i] = pct_lt(opv[i], a[i]) / 100.;
			t[i][1] = floor(pct[i] * t[i][1] + 0.5);
		}
	}

	for (i = 1; i <= 4; i++) {
		j = get_case(i);
		switch (j) {
		case 1:
			lan_separate(lt, i, 0, 0, 0);
			break;
		case 2: case 8:
			lan_share(lt, i, 1, 2, 3);
			break;
		case 3:
			lan_separate(lt, i, 0, 0, 1);
			break;
		case 4:
			lan_separate(lt, i, 0, 1, 0);
			break;
		case 6:
			lan_separate(lt, i, 0, 1, 1);
			break;
		case 7:
			lan_separate(lt, i, 1, 0, 0);
			break;
		case 9:
			lan_separate(lt, i, 1, 0, 1);
			break;
		case 10:
			lan_separate(lt, i, 1, 1, 0);
			break;
		case 11: case 17: case 18:
			break;
		case 12:
			lan_separate(lt, i, 1, 1, 1);
			break;
		case 13: case 15:
			lan_share(lt, i, 3, 2, 1);
			break;
		case 14:
			rt_lt_share(lt, i);
			break;
		default:
			break;
		}
	}
	for (i = 1; i <= 4; i++) {
		t[0][i] = lt[i];
	}
}

int pce_cat(int i, int p) {
	if (lan[i][1] == -1)
	if (p == 1 || p == 7)
		return 1;		// no separated LT lane & no turn phase no 
	else return 2;		// no separated LT lane with turn phase
	else
	if (p == 1 || p == 7)
		return 3;		// separated LT lane but no turn phase 
	else return 4;		// separated LT lane with turn phase 
}

int pct_lt(double i, int j) {
	switch (j) {
	case 1: case 3:
		if (i <= 299)
			return 100;
		else if (i <= 599)
			return 200;
		else if (i <= 999)
			return 400;
		else
			return 600;
		break;
	case 2:
		return 120;
		break;
	case 4:
		return 105;
		break;
	}

	// in case j  is not within 1..4
	return -1;
}

int get_case(int i) {
	if (lan[i][1] > 0) {
		if (lan[i][2] > 0) {
			if (lan[i][3] > 0)
				return 1;
			else if (lan[i][3]<0)
				return 2;
			else return 3;
		}
		else {
			if (lan[i][3]>0)
				return 4;
			else if (lan[i][3]<0)
				return 5;
			else
				return 6;
		}
	}
	else if (lan[i][1] == 0) {
		if (lan[i][2]>0) {
			if (lan[i][3] > 0)
				return 7;
			else if (lan[i][3]<0)
				return 8;
			else return 9;
		}
		else {
			if (lan[i][3]>0)
				return 10;
			else if (lan[i][3]<0)
				return 11;
			else return 12;
		}
	}
	else {
		if (lan[i][2]>0) {
			if (lan[i][3] > 0)
				return 13;
			else if (lan[i][3]<0)
				return 14;
			else return 15;
		}
		else {
			if (lan[i][3]>0)
				return 16;
			else if (lan[i][3] < 0)
				return 17;
			else
				return 18;
		}
	}
}

/* a - LT lane, b - Thru lane, c - RT lane.
1 - lane not exits, 0 - separated.
*/
void lan_separate(double lt[], int i, int a, int b, int c) {
	if (a == 0) {
		if (lan[i][1] > 2)
			t[i][1] = floor(t[i][1] / lan[i][1] + 0.5);
		else if (lan[i][1] == 2)
			t[i][1] = floor(0.55*lt[i] + 0.5);
		else
			t[i][1] = lt[i];
	}
	if (b == 0)
		t[i][2] = floor(t[i][2] / lan[i][2] + 0.5);
	if (c == 0)
		t[i][3] = floor(t[i][3] / fabs(lan[i][3]) + 0.5);
}

void int_los(double l, int phase1, int phase2, char* los) {
	int p1, p2, p;

	p1 = phase_num(phase1);
	p2 = phase_num(phase2);
	p = p1 + p2;

	switch (p) {
	case 2:
		if (l <= 900)
			strcpy_s(los, sizeof(los), "A");
		else if (l <= 1050)
			strcpy_s(los, sizeof(los), "B");
		else if (l <= 1200)
			strcpy_s(los, sizeof(los), "C");
		else if (l <= 1350)
			strcpy_s(los, sizeof(los), "D");
		else if (l <= 1500)
			strcpy_s(los, sizeof(los), "E");
		else
			strcpy_s(los, sizeof(los), "F");
		break;
	case 3:
		if (l <= 855)
			strcpy_s(los, sizeof(los), "A");
		else if (l <= 1000)
			strcpy_s(los, sizeof(los), "B");
		else if (l <= 1140)
			strcpy_s(los, sizeof(los), "C");
		else if (l <= 1275)
			strcpy_s(los, sizeof(los), "D");
		else if (l <= 1425)
			strcpy_s(los, sizeof(los), "E");
		else
			strcpy_s(los, sizeof(los), "F");
		break;
	case 4: case 5: case 6: case 7: case 8:
		if (l <= 825)
			strcpy_s(los, sizeof(los), "A");
		else if (l <= 965)
			strcpy_s(los, sizeof(los), "B");
		else if (l <= 1100)
			strcpy_s(los, sizeof(los), "C");
		else if (l <= 1225)
			strcpy_s(los, sizeof(los), "D");
		else if (l <= 1375)
			strcpy_s(los, sizeof(los), "E");
		else
			strcpy_s(los, sizeof(los), "F");
		break;
	}
}

int int_los1(double l, int phase1, int phase2) {
	int p1, p2, p;

	p1 = phase_num(phase1);
	p2 = phase_num(phase2);
	p = p1 + p2;
	switch (p) {
	case 2:
		if (l <= 900)
			return 1;
		else if (l <= 1050)
			return 2;
		else if (l <= 1200)
			return 3;
		else if (l <= 1350)
			return 4;
		else if (l <= 1500)
			return 5;
		else
			return 6;
		break;
	case 3:
		if (l <= 855)
			return 1;
		else if (l <= 1000)
			return 2;
		else if (l <= 1140)
			return 3;
		else if (l <= 1275)
			return 4;
		else if (l <= 1425)
			return 5;
		else
			return 6;
		break;
	case 4: case 5: case 6: case 7: case 8:
		if (l <= 825)
			return 1;
		else if (l <= 965)
			return 2;
		else if (l <= 1100)
			return 3;
		else if (l <= 1225)
			return 4;
		else if (l <= 1375)
			return 5;
		else
			return 6;
		break;
	default:
		return -1;
	}
}

int phase_num(int n) {
	switch (n) {
	case 1: case 7:
		return 1;
		break;
	case 2: case 3: case 4: case 5: case 8: case 9:
		return 2;
		break;
	case 6:
		return 3;
		break;
	default:
		return 0;
		break;
	}
}

int critical_vol(int i, int j, int phase, int pos) {
	int k, m, r, l;
	double a[4], b, at[5][5];

	switch (phase) {
	case 1:
		if (lan[i][1] == -1 && lan[i][2] == 1) {
			a[1] = (t[i][1] > t[j][1]) ? 0 : t[j][1] - t[i][1];
			a[1] = a[1] + t[i][2];
			if (lan[i][3] != -1)
				b = t[i][3] + t[j][1];
			else
				b = 0;
			a[1] = (a[1] > b) ? a[1] : b;
		}
		else
			a[1] = t[i][2] + t[j][1];

		if (lan[j][1] == -1 && lan[j][2] == 1) {
			a[2] = (t[j][1] > t[i][1]) ? 0 : t[i][1] - t[j][1];
			a[2] = a[2] + t[j][2];
			if (lan[j][3] != -1)
				b = t[j][3] + t[i][1];
			else
				b = 0;
			a[2] = (a[2] > b) ? a[2] : b;
		}
		else
			a[2] = t[j][2] + t[i][1];
		/*printf("%.0f %.0f\n",a[1],a[2]);*/
		return	(int)((a[1] > a[2]) ? a[1] : a[2]);
		break;

	case 2:
		a[1] = (t[i][1] > t[i][2]) ? t[i][1] : t[i][2];
		a[2] = (t[j][1] > t[j][2]) ? t[j][1] : t[j][2];
		/*printf("%.0f	%.0f\n",a[1],a[2]);*/
		return (int)(a[1] + a[2]);
		break;

	case 3:
		a[1] = (t[i][1] > t[j][1]) ? t[i][1] : t[j][1];
		if (t[i][1] > t[j][1])	{
			t[i][2] = t[i][2] - a[1];
			a[2] = (t[i][2] + t[j][1] > t[j][2]) ? t[i][2] + t[j][1] : t[j][2];
		}
		else	{
			t[j][2] = t[j][2] - a[1];
			a[2] = (t[j][2] + t[i][1] > t[i][2]) ? t[j][2] + t[i][1] : t[i][2];
		}
		/*printf("%.0f %.0f\n",a[1],a[2]);*/
		return (int)(a[1] + a[2]);
		break;

	case 4:
		a[1] = (t[i][1] > t[j][1]) ? t[i][1] : t[j][1];
		a[2] = (t[i][2] > t[j][2]) ? t[i][2] : t[j][2];

		/*printf("%.0f %.0f\n",a[1],a[2]);*/
		return (int)(a[1] + a[2]);
		break;

	case 5:
		m = 1;
		a[m++] = (t[i][1] > t[j][1]) ? t[j][1] : t[i][1];
		if (t[i][1] > t[j][1]) {
			b = t[j][2] + t[i][1] - t[j][1];
			a[m++] = (t[i][2] > b) ? t[i][2] : b;
		}
		else {
			b = t[i][2] + t[j][1] - t[i][1];
			a[m++] = (t[j][2] > b) ? t[j][2] : b;
		}
		b = 0;
		for (k = 1; k < m; k++)
			b = b + a[k];
		return (int)b;
		break;

	case 6:
		m = 1;
		a[m++] = (t[i][1]>t[j][1]) ? t[j][1] : t[i][1];
		if (t[i][1] > t[j][1]) {
			a[m] = t[i][1] - t[j][1];
			t[i][2] = t[i][2] - a[m];
		}
		else {
			a[m] = t[j][1] - t[i][1];
			t[j][2] = t[j][2] - a[m];
		}
		m++;
		a[m++] = (t[i][2] > t[j][2]) ? t[i][2] : t[j][2];
		b = 0;
		for (k = 1; k < m; k++)
			b = b + a[k];
		/*printf("%.0f %.0f %.0f\n",a[1],a[2],a[3]);	*/
		return (int)b;
		break;

	case 7: case 8: case 9:
		for (r = 1; r <= 4; r++)
		for (l = 1; l <= 3; l++)
			at[r][l] = badv[pos].vol[3 * (r - 1) + l - 1];

		if (at[i][1] <= 2 && at[j][3] <= 2) {
			r = i;
			l = j;
		}
		else if (at[i][3] <= 2 && at[j][1] <= 2) {
			r = j;
			l = i;
		}
		else {
			r = 0;
			l = 0;
			printf("#%3.0f check phasing & turning volumes\n", t[0][0]);
			exit(0);
		}

		if (phase == 7) {
			a[1] = (t[1][2] > t[l][1] + t[r][2]) ? t[l][2] : t[l][1] + t[r][2];
			/*printf("%.0f %.0f\n",a[1],a[2]);*/
			return (int)a[1];
		}

		if (phase == 8) {
			if (t[l][1]<t[l][2]) {
				a[1] = t[l][1];
				a[2] = (t[l][2] - t[l][1] > t[r][2]) ? t[l][2] - t[l][1] : t[r][2];
				/*printf("%.0f %.0f\n",a[1],a[2]);*/
				return (int)(a[1] + a[2]);
			}
			else
				return (int)(t[l][1] + t[r][2]);
		}

		if (phase == 9) {
			a[1] = t[l][1];
			a[2] = ((t[l][2] - t[l][1]) > t[r][2]) ? (t[l][2] - t[l][1]) : t[r][2];
			/*printf("%.0f %.0f\n",a[1],a[2]);*/
			return (int)(a[1] + a[2]);
		}
		break;
	}
	return -1;
}

/* l - separated lane, m - Thru lane, n - shared lane
1 - LT lane, 2 - THRU lane, 3 - RT lane
*/
void lan_share(double lt[], int i, int l, int m, int n) {
	double vol;

	if (lan[i][l] != 0) {
		if (l == 1) {
			if (fabs(lan[i][1]) == 2)
				t[i][1] = floor(0.55 * t[i][1] + 0.5);
			else
				t[i][l] = floor(lt[i] / fabs(lan[i][l]) + 0.5);
		}
		else
			t[i][l] = floor(0.5 + t[i][l] / fabs(lan[i][l]));
	}

	vol = floor(0.5 + (t[i][m] + t[i][n]) / (lan[i][m] + fabs(lan[i][n]) - 1));

	if (lan[i][m] == 1 && lan[i][n] == -1) {
		if (n == 1)
			t[i][n] = lt[i];
		t[i][m] = t[i][m] + t[i][n];
	}
	else if (lan[i][m] == 1 && lan[i][n] < -1) {
		if (t[i][m] > vol) {
			t[i][n] = floor(0.5 + t[i][n] / (fabs(lan[i][n]) - 1));
			t[i][m] = t[i][m];
		}
		else {
			t[i][m] = vol;
			t[i][n] = vol;
		}
	}
	else if (lan[i][m] > 1 && lan[i][n] == -1) {
		t[i][m] = (t[i][n] > vol) ? floor(0.5 + t[i][m] / (lan[i][m] - 1)) : vol;
		if (n == 3 && t[i][3] > t[i][2])
			t[i][2] = t[i][3];
		if (n == 1)
			t[i][n] = lt[i];
	}
	else {
		if (vol * fabs(lan[i][n]) < t[i][n]) {
			t[i][m] = floor(0.5 + t[i][m] / (lan[i][m] - 1));
			t[i][n] = floor(0.5 + t[i][n] / fabs(lan[i][n]));
		}
		else if (vol*lan[i][m] < t[i][m]) {
			t[i][m] = floor(0.5 + t[i][m] / lan[i][m]);
			t[i][n] = floor(0.5 + t[i][n] / (fabs(lan[i][n]) - 1));
		}
		else {
			t[i][m] = vol;
			t[i][n] = vol;
		}
	}
	if (l == 3 && lan[i][3] < 0 && t[i][2] < t[i][3])
		t[i][2] = t[i][3];
}

void rt_lt_share(double lt[], int i) {
	int j;
	double vol, a, b;

	if (lan[i][1] == -1)
		a = t[i][1] + t[i][2] + t[i][3];
	else
		a = lt[i] + t[i][2] + t[i][3];

	b = fabs(lan[i][1]) + fabs(lan[i][2]) + fabs(lan[i][3]) - 2;
	vol = (b == 0) ? 0 : floor(0.5 + a / b);

	if (fabs(lan[i][1])*vol < t[i][1]) {
		lan[i][1] = fabs(lan[i][1]);
		if (fabs(lan[i][3])*vol < t[i][3]) { // separated LT & RT 
			lan[i][3] = fabs(lan[i][3]);
			t[i][1] = lt[i];
			for (j = 2; j <= 3; j++)
				t[i][j] = floor(0.5 + t[i][j] / fabs(lan[i][j]));
			if (lan[i][1] == -1 && lan[i][2] == 1 && t[i][3] == -1)
				t[i][2] = t[i][2] + t[i][3];
		}
		else {	// separated LT
			lan[i][2] = (lan[i][2] != 1) ? lan[i][2] - 1 : 1;
			lan_share(lt, i, 1, 2, 3);
		}
	}
	else {
		if (fabs(lan[i][3]) * vol < t[i][3]) {	/* separated RT */
			lan[i][3] = fabs(lan[i][3]);
			lan[i][2] = (lan[i][2] != 1) ? lan[i][2] - 1 : 1;
			lan_share(lt, i, 3, 2, 1);
		}
		else {	/* shared RT & LT */
			if ((fabs(lan[i][1]) - 1) * vol > t[i][1]) {
				if ((fabs(lan[i][3]) - i)* vol > t[i][3]) {  // separate LT & RT
					lan[i][1] = fabs(lan[i][1]) - 1;
					lan[i][3] = fabs(lan[i][3]) - 1;
					t[i][1] = t[i][1] / lan[i][1];
					t[i][3] = t[i][3] / lan[i][3];
					t[i][2] = t[i][2] / lan[i][2];
				}
				else {		// separate LT
					lan[i][1] = fabs(lan[i][1]) - 1;
					lan_share(lt, i, 1, 2, 3);
				}
			}
			else {
				if ((fabs(lan[i][3]) - 1) * vol > t[i][3]) {  // separate RT
					lan[i][3] = fabs(lan[i][3]) - 1;
					lan_share(lt, i, 3, 2, 1);
				}
				else {
					if (lan[i][1] == -1 && lan[i][2] == 1 && lan[i][3] == -1) {
						t[i][2] = t[i][1] + t[i][2] + t[i][3];
						t[i][1] = lt[i];
					}
					else {
						t[i][1] = (lt[i] < vol) ? lt[i] : vol;
						t[i][2] = vol;
						t[i][3] = (t[i][3] < vol) ? t[i][3] : vol;

					}
				}
			}

		}
	}
}

int opdir(int i) {
	switch (i) {
	case 1:
		return 2;
		break;
	case 2:
		return 1;
		break;
	case 3:
		return 4;
		break;
	case 4:
		return 3;
		break;
	default:
		return 0; // need to investigate which return value is good if i is not in 1..4.
	}
}

double rt_critical(int i, int op, int phase) {
	double f;

	if (rt_redt > 0)	/* No extra RT reduction */
		return 1. - rt_redt;

	if (phase == 2 || phase == 4 || phase == 6 || phase == 9 ||
		((phase == 1 || phase == 3 || phase == 5) &&
		fabs(lan[op][1]) + lan[op][2] + fabs(lan[op][3]) == 0)) {
		if (0.5 * t[i][1] >= t[i][3])	/* 100% reduction */
			f = 0;
		else {
			if (0.5 * t[i][1] >= t[i][3] * 0.3)	/* %50 Lt/Rt reduction */
				f = 1 - 0.5 * t[i][1] / t[i][3];
			else		/* rt_redt reduction */
				f = 1 - fabs(rt_redt);
		}
	}
	else
		f = 1 - fabs(rt_redt);	/* rt_redt reduction */
	return f;
}

void rt_turn() {
	int i, j;
	//double rt_critical();

	for (i = 1; i <= 4; i++) {
		if (lan[i][3] > 0) {
			j = get_case(i);
			switch (j) {
			case 1: case 4: case 7: case 10:
				t[i][3] = rt_critical(i, opdir(i), phase[(int)floor(0.5 + i / 2.)])	* t[i][3];
				if (t[i][2] < t[i][3]) {
					t[i][2] = t[i][3];
					lan[i][1] = fabs(lan[i][1]);
				}
				break;
			case 13:
				t[i][3] = rt_critical(i, opdir(i), phase[(int)floor(0.5 + i / 2.)]); // need to double check this statement.
				if (t[i][2] < t[i][3]) {
					t[i][2] = t[i][3];
					lan[i][1] = fabs(lan[i][1]);
				}
				break;
			}
		}
	}
}

void convert_lan(double geo[]) {
	int i;
	double lanl, lant, lanr;

	for (i = 1; i <= 4; i++) { // i: 1: NB 2: SB 3: EB 4: WB
		lanl = lan[i][1];
		lant = lan[i][2];
		lanr = lan[i][3];

		if (t[i][2] == 0 && lan[i][1] < 0 && lan[i][3] < 0) {
			lant = 0;
			if (lan[i][1] < lan[i][3]) {
				lanl = fabs(lan[i][1]);
				lanr = fabs(lan[i][3]) - 1;
			}
			else {
				lanl = fabs(lan[i][3]);
				lanr = fabs(lan[i][1]) - 1;
			}
		}
		else {
			if (lan[i][1] < 0)
				lanl = fabs(lan[i][1]) - 1;
			if (lan[i][3] < 0)
				lanr = fabs(lan[i][3]) - 1;
		}
		geo[i] = (lanl + lanr) / 4.0 + lant;
	}
}

double airq_signal(double geo[], int phasing) {
	int i, j;
	double cap[5], vol[5], dirct_ns_vol, dirct_ew_vol, avg, d[5], total_vol, total_delay;

	for (i = 1; i <= 4; i++) {
		cap[i] = 0;
		vol[i] = 0;
	}

	for (i = 1; i <= 4; i++)
	for (j = 1; j <= 3; j++)
		vol[i] += t[i][j];

	dirct_ns_vol = (geo[1] + geo[2] == 0) ? 0 : (vol[1] + vol[2]) / (geo[1] + geo[2]);
	dirct_ew_vol = (geo[3] + geo[4] == 0) ? 0 : (vol[3] + vol[4]) / (geo[3] + geo[4]);

	if (dirct_ns_vol + dirct_ew_vol == 0) return 0;

	avg = dirct_ns_vol / (dirct_ns_vol + dirct_ew_vol);
	for (i = 1; i <= 2; i++)
		cap[i] = (1700 - (150 * phasing))*geo[i] * avg;
	for (i = 3; i <= 4; i++)
		cap[i] = (1700 - (150 * phasing))*geo[i] * (1 - avg);
	for (i = 1; i <= 4; i++)
		d[i] = delay(vol[i] / cap[i]);

	total_vol = 0;
	total_delay = 0;

	for (i = 1; i <= 4; i++) {
		total_vol = total_vol + vol[i];
		total_delay = total_delay + vol[i] * d[i];
	}

	return (total_delay / total_vol >= 0) ? total_delay / total_vol : 0;
}

int delay(double vc) {
	if (vc < 0.27)
		return 0;
	else if (vc < 0.57)
		return 3;
	else if (vc < 0.62)
		return 5;
	else if (vc < 0.67)
		return 10;
	else if (vc  <0.72)
		return 15;
	else if (vc < 0.77)
		return 20;
	else if (vc < 0.82)
		return 25;
	else if (vc < 0.87)
		return 35;
	else if (vc < 0.92)
		return 40;
	else if (vc < 0.97)
		return 50;
	else if (vc < 1.02)
		return 60;
	else if (vc < 1.10)
		return 80;
	else if (vc <1.20)
		return 100;
	else
		return 120;
}

double airq_no_signal(char* cs) {
	int i, j, *ip, *jp;
	double d[5], vol[5], total_vol, total_delay;

	for (i = 1; i <= 4; i++) {
		vol[i] = 0;
		for (j = 1; j <= 3; j++)
			vol[i] += t[i][j];
	}

	ip = &i;
	jp = &j;
	get_type(cs, ip, jp);

	switch (i) {
	case 1:
		d[1] = 10;
		d[2] = 10;
		break;
	case 2:
		d[1] = unctrl(1, vol);
		d[2] = unctrl(2, vol);
		break;
	default:
		d[1] = 0;
		d[2] = 0;
		break;
	}

	switch (j) {
	case 1:
		d[3] = 10;
		d[4] = 10;
		break;
	case 2:
		d[3] = unctrl(3, vol);
		d[4] = unctrl(4, vol);
		break;
	default:
		d[3] = 0;
		d[4] = 0;
		break;
	}

	total_vol = 0;
	total_delay = 0;
	for (i = 1; i <= 4; i++){
		total_vol = total_vol + vol[i];
		total_delay = total_delay + d[i] * vol[i];
	}
	return (total_vol == 0 || total_delay / total_vol < 0) ? 0 : total_delay / total_vol;
}

double unctrl(int n, double vol[]) {
	double cap, vc;

	cap = (lan[n][2] == 1 && t[n][2] == 0) ? 0 : 1500 * lan[n][2];
	vc = (cap == 0) ? 0 : vol[n] / cap;
	return delay(vc);
}

void get_type(char* cs, int* i, int* j) {
	if (strncmp(cs, "Flash", 5) == 0) {
		if (strncmp(cs, "Flash    ", 9) == 0 || strncmp(cs, "Flash 4W ", 9) == 0
			|| strncmp(cs, "Flash 5W ", 9) == 0) {
			*i = 1;
			*j = 1;
		}
		else if (strncmp(cs, "Flash N  ", 9) == 0 || strncmp(cs, "Flash S  ", 9) == 0 ||
			strncmp(cs, "Flash N-S", 9) == 0) {
			*i = 1;
			*j = 2;
		}
		else if (strncmp(cs, "Flash E  ", 9) == 0 || strncmp(cs, "Flash W  ", 9) == 0 ||
			strncmp(cs, "Flash E-W", 9) == 0) {
			*i = 2;
			*j = 1;
		}
		else {
			*i = 1;
			*j = 1;
		}
	}
	else if (strncmp(cs, "Yield", 5) == 0) {
		if (strncmp(cs, "Yield    ", 9) == 0 || strncmp(cs, "Yield 4W ", 9) == 0
			|| strncmp(cs, "Yield 5W ", 9) == 0) {
			*i = 1;
			*j = 1;
		}
		else if (strncmp(cs, "Yield N  ", 9) == 0 ||
			strncmp(cs, "Yield S  ", 9) == 0 || strncmp(cs, "Yield N-S", 9) == 0) {
			*i = 1;
			*j = 2;
		}
		else if (strncmp(cs, "Yield E  ", 9) == 0 ||
			strncmp(cs, "Yield W  ", 9) == 0 || strncmp(cs, "Yield E-W", 9) == 0) {
			*i = 2;
			*j = 1;
		}
		else {
			*i = 1;
			*j = 1;
		}
	}
	else if (strncmp(cs, "Stop", 4) == 0) {
		if (strncmp(cs, "Stop    ", 8) == 0 || strncmp(cs, "Stop 4W ", 8) == 0
			|| strncmp(cs, "Stop 5W ", 8) == 0) {
			*i = 1;
			*j = 1;
		}
		else if (strncmp(cs, "Stop N  ", 8) == 0 ||
			strncmp(cs, "Stop S  ", 8) == 0 || strncmp(cs, "Stop N-S", 8) == 0) {
			*i = 1;
			*j = 2;
		}
		else if (strncmp(cs, "Stop E  ", 8) == 0 ||
			strncmp(cs, "Stop W  ", 8) == 0 || strncmp(cs, "Stop E-W", 8) == 0) {
			*i = 2;
			*j = 1;
		}
		else {
			*i = 1;
			*j = 1;
		}
	}
	else if (strncmp(cs, "Uncont", 6) == 0){
		*i = 2;
		*i = 2;
	}
	else {
		*i = 0;
		*j = 0;
	}
}

void menu_0() {
	system("cls");
	printf("\n");
	printf("    *******************************************************************\n");
	printf("    *  INT                                                            *\n");
	printf("    *                   CITY OF BELLEVUE                              *\n");
	printf("    *                PLANNING DEPARTMENT                              *\n");
	printf("    *                                                                 *\n");
	printf("    *                                                                 *\n");
	printf("    *      ESTIMATION OF FUTURE TURNING MOVEMENT                      *\n");
	printf("    *              AVERAGE VEHICLE DELAY                              *\n");
	printf("    *                      AND                                        *\n");
	printf("    *           INTERSECTION CAPACITY ANALYSIS (212)                  *\n");
	printf("    *                                                                 *\n");
	printf("    *                                                                 *\n");
	printf("    *                  Originally  Created by                         *\n");
	printf("    *               Dave Tallent & Jun Ma, 1991                       *\n");
	printf("    *                                                                 *\n");
	printf("    *                 Migrated to Windows Platform by                 *\n");
	printf("    *                   Hu Dong, 2013                                 *\n");
	printf("    *******************************************************************\n");

	Sleep(4000);
	system("cls");
}

void menu_1(char cs[], struct LEGSVOL* lg, struct LEGSVOL* lg0, struct LEGSVOL* lg1, struct LEGSVOL* l,
struct LEGSVOL* l0, char* cs1, char* cs2, char* cs3, char* cs4, char* cs5, char* address,
	char* geometry, char* title, int pos) {

	char c[10];
	int i, n;
	system("cls");
	printf("\n");
	printf("\n");
	printf("		%s\n", cs);
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
		scanf_s("%s", c, sizeof(c));
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

	system("cls");
	printf("\n");
	printf("\n");

	printf("		1 - List Error Messages\n");
	printf("\n");
	printf("		2 - Output Final Results\n");
	printf("\n");
	printf("		3 - Exit\n");
	printf("\n");
	printf("Enter: ");
	scanf_s("%s", cs0, sizeof(cs0));
	i = atoi(cs0);
	while (!(i >= 1 && i <= 3)) {
		printf("Enter:");
		scanf_s("%s", cs0, sizeof(cs0));
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

	system("cls");
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
	scanf_s("%d", &n);

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
	int choice;

	system("cls");
	printf("\n");
	printf("          RIGHT TURN REDUCTION MENU\n");
	printf("\n");
	printf("\n");
	printf("    1. Change User Defined Rt Turn Volume Reduction ");
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
	scanf_s("%i", &choice);
	while (!(choice >= 1 && choice <= 3)) {
		printf("Enter: ");
		scanf_s("%i", &choice);
	}

	if (choice == 3)
		return;

	switch (choice) {
	case 1:
		printf("Enter reduction percentage (0-1): \n");
		double rate;
		scanf_s("%lf", &rate);
		rt_redt = rate;
		while (!(rt_redt >= 0 && rt_redt <= 1)) {
			printf("Enter a decimal number: \n");
			scanf_s("%lf", &rate);
			rt_redt = rate;
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
	struct LEGSVOL a[TOTINT], *pt;

	system("cls");
	printf("\n");
	printf("\n");
	printf("				%s\n", cs);
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

	scanf_s("%s", line, sizeof(line));
	n = atoi(line);

	while (!(n >= 1 && n <= 6)) {
		printf(" Enter:\n");
		scanf_s("%s", line, sizeof(line));
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

// compare two strings case insensitive.
// return true if they are identical otherwise return false
bool stringcompare(char* str1, char* str2) {
	int i;
	int len = strlen(str1);

	// converting str1 to capital
	char* cap = new char[len + 1];

	for (i = 0; i < len; i++)
		cap[i] = toupper(str1[i]);

	if (strcmp(cap, str2) == 0) {
		delete cap;
		return true;
	}
	else {
		delete cap;
		return false;
	}
}

void ls_rst(struct LEGSVOL* pt, char* cs, char* address, char* geometry, char* title, int pos) {
	int n, n1, n2, m, *pn1, *pn2, *pm;
	char line[40], fname[40];

	system("cls");
	printf("\n");
	printf("\n");
	printf("		%s\n",cs);
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
	if (stringcompare(cs, "XXXX -> FLXX -> FLFN"))
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
	if (stringcompare(cs, "XXXX -> FLXX -> FLFN"))
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
	scanf_s("%s", line, sizeof(line));
	n = atoi(line);
	while (!(n >= 1 && n <= 5)) {
		printf("Enter:\n");
		scanf_s("%s", line, sizeof(line));
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
		get_range(pn1, pn2, fname, sizeof(fname), pm);
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
	get_range(pn1, pn2, fname, sizeof(fname), pm);

	if (m == 2){
		lnum = row_p;
		fopen_s(&fp, fname, "w");
	}
	else {
		fp = stdout;
		lnum = row_s;
	}

	k = 30;
	fprintf(fp, "%s\n", cs);
	fprintf(fp, "Int | Counted/ |  Center  | Scen. A/D | Leg/Node | Turn Vol |\n");
	fprintf(fp, " #  | Modeled  |Node Check|  Balance  | Vol Check| Balancing|\n");
	fprintf(fp, "-------------------------------------------------------------\n");

	for (i = n1; i < n2; i++) {
		if (!(badv[i].flag[0] == 0 && badv[i].flag[1] == 0 && badv[i].flag[2] == 0 &&
			badv[i].flag[3] == 0 && badv[i].flag[4] == 0 && badv[i].flag[5] == 0)){
			k++;
			if (m == 1 && k > row_s) {
				k = getchar();
				k = 1;
			}
			fprintf(fp, "%3d  |", badv[i].id);
			for (j = 1; j <= 5; j++)
			if (badv[i].flag[j] != 0)
				fprintf(fp, "%6d    |", badv[i].flag[j]);
			else
				fprintf(fp, "%10s|", " ");

			if (badv[i].flag[0] != 0)
				fprintf(fp, "%6d    ", badv[i].flag[0]);
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
	get_range(pn1, pn2, fname, sizeof(fname), pm);

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
		//linenum++;
		//if (linenum > lnum) {
		//	if (m == 1)
		//		linenum = getchar();
		//	if (m == 2)
		//	for (j = 0; j < 4; j++)
		//		fprintf(fp, "\n");
		//	linenum = 1;
		//}

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

/* ask output device and int# range to be printed 
*n1: array position for the start intersection
*n2: array position for the end intersection
*fname: output file name
length: string length for the output file name
*m:  output device. 2: external file 1: screen only
*/
void get_range(int* n1, int* n2, char* fname, int length, int* m) {
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
	if (to > badv[maxid - 1].id)	/* if TO int# is greater than the    */
		to = badv[maxid - 1].id;	/* biggest int#, take the biggest one */

	// get array positions for the selected intersection ID range (from .. to)
	// *n1 is the position for from, *n2 is the position for to
	*n1 = is_ints_id(from, badv, maxid);
	*n2 = is_ints_id(to, badv, maxid);

	while (*n1 < 0 && from < to) {
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

	scanf_s("%s", line, sizeof(line));
	*m = atoi(line);
	while (!(*m == 1 || *m == 2)) {
		printf("Enter :");
		scanf_s("%s", line, sizeof(line));
		*m = atoi(line);
	}

	if (*m == 2){
		printf("Enter file name:\n");
		scanf_s("%s", fname, length);
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
	//for (i = 0; i<65; i++)
	//	fprintf(fp, " ");
	fprintf(fp, "\n\n");

	for (j = 1; j<78; j++)
		fprintf(fp, "-");
	fprintf(fp, "\n");

	fprintf(fp, "    Int#       ND    SA    NA    SD    ED    WA    EA    ED    Dep    Arr\n");
	for (j = 1; j<78; j++)
		fprintf(fp, "-");
	fprintf(fp, "\n");
}

void compute_los(char* cs, char* address, char* geometry, char* title, int scen) {
	int i, n, n1, n2, m, *pn1, *pn2, *pm;
	char fname[40];
	FILE *fp;

	system("cls");
	printf("\n");
	printf("\n");
	printf("			%s\n", cs);
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
		get_range(pn1, pn2, fname, sizeof(fname), pm);
		prn_los(cs, n1, n2, title, fname, m);
		break;
	case 2:
		opt_los(address, geometry, scen);
		pn1 = &n1;
		pn2 = &n2;
		pm = &m;
		get_range(pn1, pn2, fname, sizeof(fname), pm);
		prn_los_opt(cs, n1, n2, title, fname, m);
		break;
	case 3:
		cal_los(address, geometry, scen);
		printf("Enter output filename:");
		scanf_s("%s", fname, sizeof(fname));
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
		for (j = 0; j < 15; j++)
			cs1[j] = badv[i].addr[j];
		cs1[15] = '\0';
		for (j = 28; j < 43; j++)
			cs2[j - 28] = badv[i].addr[j];
		cs2[15] = '\0';

		if (badv[i].id == 77)
			int a = 0;
		fprintf(fp, " %3d | %15s     %15s |", badv[i].id, cs1, cs2);
		if (badv[i].vc != 0) {
			fprintf(fp, "  %s   %4.2f   %2.0f |", badv[i].los, badv[i].vc, badv[i].delay[0]);
			if (badv[i].opt_vc != 0)
				fprintf(fp, " %s   %4.2f   %2.0f ", badv[i].opt_los, badv[i].opt_vc, badv[i].delay[1]);
			else
				fprintf(fp, "               ");
			if (badv[i].flag[0] != 0)
				fprintf(fp, "|  %d\n", badv[i].flag[0]);
			else
				fprintf(fp, "|\n");
		}
		else
			fprintf(fp, "                |               |\n");

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

	fprintf(fp, "-----------------------------------------------------------------------------------\n");
	fprintf(fp, " INT |          INTERSECTION               |    EXISTING    |    OPTIMAL    |  ERR\n");
	fprintf(fp, "  #  | N-S Street        E-W Street        | LOS   V/C   PH | LOS  V/C   PH |\n");
	fprintf(fp, "-----|-------------------------------------|----------------|----------------------\n");
}

/* print intersectino LOS report
cs: report title
from: array position for the start intersection ID
to:   array position for the end intersection ID
title: scenario name
fname: file name for the report
m: reprot device. 2: output to file 1: screen only   
*/
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
	pagenum = 1;
	for (j = 0; j < 2; j++)
		fprintf(fp, "\n");
	title_los(fp, cs, pagenum, title);

	for (i = from; i <= to; i++) {
		// intersection ID, address
		fprintf(fp, "%3d |%s|", badv[i].id, badv[i].addr);
		if (badv[i].vc != 0) {
			// los, vc, delay
			fprintf(fp, " %s   %3.2f   %3.0f  ", badv[i].los, badv[i].vc, badv[i].delay[0]);
			if (badv[i].flag[0] != 0)
				// error flag (LOS related)
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

/*
print header for the LOS report.
fp: output file pointer
cs: report title
page: not used any more
title: scenario name
*/
void title_los(FILE* fp, char* cs, int page, char* title) {
	int i, j, text_len;

	fprintf(fp, "%s\n", cs);
	text_len = strlen(title);
	j = 35 - text_len / 2;

	for (i = 0; i <= j; i++)
		fprintf(fp, " ");
	fprintf(fp, "%s\n", title);

	for (j = 1; j < 82; j++)
		fprintf(fp, "-");
	fprintf(fp, "\n");

	fprintf(fp, "Int#| North-South Street	East-West Street");
	fprintf(fp, "	     | LOS V/C   AVD  Err\n");
	for (j = 1; j < 82; j++)
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
	get_range(pn1, pn2, fname, sizeof(fname), pm);

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
			if (m == 1)
				linenum = getchar();
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
				fprintf(fp, " %c %4.2f     ", int_to_char(ssv[i].vol[j]), ssv[i].vol[j + 1] / 100.);
			else
				fprintf(fp, "            ");
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


	fprintf(fp, "--------------------------------------------------------------------------------\n");
	fprintf(fp, " INT|            INTERSECTION         | BASE YR     TestImp   NO ACTN      FINAL\n"); // one column could be missing need to investigate
	fprintf(fp, "  # | N-S Street        E-W Street    | ACTUAL       XXFN      FLXX        FLFN \n");
	fprintf(fp, "----|---------------------------------|-----------------------------------------\n");

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
	title_turns(fp, cs, pagenum, title);

	for (i = from; i <= to; i++) {
		linenum++;
		if (linenum > lnum) {
			if (m == 1)
				linenum = getchar();
			//if (m == 2)
			//for (j = 0; j < 6; j++)
			//	fprintf(fp, "\n");
			//linenum = 1;
			//pagenum++;
			//title_turns(fp, cs, pagenum, title);
		}

		fprintf(fp, "%3d  ", badv[i].id);
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

	//fprintf(fp, "%s\n", cs);
	//text_len = strlen(title);
	//j = 35 - text_len / 2;

	//for (i = 0; i <= j; i++)
	//	fprintf(fp, " ");

	//fprintf(fp, "%s\n", title);

	//fprintf(fp, "\n");

	//for (j = 1; j < 70; j++)
	//	fprintf(fp, "-");
	//fprintf(fp, "\n");

	fprintf(fp, "Int |	NORTHBOUND	SOUTHBOUND	");
	fprintf(fp, "EASTBOUND	WESTBOUND \n");
	fprintf(fp, " #  |  LT   TH    RT   LT   TH   RT    ");
	fprintf(fp, "LT   TH   RT    LT   TH   RT\n");

	for (j = 1; j < 70; j++)
		fprintf(fp, "-");
	fprintf(fp, "\n");
}


/* calculate approach volumes
*/
void leg_sum(struct TMVOL t[], struct LEGSVOL* l) {
	int n;
	for (n = 0; n < maxid; n++) {
		(*(l + n)).id = t[n].id;
		(*(l + n)).vol[0] = t[n].vol[1] + t[n].vol[11] + t[n].vol[6]; // Norht leg Departure
		(*(l + n)).vol[4] = t[n].vol[3] + t[n].vol[4] + t[n].vol[5];  // North leg arrival
		(*(l + n)).vol[1] = t[n].vol[4] + t[n].vol[8] + t[n].vol[9];  // South leg Departure
		(*(l + n)).vol[5] = t[n].vol[0] + t[n].vol[1] + t[n].vol[2];  // South leg Arrival
		(*(l + n)).vol[2] = t[n].vol[2] + t[n].vol[3] + t[n].vol[7];  // East leg Departure
		(*(l + n)).vol[6] = t[n].vol[9] + t[n].vol[10] + t[n].vol[11]; // East leg Arrival
		(*(l + n)).vol[3] = t[n].vol[0] + t[n].vol[5] + t[n].vol[10]; // West leg Departure
		(*(l + n)).vol[7] = t[n].vol[6] + t[n].vol[7] + t[n].vol[8];  // West leg Arrival
	}
}

void prn_output(char* address, char* geometry, char* title) {
	struct CHAR_S {
		char a[7];
	} cs[13];
	FILE *faddr, *fgeo, *fp;
	int i, j, k, linenum, pagenum, lnum;
	char line[100], addr[60], device[11], phase_ns[3], phase_ew[3],
		cs1[20], fname[40];

	lnum = row_p;
	printf("Enter file name:");
	scanf_s("%s", fname, sizeof(fname));
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
		sscanf_s(line, "%5c%56c%3c%10c", cs[0].a, _countof(cs[0].a),  addr, _countof(addr), 
			cs1, _countof(cs1), device, _countof(device));
		cs1[3] = '\0';
		device[10] = '\0';
		t[0][0] = atof(cs[0].a);
		linenum++;

		//if (linenum > lnum) {
		//	for (i = 0; i < 4; i++)
		//		fprintf(fp, "\n");
		//	linenum = 1;
		//	pagenum++;
		//	title_output(fp, title, pagenum);
		//}

		fgets(line, 100, fgeo);
		// read channelization
		sscanf_s(line, "%5c%3c%3c%5c%7c%5c%5c%7c%5c%5c%7c%5c%5c%7c%5c", cs[0].a, _countof(cs[0].a), phase_ns, _countof(phase_ns),
			phase_ew, _countof(phase_ew), cs[1].a, _countof(cs[1].a), cs[2].a, _countof(cs[2].a), cs[3].a, _countof(cs[3].a),
			cs[4].a, _countof(cs[4].a), cs[5].a, _countof(cs[5].a), cs[6].a, _countof(cs[6].a), cs[7].a, _countof(cs[7].a),
			cs[8].a, _countof(cs[8].a), cs[9].a, _countof(cs[9].a), cs[10].a, _countof(cs[10].a), cs[11].a, _countof(cs[11].a),
			cs[12].a, _countof(cs[12].a));
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
		fprintf(fp, "%3.0f |%s| %s %s |%2d %2d |", t[0][0], badv[k].addr, cs1, device, phase[1], phase[2]);

		// output channelization
		for (i = 1; i <= 4; i++)
			for (j = 1; j <= 3; j++)
			if (lan[i][j] > 9999)
				fprintf(fp, "****");
			else
				fprintf(fp, "%4.0f", lan[i][j]);
		fprintf(fp, "|");

		// output volumes
		for (i = 0; i <= 11; i++)
			fprintf(fp, "%5d", badv[k].vol[i]);
		fprintf(fp, "|");

		// output LOS, vc, delay and error related to LOS.
		fprintf(fp, " %4s| %5.2f| %5.0f|", badv[k].los, badv[k].vc, badv[k].delay[0]);
		if (badv[k].flag[0] != 0)
			fprintf(fp, " %5d|\n", badv[k].flag[0]);
		else
			fprintf(fp, "      |\n");
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
	fprintf(fp, "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	fprintf(fp, "INT |               INTERSECTION                             |                |      |            Channelization                      |                  Turning Movement Volume                   |--------------------------|\n");
	fprintf(fp, " #  | North-South Street          East-West Street           | LOC   Control  | Phase| NBL NBT NBR SBL SBT SBR EBL EBT EBR WBL WBT WBR|  NBL  NBT  NBR  SBL  SBT  SBR  EBL  EBT  EBR  WBL  WBT  WBR| LOS |  vc  | Delay|  Err |\n");
	fprintf(fp, "----|--------------------------------------------------------| ---------------|------|------------------------------------------------|------------------------------------------------------------|-----|------|------|------|\n");
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
