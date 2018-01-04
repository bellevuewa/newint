/* Turning movement /* Header file a.h
03/06/92
Jun Ma */

#ifndef _A_H
#define _A_H
#include <stdio.h> 
#include <ctype.h> 
#include <string.h> 
#include <stdlib.h> 
#include <math.h>

#define TOTINT 1000		// maximum number of intersections 
#define max 100 
#define max_error 0.001 // maximum predetermined error
#define row 5			//a four-leg intersection
#define row_s 17		// maximum number of lines per screen 
#define row_p 50		// maximum number of lines per page

/*
link volumes of an intersection
.id - intersection number, in int. #
.vol[0] - link volumes of North leg Northbound
.vol[1] - link volumes of South leg Southbound
.vol[2] - link volumes of East leg Eastbound
.vol[3] - link volumes of West leg Westbound
.vol[4] - link volumes of North leg Southbound
.vol[5] - link volumes of South leg Northbound
.vol[6] - link volumes of East leg Westbound
.vol[7] - link volumes of West leg Eastbound
*/

struct LEGSVOL { 
	int id; 
	int vol[8]; 
};

/*
information of an intersection:
.id - intersection number
.addr - actual location of the intersection
.emid - intersection number, in emme/2 
.ints_id[0] - North leg adjacent node  
.ints_id[1] - South leg adjacent node
.ints_id[2] - East leg adjacent node
.ints_id[3] - West leg adjacent node 
.vol[0] - Northbound left turning volumes
.vol[1] -	through volumes
.vol[2] -	right turning volumes
.vol[3] - Southbound left turning volumes
.vol[4] -	through volumes
.vol[5] -	right turning volumes
.vol[6] - Eastbound left turning volumes
.vol[7] -	through volumes
.vol[8] -	right turning volumes
.vol[9] - Westbound left turning volumes
.vol[10] -	through volumes
.vol[11]	right turning volumes
.flag[0] - error code: level of service related error 
.flag[1] - int. counted or modeled related 
				101 : not counted and not modeled
				102 : not counted but modeled
				103 : counted but not modeled
.flag[2] - center node error
.flag[3] - arrival/departure imbalance
.flag[4] - directional node 
.flag[5] - balancing error 
.los[2] - phasing of existing condition 
.vc - V/C ratio of existing phasing
.opt_los[2] - LOS of optimal phasing
.opt_vc - V/C ratio of optimal phasing
.delay - average vehicle delay

*/

struct TMVOL {
	int id;
	char addr[60];
	int emid;
	int ints_id[4];
	int vol[12];
	int flag[6];
	char los[2];
	double vc;
	char opt_los[2];
	double opt_vc;
	double delay[2];
};


/*
/* base	link volumes of base year actual counts
/* sv0	link volumes of base year land use on base year network
/* svl	link volumes of base year land use on future network
/* sv2	link volumes of future land use on future network
/* badv	turning movement volumes
/* maxid	number of intersections processed
/* phase	phasing code (IMPAX)
/* rt _rest a toggle indicating right turn reduction
*/

struct LEGSVOL base[TOTINT], sv0[TOTINT], sv1[TOTINT], sv2[TOTINT],
	sv[TOTINT];
struct TMVOL badv[TOTINT];
int maxid, phase[3];

// t[][]: turning movement volumes. NB SB EB WB. 2nd dimension LT TH RT
// lane[][]: channelization for each movement NB SB EB WB 2nd dimension LT TH RT
double t[5][5], lan[5][5], pct[5], rt_redt;
int gTotIntAval;
#endif