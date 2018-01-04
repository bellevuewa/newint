/*
	Turning movement analysis	-- p3.c
	Level of Service and average vehicle delay 33/06/92
		Jun Ma
*/


#include "stdafx.h"
#include "a.h"
#include "newint.h"

void opt_los(char* address, char* geometry, int scen) {
	struct CHAR_S {
		char a[7];
	} cs[13];
	FILE *faddr, *fgeo;
	int i, j, k, m, n, error, nphase1, nphase2, nsp[10], ewp[10], opt1, opt2, *pt;
	char line[100], addr[60], device[10], phase_ns[3], phase_ew[3],
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
		sscanf_s(line, "%5c%56c%3c%10c", cs[0].a, addr, cs1, device);
		cs1[3] = '\0';
		device[10] = '\0';
		t[0][0] = atof(cs[0].a);

		fgets(line, 100, fgeo);
		sscanf_s(line, "%5c%3c%3c%5c%7c%5c%5c%7c%5c%5c%7c%5c%5c%7c%5c",
			cs[0].a, phase_ns, phase_ew, cs[1].a, cs[2].a, cs[3].a, cs[4].a,
			cs[5].a, cs[6].a, cs[7].a, cs[8].a, cs[9].a, cs[10].a, cs[11].a,
			cs[12].a);

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
		sscanf_s(line, "%5c%56c%3c%10c", cs[0].a, addr, cs1, device);
		t[0][0] = atof(cs[0].a);
		fgets(line, 100, fgeo);
		sscanf_s(line, "%5e43c%3c%5c%7c%5c%5c%7c%5c%5c%7e45c%5c%7c%5c", cs[0].a, phase_ns, phase_ew, cs[1].a, cs[2].a, cs[3].a, cs[4].a, cs[5].a, cs[6].a, cs[7].a, cs[8].a, cs[9].a, cs[10].a, cs[11].a, cs[12].a);
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
			ssv[k].vol[pst2] = (int) floor(vc * 100 + 0.5);
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

void cal_los(char* address, char* geometry, int scen) {
	struct CHAR_S {
		char a[7];
	} cs[13];
	FILE *faddr, *fgeo;
	int i, j, k, error, nphase1, nphase2;
	char line[100], addr[60], device[10], phase_ns[3], phase_ew[3],
		cs1[20], los[5];
	double cv_12, cv_34, geo[5], vc, air_q;
	errno_t err = 0;

	err = fopen_s(&faddr, address, "r"); 
	for (i = 0; i <= 2; i++)
		fgets(line, 100, faddr); 
	err = fopen_s(&fgeo, geometry, "r"); 
	for (i = 0; i <= 2; i++)
		fgets(line, 100, fgeo);

	while ((fgets(line, 100, faddr)) != NULL) {
		sscanf_s(line, "%5c%56c°63c%10c", cs[0].a, addr, cs1, device); 
		cs1[3] = '\0';
		device[10] = '\0';
		t[0][0] = atof(cs[0].a);
		fgets(line, 100, fgeo);
		sscanf_s(line, "%5c%3c%3c%5c%7c%5c%5c%7c%5c%5c%7c%5c%5c%7c%5c", cs[0].a, phase_ns, phase_ew,
			cs[1].a, cs[2].a, cs[3].a, cs[4].a, cs[5].a, cs[6].a, cs[7].a, cs[8].a, cs[9].a, cs[10].a, cs[11].a, cs[12].a);
		if (t[0][0] != atof(cs[0].a)) {
			printf("Check node number in GEOMETRY. Addr#=%3.0f Geo#=%3.0f\n", t[0][0],atof(cs[0].a));
			exit(0);
		}

		for (i = 1; i <= 4; i++)
			for (j = 1; j <= 3; j++)
				lan[i][j] = atof(cs[(i - 1) * 3 + j].a);
		k = is_ints_id((int)t[0][0], badv, maxid);

		for (i = 1; i <= 4; i++)
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

		for (i = 1; i <= 4; i++) {
			if (fmod(lan[i][3], 10.) == 9) { // last digit is 9?
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
			convert_lan(geo);
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

			for (i = 1; i <= 4; i++)
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
			if (badv[i].flag[0] == 0)
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

int check_vol_lan() {
	int i, j, n;
	
	n = 0;
	for (i = 1; i <= 4; i++) {
		for (j = 1; j <= 3; j++)
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

	if (p1 == 7 || p1 == 8 || p1 == 9)
		if (!((t[1][3] == 0 && t[2][1] == 0) || (t[1][1] == 0 && t[2][3] == 0)))
			i = 1;
	if (p1 == 4 || p1 == 5 || p1 == 6 || p1 == 9)
		if (!(lan[1][1] >= 0 && lan[2][1] >= 0))
			i = 1;

	if (p2 == 7 || p2 == 8 || p2 == 9)
		if (!((t[3][1] == 0 && t[4][3] == 0)|| (t[3][3] == 0 && t[4][1] == 0)))
			j = 1;
	
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
	one land shared woth others. positive number means it is separated.
*/
void get_lane() {
	int i, j, k, l1[4], l2[4], lf_lan;

	for (k = 1; k <= 4; k++) {
		for (j = 1; j <= 3; j++)
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

	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 3; j++)
			total = total + t[i][j];

	opv[1] = t[2][2] + t[2][3];
	opv[2] = t[1][2] + t[1][3];
	opv[3] = t[4][2] + t[4][3];
	opv[4] = t[3][2] + t[3][3];

	ratio[1] = (opv[1] + opv[2] + t[1][1] + t[2][1]) / total; 
	ratio[3] = (opv[3] + opv[4] + t[3][1] + t[4][1]) / total;
	ratio[2] = ratio[1];
	ratio[4] = ratio[3];

	for (i = 1; i <= 4; i++) {
		e[i] = floor(ratio[i] * 1200 - opv[i] + 0.5);
		if (e[i]<0)
			e[i] = 0;
	}
	if (t[i][1] > (e[i] + 90))
		return 0;
	else
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
			lan_separate(lt, 1, 0, 0, 0);
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
			lan_separate(lt, 1, 0, 1, 1);
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
			lan_share(lt, 1, 3, 2, 1); 
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

	for (i = 1; i <= 4; i++) {
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