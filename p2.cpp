/* Turning movement analysis Post processor
03/06/92
Jun Ma
*/

#include "stdafx.h"
#include "a.h"
#include "newint.h"


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
void a_d_balance(struct LEGSVOL* lg, int n) {
	int i, tl, t2;
	for (i = 0; i < maxid; i++) {
		if (badv[i].emid < 9999) {
			tl = total(lg + i, 0, 3);
			t2 = total(lg + i, 4, 7);
			if ((abs(tl - t2)) > 4)
				badv[i].flag[3] = (badv[i].flag[3] == 0) ?	300 + n * 10 : badv[i].flag[3];
		}
	}
}

//If int. number is unique, return 1; otherwise return 0
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

// quicksort emme/2 int. number, in ascending order
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

	for (i = 0; i < maxid; i++) {
		(*(s + i)).id = badv[i].id; 
		for (j = 0; j <= 7; j++) 
			(*(s + i)).vol[j] = 0;
	}

	fgets(linerd, 40, fp);
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
			badv[num].flag[5] = 500 + scen * 10 + (int) (acpt_error * 100); 
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
			*(mbpt + i*row + j) = (int)( ratio * (*(mbpt + i*row + j)));
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
			*(mbpt + j*row + i) =  ratio * (*(mbpt + j*row + i));
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

// if number x is a emme/2 int. number, return the position of the int.
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
	for (i = 0; i < (int)strlen(s); i++) 
		s[i] = toupper(s[i]);
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
	char linerd[100], x[10];
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
	fgets(linerd, max, fp);
	fgets(linerd, max, fp);
	fgets(linerd, max, fp);

	while ((fgets(linerd, max, fp)) != NULL) {
		sscanf_s(linerd, "%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c%5c",
			x[0].a, x[1].a, x[2].a, x[3].a, x[4].a, x[5].a, x[6].a, x[7].a,
			x[8].a, x[9].a, x[10].a, x[11].a, x[12].a);
		j = is_ints_id(atoi(x[0].a), badv, maxid);
		if (j == -1) {
			n = 0;
			printf("*** Node %d doesn't exist in the base year file.\n", atoi(x[0].a));
		}
		for (i = 0; i <= 11; i++)
			badv[j].vol[i] = atoi(x[i + 1].a);
		for (i = 0; i < 6; i++)
			badv[j].flag[i] = 0;
	}

	fclose(fp);
	if (n == 0)
		system("sleep 5");
}