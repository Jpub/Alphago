/* Copyright (C) 2016 Otsuki Tomoshi <tomoshi@m00.itscom.net> */

#ifndef _GO_H_
#define _GO_H_

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define MIN(a,b) ((a)<(b) ? (a) : (b) )
#define MAX(a,b) ((a)>(b) ? (a) : (b) )
#define ABS(a)   ((a)>0   ? (a) : -(a))
#define SGN(a)   ((a)==0  ? (0) : ( (a)>0 ? (1) : (-1) ))
#define IN_RANGE(a, x, b) ((a) <= (x) && (x) <= (b))

#define BOARD_EDGE_SIZE 19
#define TIME_LIM 600

#define BB_EDGE     (BOARD_EDGE_SIZE + 2)  
#define XY_SIZE     (BB_EDGE * BB_EDGE)    
#define BB_EFF       64
#define NUM_PER_BB   64
#define BB_IDX_SIZE ((XY_SIZE - 1)/NUM_PER_BB + 1)    

#define VEC_WIDTH 8

typedef unsigned long long bb_t;

typedef enum {SP = 0, BK, WH, OB} color_t;
typedef enum {TBN_BK = 0, TBN_WH} teban_t;
typedef enum {OFF = 0, ON} onoff_t;

#define CROSS_MAX  4
#define D_MAX 8

#define TBN_MAX 2
#define COLOR_MAX 4
#define GET_AITE(st)     (BK + WH - (st))
#define AITE_TBN(tbn)    (TBN_BK + TBN_WH - (tbn))
#define TBN2COLOR(tbn)   ((tbn) + 1)
#define COLOR2TBN(color) ((color) - 1)

#define TESUU_MAX   (XY_SIZE * 2)
#define REN_MAX     (XY_SIZE * 2) /* 連の最大数 */
#define REN_BB_SIZE (REN_MAX/BB_EFF + 1)

#define FRAME 512

#define IS_FIRST_MOVE(ban) (ban->tesuu == 1 && ban->occupiedPoints[TBN_BK] == 0 && ban->occupiedPoints[TBN_WH] == 0)
#define X_Y_TO_POS(x, y) ((y) * BB_EDGE + (x))

#define POS_TO_X(pos)   (pos % BB_EDGE)
#define POS_TO_Y(pos)   (pos / BB_EDGE)

#define POS_TO_IDX(pos) ((pos)/NUM_PER_BB)
#define POS_TO_BIT(pos) ((pos)%NUM_PER_BB)

#define GET_POS(idx, digit) ((idx) * NUM_PER_BB + (digit))

#define BELONGS_TO(bb, pos) (bb[POS_TO_IDX(pos)] & (1LL << (POS_TO_BIT(pos))))
#define ADD_AN_ELEMENT_TO_BB(bb, pos) (bb[POS_TO_IDX(pos)] |= (1LL << (POS_TO_BIT(pos))))
#define ERASE_AN_ELEMENT_FROM_BB(bb, pos) (bb[POS_TO_IDX(pos)] &= ~(1LL << (POS_TO_BIT(pos))))

#define MERGE_BB(bb, bb2)     if (1){int k; for(k=0;k<BB_IDX_SIZE;k++)bb[k] |= bb2[k];}
#define CLEAR_BB(bb)          if (1){int k; for(k=0;k<BB_IDX_SIZE;k++)bb[k] = 0LL;}
#define SPLIT_OUT_BB(bb, bb2) if (1){int k; for(k=0;k<BB_IDX_SIZE;k++)bb[k] &= ~(bb2[k]);}

extern char *COLOR_NAME;
extern int D2DELTA[D_MAX];

typedef struct {
	int color;
	int occupiedPoints;
	int breathPoints;
	int deadFlag;
	bb_t occupiedBB[BB_IDX_SIZE];
	bb_t breathBB[BB_IDX_SIZE];
} renInfo_t;

typedef struct {
	int pos;
	int mergeRenID[CROSS_MAX];
	int removeRenID[CROSS_MAX];
	int ko;

	renInfo_t renInfo;
	int mergeNum, removeNum;
	double time;
} history_t;

#define FILTER_SIZE 192 //100
#define CHANNEL_SIZE 48

typedef struct {
	int tbn;                         /* 手番 */
	int tesuu;                       /* 手数 */
	int renNum;                      /* 連の最大ID */
	bb_t occupiedBB[TBN_MAX][BB_IDX_SIZE];    /* 各手番の盤上の石 bb */
	int occupiedPoints[TBN_MAX];

	int color[XY_SIZE];              /* xyの石の種類(BLACK, WHITE, SPACE, OB) */
	int renID[XY_SIZE];              /* xyの連のid(default: 0) */
	int prisoner[TBN_MAX];           /* agehama */

	renInfo_t renInfo[REN_MAX];
	history_t histInfo[TESUU_MAX];

	double komi;
	int feature[CHANNEL_SIZE][21][21];
} goban_t;


typedef float cnn_t;

typedef struct {
	goban_t *ban;
	cnn_t hidden[14][21][21][FILTER_SIZE];
	cnn_t hiddenIn[23][23][CHANNEL_SIZE];
	cnn_t hiddenOut[21][21];
} game_hdl_t;

typedef struct {
	cnn_t weight_cnn_5x5[CHANNEL_SIZE][FILTER_SIZE][25];
	cnn_t weight_cnn_3x3[13][FILTER_SIZE][FILTER_SIZE][9];
	cnn_t weight_cnn_1x1[FILTER_SIZE];
	cnn_t weight_cnn_bias[361];
} parameter_t;

extern parameter_t *cnn_param;

/* from init.c */
game_hdl_t *open_game_hdl(int argc, char **argv);
void close_game_hdl(game_hdl_t *hdl);
void set_initial_pos(goban_t *ban);


/* from move.c */
void make_move(const int xy, goban_t *ban);
void unmake_move(goban_t *ban);

/* from cnn_input.c */
int get_cnn_prob(goban_t *ban, game_hdl_t *hdl, double pos_to_prob[XY_SIZE]);
int get_feature(FILE *fp, const int i, const int tesuu, goban_t *ban);
int load_cnn_weight(game_hdl_t *hdl);

/* from util.c */
int detect_suicide_or_capture(goban_t *ban, const int pos);
double Tool_GetTimeNow();
int get_single_pos(bb_t *targetBB);
int get_another_pos(bb_t *targetBB, const int exclude);
int get_double_pos(int *cand, bb_t *targetBB);
int check_dead(const int pos, goban_t *ban);
int check_kill(const int seme, const int escape,  goban_t *ban);
int judge_eff_te(const int pos, goban_t *ban);
int outPos(const int pos);
int judgeKoNG(const int pos, goban_t *ban);
int teOK(const int xy, goban_t *ban);
void writeBan(goban_t *ban);
int get_nb_stones(const int xy, int nb_num[CROSS_MAX], int nb_stones[CROSS_MAX][COLOR_MAX], goban_t *ban);
int bb_pop_cnt(bb_t *targetBB);

/* from bit_util.c */
int Get_FirstBit64(long long bit);
int pop_cnt(unsigned long long bits);

#endif /* ifdef GO_H */
