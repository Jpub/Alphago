#include "go.h"

#define neighbor_count_at(ban, pos, color2) \
  ((ban->color[pos + D2DELTA[1]] == color2) +	\
   (ban->color[pos + D2DELTA[3]] == color2) +	\
   (ban->color[pos + D2DELTA[5]] == color2) +	\
   (ban->color[pos + D2DELTA[7]] == color2))

int outPos(const int pos)
{
  fprintf(stderr, "(%d %d)", POS_TO_X(pos), POS_TO_Y(pos));
  return 0;
}

int judgeKoNG(const int pos, goban_t *ban)
{
  history_t *histInfo = &ban->histInfo[ban->tesuu - 1];
  assert (ban->tesuu >= 1);
  return (pos == histInfo->ko);
}

/* not judging KO ng move*/
int teOK(const int xy, goban_t *ban)
{
	const int color = TBN2COLOR(ban->tbn);
	int d;

	if (xy == 0)
		return 1;
	if (IN_RANGE(0, xy, XY_SIZE - 1) && ban->color[xy] == SP){
		for (d = 1; d < D_MAX; d += 2){ /* cross */
			const int pos = xy + D2DELTA[d];
			const int renID = ban->renID[pos];
			const int colorD = ban->color[pos];
			if (colorD == SP){
				return 1;
			}
			if (colorD == color && ban->renInfo[renID].breathPoints > 1){
				return 1;
			}
			if (colorD == GET_AITE(color) &&  ban->renInfo[renID].breathPoints == 1){
				return 1;
			}
		}
	}
	return 0;
}
	
/* REQUIRE : ban->color[pos] == SP

   合法手でなければ NG
   合法手でも、四方を味方石で囲まれた位置は、
   取られそうな場合(味方の呼吸点1の連に接する手)を除き NG (return 0),
   そうでなければ return 1; */
int judge_eff_te(const int pos, goban_t *ban)
{
	const int colorAite = GET_AITE(TBN2COLOR(ban->tbn));
	int d, tgt, colorD;

	if (neighbor_count_at(ban, pos, SP) >= 1){
		return 1;
	}
	if (!teOK(pos, ban) || judgeKoNG(pos, ban)){
		return 0;
	}
	for (d = 1; d < D_MAX; d += 2){
		tgt = pos + D2DELTA[d];
		colorD = ban->color[tgt];
		if (colorD == OB)
			continue;

		if (colorD == SP || colorD == colorAite){
			return 1;
		}
		if (ban->renInfo[ban->renID[tgt]].breathPoints == 1){
			return 1;
		}
	}
	return 0;
}

void writeBan(goban_t *ban)
{
	int x, y;

	for (x = 1; x <= BOARD_EDGE_SIZE; x++){
		fprintf(stderr, "%.1s", &COLOR_NAME[ban->color[x]]);
	}
	fprintf(stderr, "\n   [");
	for (x = 1; x <= BOARD_EDGE_SIZE; x++){
		fprintf(stderr, "%2d", x);
	}
	fprintf(stderr, "]\n");
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		fprintf(stderr, "[%2d] ", y);
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			fprintf(stderr, "%.1s ", &COLOR_NAME[ban->color[X_Y_TO_POS(x, y)]]);
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "tbn = %.1s, tesuu = %d, bk: %d, wh: %d  prev: %.1s",
	&COLOR_NAME[ban->tbn + 1], ban->tesuu, ban->prisoner[TBN_BK], ban->prisoner[TBN_WH], &COLOR_NAME[GET_AITE(ban->tbn + 1)]);
	outPos(ban->histInfo[ban->tesuu - 1].pos);
	return;
}
#define LEADING_ONES(x) ((1ULL << (x)) - 1ULL)

int get_around_bb(bb_t *aroundBB, bb_t *inBB)
{
	int k;
	for (k = 0; k < BB_IDX_SIZE; k++){
		aroundBB[k] = 
		(inBB[k] >> 1ULL) | (inBB[k] >> BB_EDGE)
		| ((inBB[k] & LEADING_ONES(NUM_PER_BB - 1)) << 1ULL)
		| ((inBB[k] & LEADING_ONES(NUM_PER_BB - BB_EDGE)) << BB_EDGE);
		if (k < BB_IDX_SIZE - 1 && inBB[k + 1]){
			aroundBB[k] |=
			((inBB[k + 1] & LEADING_ONES(BB_EDGE)) << (NUM_PER_BB - BB_EDGE))
			| ((inBB[k + 1] & LEADING_ONES(1)) << (NUM_PER_BB - 1));
		}
		if (k > 0 && inBB[k - 1]){
			aroundBB[k] |=
			(inBB[k - 1]  >> (NUM_PER_BB - BB_EDGE))
			|(inBB[k - 1]  >> (NUM_PER_BB - 1));
		}
	}
	return 0;
}

int is_danger_around(goban_t *ban, bb_t *target_occupiedBB, const int check_tbn)
{
	int k, target;
	bb_t aroundBB[BB_IDX_SIZE], bit;
	get_around_bb(aroundBB, target_occupiedBB);
	for (k = 0; k < BB_IDX_SIZE; k++){
		for (bit = aroundBB[k] & ban->occupiedBB[check_tbn][k]; bit; bit &= (bit - 1)){
			target = GET_POS(k, Get_FirstBit64(bit));
			if (ban->renInfo[ban->renID[target]].breathPoints == 1){
				return 1;
			}
		}
	}
	return 0;
}

int detect_suicide_or_capture(goban_t *ban, const int pos)
{
	int d;
	bb_t breathBB[BB_IDX_SIZE];

	CLEAR_BB(breathBB);
	ADD_AN_ELEMENT_TO_BB(breathBB, pos);

	for (d = 1; d < D_MAX; d += 2){ /* CROSS */
		const int target = pos + D2DELTA[d];
		if (ban->color[target] == SP){
			ADD_AN_ELEMENT_TO_BB(breathBB, target);
		}

		if (ban->color[target] == TBN2COLOR(ban->tbn)){
			renInfo_t *renInfo2 = &ban->renInfo[ban->renID[target]];
			MERGE_BB(breathBB, renInfo2->breathBB);
		}
		if (ban->color[target] == GET_AITE(TBN2COLOR(ban->tbn))){
			renInfo_t *renInfo2 = &ban->renInfo[ban->renID[target]];
			if (renInfo2->breathPoints == 1){  /* capture */
				return 1;
			}
		}
	}

	int breathPoints = bb_pop_cnt(breathBB);
	if (breathPoints >= 3){
		return 0;
	}

	assert (breathPoints == 2);
	return 1;
}

int check_kill(const int seme, const int escape,  goban_t *ban)
{
	const int prePos = ban->histInfo[ban->tesuu - 1].pos;
	int res = 0;
	assert (seme && escape);// && ban->color[seme] == SP && ban->color[escape] == SP);
	if (!teOK(seme, ban)|| judgeKoNG(seme, ban) || detect_suicide_or_capture(ban, seme)){
		return 0;
	}
	
	make_move(seme, ban);
	renInfo_t *renInfo = &ban->renInfo[ban->renID[seme]];
	if (renInfo->breathPoints >= 2 &&
		check_dead(escape, ban)){
		
		// semeの直前の相手石の連の、周囲の石(つまりsemeの方)を取られないかをcheck
		renInfo_t *renInfo2 = &ban->renInfo[ban->renID[prePos]];
		if (is_danger_around(ban, renInfo2->occupiedBB, AITE_TBN(ban->tbn))){
			res = 0;
		} else {
			res = 1;
		}
	}
	unmake_move(ban);
	return res;
}


int check_dead(const int pos, goban_t *ban)
{
	int res = 0, k, cand[2];
	if (ban->tesuu >= TESUU_MAX - 2){
		return 0;
	}
	make_move(pos, ban);
	
	renInfo_t *renInfo = &ban->renInfo[ban->renID[pos]];

	res = (renInfo->breathPoints == 1);

	if (renInfo->breathPoints == 2){
		get_double_pos(cand, renInfo->breathBB);
		for (k = 0; k < 2; k++){
			if (res == OFF){
				res = check_kill(cand[k], cand[1 - k], ban);
			}
		}
	}
	unmake_move(ban);
	return res;
}

int get_nb_stones(const int pos, int nb_num[4], int nb_stones[4][4], goban_t *ban)
{
	int d, tgt, colorD;

	assert(IN_RANGE(1, POS_TO_X(pos), BOARD_EDGE_SIZE) && IN_RANGE(1, POS_TO_Y(pos), BOARD_EDGE_SIZE));

	nb_num[0] = nb_num[1] = nb_num[2] =nb_num[3] = 0;

	for (d = 1; d < D_MAX; d += 2){ /* cross */
		tgt = pos + D2DELTA[d];
		colorD = ban->color[tgt];
		nb_stones[colorD][nb_num[colorD]++] = tgt;
	}
	return 0;
}

int bb_pop_cnt(bb_t *targetBB)
{
	int k, cnt = 0;
	for (k = 0; k < BB_IDX_SIZE; k++)
		cnt += pop_cnt(targetBB[k]);
	return cnt;
}

int get_double_pos(int *cand, bb_t *targetBB)
{
	int k, cnt = 0;
	unsigned long long bit;
	assert(bb_pop_cnt(targetBB) == 2);

	for (k = 0; k < BB_IDX_SIZE; k++){
		for (bit = targetBB[k]; bit; bit &= (bit - 1)){
			cand[cnt++] = GET_POS(k, Get_FirstBit64(bit));
		}
	}
	return 0;
}

int get_single_pos(bb_t *targetBB)
{
	int k;

	for (k = 0; k < BB_IDX_SIZE; k++){
		if (targetBB[k]){
			return GET_POS(k, Get_FirstBit64(targetBB[k]));
		}
	}
	return 0;
}

                       
int get_another_pos(bb_t *targetBB, const int exclude)
{
	int cand[2];
	get_double_pos(cand, targetBB);
	if (cand[0] == exclude){
		return cand[1];
	} else {
		return cand[0];
	}
	return 0;
}
double Tool_GetTimeNow()
{
	return clock()/(double)(CLOCKS_PER_SEC);
}
