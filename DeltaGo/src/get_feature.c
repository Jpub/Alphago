#include "go.h"

static int writeColor(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int index, int keep[21][21])
{
	int x, y;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			keep[x][y] = (ban->color[X_Y_TO_POS(x, y)] == index);
		}
	}
	
	return 0;
}

static int writeSame(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int value, int keep[21][21])
{
	int x, y;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			keep[x][y] = value;
		}
	}
	return 0;
}

static int writeHist(FILE *fp_out, const int id, const int tesuu, goban_t *ban, const int tesuu_hist, int keep[21][21])
{
	int x, y;
	int pos_old = 0;
	if (tesuu >= tesuu_hist){
		pos_old = ban->histInfo[tesuu - tesuu_hist + 1].pos;
	}
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			keep[x][y] = (X_Y_TO_POS(x, y) == pos_old);
		}
	}
	return 0;
}

static int writeLiberty(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int num, int keep[21][21])
{
	int x, y;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			int pos = X_Y_TO_POS(x, y);
			int breath = ban->renInfo[ban->renID[pos]].breathPoints;
			breath = MIN(breath, 8);
			keep[x][y] = (breath == num);
		}
	}
	
	return 0;
}

static int writeLibertyAfterMoveSize(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int num, int keep[21][21])
{
	int x, y, d;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			int pos = X_Y_TO_POS(x, y);
			int breath = 0;

			if (ban->color[pos] == SP){
				bb_t breathBB[BB_IDX_SIZE];
				CLEAR_BB(breathBB); 

				for (d = 1; d < D_MAX; d+= 2){ /* cross */
					const int c = pos + D2DELTA[d];
					if (ban->color[c] == SP){
						ADD_AN_ELEMENT_TO_BB(breathBB, c);
					}
					if (ban->color[c] == TBN2COLOR(ban->tbn)){
		    			int renID = ban->renID[c];
		 				renInfo_t *g = &ban->renInfo[renID];
						MERGE_BB(breathBB, g->breathBB);
					}
				}
				ERASE_AN_ELEMENT_FROM_BB(breathBB, pos);
    			breath = bb_pop_cnt(breathBB);
    		}
			breath = MIN(breath, 8);
			keep[x][y] = (breath == num);
			
		}
	}
	return 0;
}


static int writeCaptureSize(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int num, int keep[21][21])
{
	int x, y, d;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			int pos = X_Y_TO_POS(x, y);
			int atari = 0;
			if (ban->color[pos] == SP){
				bb_t atariBB[BB_IDX_SIZE];
				CLEAR_BB(atariBB);

				for (d = 1; d < D_MAX; d+= 2){ /* cross */
					const int c = pos + D2DELTA[d];
					if (ban->color[c] == OB || ban->color[c] == SP){
						continue;
					}
		 			int renID = ban->renID[c];
					
					renInfo_t *g = &ban->renInfo[renID];
					if (g->breathPoints == 1 && g->color != TBN2COLOR(ban->tbn)){
						MERGE_BB(atariBB, g->occupiedBB);
    				}
    			}
				atari  = bb_pop_cnt(atariBB);
			}
			atari = MIN(atari, 8);
			keep[x][y] = (atari == num);
		}
	}
	return 0;
}


static int writeCapturedSize(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int num, int keep[21][21])
{
	int x, y, d;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			int pos = X_Y_TO_POS(x, y);
			int atari = 0, breath = 0;
			if (ban->color[pos] == SP){
				bb_t breathBB[BB_IDX_SIZE], atariBB[BB_IDX_SIZE];
				CLEAR_BB(breathBB); CLEAR_BB(atariBB);
				for (d = 1; d < D_MAX; d+= 2){ /* cross */
					const int c = pos + D2DELTA[d];
					if (ban->color[c] == SP){
						ADD_AN_ELEMENT_TO_BB(breathBB, c);
					}
					if (ban->color[c] == TBN2COLOR(ban->tbn)){
						int renID = ban->renID[c];
						renInfo_t *g = &ban->renInfo[renID];
						MERGE_BB(atariBB, g->occupiedBB);
						MERGE_BB(breathBB, g->breathBB);
    				}
    			}
				ERASE_AN_ELEMENT_FROM_BB(breathBB, pos);
				ADD_AN_ELEMENT_TO_BB(atariBB, pos);
    			breath = bb_pop_cnt(breathBB);
				atari  = bb_pop_cnt(atariBB);
				if (breath != 1){
					atari = 0;
				}
			}
			atari = MIN(atari, 8);
			keep[x][y] = (atari == num);
		}
	}
	return 0;
}

static int writeLegality(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int keep[21][21])
{
	int x, y;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			int pos = X_Y_TO_POS(x, y);
			keep[x][y] = (ban->color[pos] == SP && judge_eff_te(pos, ban));
		}
	}
	return 0;
}
 
// そこに打つと、相手の石をシチョウで取れるかどうかをcheckする
static int writeLadderCapture(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int keep[21][21])
{
	int x, y, d;

	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			keep[x][y] = 0;
			int pos = X_Y_TO_POS(x, y);
			for (d = 1; d < D_MAX; d += 2){ /* cross */
				const int c = pos + D2DELTA[d];
				if (ban->color[c] == GET_AITE(TBN2COLOR(ban->tbn))){
					int renID = ban->renID[c];
					renInfo_t *g = &ban->renInfo[renID];
					if (g->breathPoints == 2){
						int target = get_another_pos(g->breathBB, pos);
						if (check_kill(pos, target, ban)){
							keep[POS_TO_X(pos)][POS_TO_Y(pos)] = 1;
						}
    				}
    			}
			}
		}
	}

	return 0;
}


// そこに打つと、シチョウで取られるかどうかをcheckする
static int writeLadderEscape(FILE *fp_out, const int id, const int tesuu, goban_t *ban, int keep[21][21])
{
	int x, y;

	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			keep[x][y] = 0;
			int pos = X_Y_TO_POS(x, y);
			if (teOK(pos, ban) &&
				!detect_suicide_or_capture(ban, pos)){
				if (check_dead(pos, ban)){
					keep[x][y] = 1;
				}
			}
			
		}
	}
	return 0;
}

int get_feature(FILE *fp, const int i, const int tesuu, goban_t *ban)
{
	int j;
	
	//Stone colour 3 Player stone / opponent stone / empty
	if (ban->tbn == TBN_BK){
		writeColor(fp, i, tesuu, ban, BK, ban->feature[0]);
		writeColor(fp, i, tesuu, ban, WH, ban->feature[1]);
	} else {
		writeColor(fp, i, tesuu, ban, WH, ban->feature[0]);
		writeColor(fp, i, tesuu, ban, BK, ban->feature[1]);
	}
	writeColor(fp, i, tesuu, ban, SP, ban->feature[2]);
	//Ones 1 A constant plane filled with 1
	writeSame(fp, i, tesuu, ban, 1, ban->feature[3]);
	
	//Turns since 8 How many turns since a move was played
	for (j = 1; j <= 8; j++){
		writeHist(fp, i, tesuu, ban, j, ban->feature[3 + j]);
	}
	//Liberties 8 Number of liberties (empty adjacent points)
	for (j = 1; j <= 8; j++){
		writeLiberty(fp, i, tesuu, ban, j, ban->feature[11 + j]);
	}
	//Capture size 8 How many opponent stones would be captured
	for (j = 1; j <= 8; j++){
		writeCaptureSize(fp, i, tesuu, ban, j, ban->feature[19 + j]);
	}
	//Self-atari size 8 How many of own stones would be captured
	for (j = 1; j <= 8; j++){
		writeCapturedSize(fp, i, tesuu, ban, j, ban->feature[27 + j]);
	}
	//Liberties after move 8 Number of liberties after this move is played
	for (j = 1; j <= 8; j++){
		writeLibertyAfterMoveSize(fp, i, tesuu, ban, j, ban->feature[35 + j]);
	}
	//Ladder capture 1 Whether a move at this point is a successful ladder capture
	writeLadderCapture(fp, i, tesuu, ban, ban->feature[44]); // 20160616
	
	//Ladder escape 1 Whether a move at this point is a successful ladder escape
	writeLadderEscape(fp, i, tesuu, ban, ban->feature[45]);
	
	//Sensibleness 1 Whether a move is legal and does not fill its own eyes
	writeLegality(fp, i, tesuu, ban, ban->feature[46]);
			
	//Zeros 1 A constant plane filled with 0
	writeSame(fp, i, tesuu, ban, 0, ban->feature[47]);
	return 0;
}
