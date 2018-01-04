/*
Turning movement analysis -- pl.c
It includes the main program and the base year data processing.
03/06/92
Jun Ma

*/
#include "stdafx.h"
#include "a.h" 
#include "newint.h"

int main()
{
	struct LEGSVOL *lsv, *lsv0, *lg, *lg0, *lg1, *lg2;
	FILE *fp;
	char linerd[max], cs[10], filename[40], addr1[40], addr2[40], node1[40], node2[40], turn1[40], geo1[40], geo2[40], title[40], modxxxx[40], modxxfn[40], modflxx[40], modflfn[40], handadj1[40], handadj2[40], handadj3[40];
	int n, i;
	errno_t err = 0;
	
	menu_0();

	rt_redt = 0.3;
	printf("Enter file name: ");
	scanf_s("%s", filename);
	while ((err = fopen_s(&fp, filename, "r")) > 0) {
		printf("%s doesn't exist\n", upper(filename));
		printf("Enter:");
		scanf_s("%s", filename);
	}

	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", title);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", addr1);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", addr2);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", geo1);
	fgets(linerd, 80, fp); 
	sscanf_s(linerd, "%s", geo2);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", node1);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", node2);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", turn1);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", handadj1);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", handadj2);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", handadj3);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modxxxx);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modxxfn);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modflxx);
	fgets(linerd, 80, fp);
	sscanf_s(linerd, "%s", modflfn);
	fclose(fp);

	n = 1;
	while (n > 0) {
		system("clear");
		printf("                      MAIN MENU\n");
		printf("\n");
		printf("\n");
		printf("		1.  xxxx\n");
		printf("			 - Base Year TMC \n");
		printf("		2. xxxx -> xxFN\n");
		printf("			 - Base Year TMC Modified to Reflect Futurn Network\n");
		printf("		3. xxxx - FLxx\n");
		printf("			- Base Year TMC Modified to Reflect Future Land Use\n");
		printf("		4. xxx -> xxFN -> FLFN\n");
		printf("			- Base Year TMC Modified to Reflect Future Network and Land Use\n");
		printf("		5. xxx -> FLxx -> FLFN\n");
		printf("			- Base Year TMC Modified to Reflect Future Land Use and Network\n");
		printf("		6. Output LOS Summary for All Scenarios\n");
		printf("\n");
		printf("		7. Right Turn Reduction\n");
		printf("\n");
		printf("		9. Exit\n");

		scanf_s("%c", cs);
		n = atoi(cs);
		while (!((n <= 7 && n > 0) || n == 9)) {
			printf("Enter:");
			scanf_s("%s", cs);
			n = atoi(cs);
		}
		if (n == 9) exit(0);
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
				system("sleep 5");
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
				system("sleep 5");
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
				system("sleep 5");
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
				system("sleep 5");
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
				get_leg_vol(badv,lsv,4);
				cal_los_all(addr1,geo1,sv2,4,5);	/* modflxx */
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
   and the first hand adjustment */

void read_bdata(char* addr, char* node, char* turn, char* adj, struct LEGSVOL* lg) {
	int i, j, n, *n1, *n2, error;
	FILE *faddr, *fnode, *fvol;
	char linerd[100];
	errno_t err = 0;

	struct x_type {
		char a[7];
	} x[20];
	error = 1;
	if (addr[0] == '0' || node[0] == '0' || turn[0] == '0' || adj[0] == '0') {
		printf("Check base year data file names\n");
		exit(0);
	}

	if ((err = fopen_s(&faddr, addr, "r")) > 0) {	/* address */
		printf("%s doesn't exist. ", upper(addr));
		exit(0);
	}

	for (i = 0; i <= 2; i++)
		fgets(linerd, 100, faddr);

	if ((err = fopen_s(&fnode, node, "r")) > 0) {	/* nodes */
		printf("%s doesn't exist. ", upper(node));
		exit(0);
	}

	for (i = 0; i <= 2; i++)
		fgets(linerd, max, fnode);

	if ((err = fopen_s(&fvol, turn, "r")) > 0) {	/* turns */
		printf("%s doesn't exist. ", upper(turn));
		exit(0);
	}

	for (i = 0; i <= 2; i++)
		fgets(linerd, max, fvol);
	i = 0;

	while ((fgets(linerd, max, faddr)) != NULL) {
		sscanf_s(linerd, "%5c%56c", x[1].a, badv[i].addr);
		badv[i].id = atoi(x[1].a);
		for (j = 1; j <= 6; j++)
			strcpy_s(x[j].a, " ");
		if ((fgets(linerd, max, fnode)) != NULL) {
			sscanf_s(linerd, "%5c%6c%6c%6c%6c%6c", x[1].a, x[2].a, x[3].a, x[4].a,
				x[5].a, x[6].a);
			n = atoi(x[1].a);
			badv[i].emid = atoi(x[2].a);
			for (j = 0; j < 4; j++)
				badv[i].ints_id[j] = atoi(x[j + 3].a);
			if (badv[i].id != n) {
				printf("Check node # in %s. Addr#=%d Node#=%d\n", upper(node), badv[i].id, n);
				exit(0);
			}
			else {
				printf("Check %s and %s\n", upper(addr), upper(node));
				exit(0);
			}

			for (j = 1; j <= 13; j++)
				strcpy_s(x[j].a, " ");
			if ((fgets(linerd, max, fvol)) != NULL) {
				sscanf_s(linerd, "%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c", x[1].a,
					x[2].a, x[3].a, x[4].a, x[5].a, x[6].a, x[7].a, x[8].a, x[9].a, x[10].a,
					x[11].a, x[12].a, x[13].a);
				n = atoi(x[1].a);
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

		maxid = i;
		fclose(faddr);
		fclose(fnode);
		fclose(fvol);

		// check if two int. have the same int. # and sort int. in ascending
		// order according to the int. #
		n1 = &i;
		n2 = &j;
		sort_id(badv, 0, maxid - 1);
		if (is_unique_id(badv, n1, n2) == 0) {
			printf("%d(%d) %d(%d) have the same intersection #\n",
				badv[i].id, badv[i].emid, badv[j].id, badv[j].emid);
			exit(0);
		}

		/* read hand adjustment file */
		add_adj(adj);

		/*
		printf("id\n");
		for(i = 0; i < maxid;i++) {
		printf("%5d (%5d) ",badv[i].id,badv[i].emid);
		for (j = 0;j < 12;j++)
		printf("%5d",badv[i].vol[j]);
		printf("\n");
		}
		*/

		/* check if two int. have the same emme/2 int. # and sort int. in
		   ascending order according to emme/2 int. */
		n1 = &i;
		n2 = &j;
		sort_emid(badv, 0, maxid - 1);

		/*
		printf("emid\n");
		for (i = 0;i < maxid; i++) {
		printf("%5d (%5d) ",badv[i].id,badv[i].emid);
		for(j = 0; j < 12; j++)
		printf("%5d",badv[i].vol[j]);
		printf("\n");
		}
		*/

		// compute link volumes of base year actual counts, write them into BASE leg_sum(badv,1g);
		sort_legvol(lg, 0, maxid - 1);
		if (is_unique_emid(badv, n1, n2) == 0) {
			printf("%d(%d) %d(%d) have the same EMME/2 #\n", badv[i].emid, badv[i].id, badv[j].emid, badv[j].id);
			exit(0);
		}
	}

}

void check_bdata(struct LEGSVOL* lg) {
	int i, s;
	for (i = 0; i < maxid - 1; i++) {
		s = total(lg + 1, 0, 7);
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

void zero_out() {
	int i, j;

	for (i = 0; i < maxline; i++) {
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

		badv[i].id = 0;
		strcpy_s(badv[i].addr, " ");
		badv[i].emid = 0;
		for (j = 0; j < 4; j++)
			badv[i].ints_id[j] = 0;

		for (j = 0; j < 6; j++)
			badv[i].flag[j] = 0;

		strcpy_s(badv[i].los, " ");
		badv[i].vc = 0;
		strcpy_s(badv[i].opt_los, " ");
		badv[i].opt_vc = 0;
		for (j = 0; j < 2; j++)
			badv[i].delay[j] = 0;
	}
}

