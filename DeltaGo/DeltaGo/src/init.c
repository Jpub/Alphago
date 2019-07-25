#include "go.h"

char *COLOR_NAME = {"+@o=\0"};

/* ¶ã‚©‚çŽžŒv‰ô‚è
   d = 0: upperleft,   d = 1: upper,   d = 2: upplerright,   d = 3: left,
   d = 4: right,   d = 5: lowerleft,   d = 6: lower,   d = 7: lowerright,
*/
int D2DELTA[D_MAX] = { -BB_EDGE - 1, -BB_EDGE, -BB_EDGE + 1, +1, +BB_EDGE + 1, +BB_EDGE, +BB_EDGE - 1, -1 };
parameter_t *cnn_param;

void set_initial_pos(goban_t *ban)
{
	int x, y, pos;
	memset (ban, 0, sizeof(goban_t));
	ban->tesuu = 1;
	ban->tbn   = TBN_BK;
	for (pos = 0; pos < XY_SIZE; pos++){
		x = POS_TO_X(pos);
		y = POS_TO_Y(pos);
		if (!(IN_RANGE(1, x, BOARD_EDGE_SIZE) && IN_RANGE(1, y, BOARD_EDGE_SIZE))){
			ban->color[pos] = OB;
		}
	}
	return;
}

game_hdl_t *open_game_hdl(int argc, char **argv)
{
	game_hdl_t *hdl;

	if ((hdl = calloc(1, sizeof(game_hdl_t))) == NULL ||
		(hdl->ban = calloc(1, sizeof(goban_t))) == NULL ||
		(cnn_param = calloc(1, sizeof(parameter_t))) == NULL){
		return NULL;
	}
	set_initial_pos(hdl->ban);
	return hdl;
}

void close_game_hdl(game_hdl_t *hdl)
{
	free(hdl->ban);
	free(hdl);
	return;
}
