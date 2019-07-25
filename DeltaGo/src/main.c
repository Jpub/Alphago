/* Copyright (C) 2016  Otsuki Tomoshi <tomoshi@m00.itscom.net> */
#include "go.h"
#include <stdarg.h>

const char *X_CORD = {"-ABCDEFGHJKLMNOPQRST-\0"};
const char *(Y_CORD[BOARD_EDGE_SIZE + 2]) = {"-",
	 "1",    "2",  "3",   "4",  "5",  "6",  "7",  "8",  "9", "10",  "11",  "12", "13",  "14", "15", "16", "17", "18", "19"
};

void send_gtp(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap );  // 標準出力に書き出す
	va_end(ap);
}

// 相手の手を受信 "play W D17" のように来る
int get_receive_pos(int *tbn, char *str)
{
	int pos_x = 0, pos_y = 0, x, y, y2;

	if (!strncmp(&str[5], "W", 1)){
		*tbn = TBN_WH;
	} else if (!strncmp(&str[5], "B", 1)){
		*tbn = TBN_BK;
	} else {
		fprintf(stderr, "tbn_err\n");
	}
	for (x = 1; x <= BOARD_EDGE_SIZE; x++){
		if (!strncmp(&str[7], &X_CORD[x], 1)){
			pos_x = x;
			break;
		}
	}

	for (y = 1; y <= 9; y++){ /* 1 keta me */
		if (!strncmp(&str[8], Y_CORD[y], 1)){
			if (y == 1){
				for (y2 = 10; y2 <= 19; y2++){  /* 2 keta me */
					if (!strncmp(&str[8], Y_CORD[y2], 2)){
						y = y2;
						break;
					}
				}
			}
			pos_y = BOARD_EDGE_SIZE + 1 - y;
			break;
		}
	}

	fprintf(stderr, "x = %d, y = %d\n", pos_x, pos_y);
	return X_Y_TO_POS(pos_x, pos_y);
}


int gtp(game_hdl_t *hdl)
{
	char str[FRAME];
	goban_t *ban = hdl->ban;

	// stdoutをバッファリングしないように。GTPで通信に失敗するので。
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);  // stderrに書くとGoGuiに表示される。
	while (1) {
		writeBan(ban);

		fprintf(stderr, "  NOW: tesuu: %d, tbn: %.1s,  (PREV): ", ban->tesuu, &COLOR_NAME[TBN2COLOR(ban->tbn)]);
		outPos(ban->histInfo[ban->tesuu - 1].pos), fprintf(stderr, "\n");

		if (fgets(str, FRAME, stdin)==NULL ) break;  // 標準入力から読む

		if (strstr(str,"boardsize")) {
			send_gtp("= \n\n");    // "boardsize 19": 19路盤
			if (atoi(&str[10]) != BOARD_EDGE_SIZE){
				fprintf(stderr, "illegal boardsize (%d != %d)\n", atoi(&str[10]), BOARD_EDGE_SIZE);
			}
			fprintf( stderr, "if you write in stderr, output into gogui\n");
		} else if (strstr(str,"clear_board")){
			set_initial_pos(ban);
			send_gtp("= \n\n");
		} else if (strstr(str,"komi")){
			ban->komi  = atof(&str[5]);
			fprintf(stderr, "komi = %f\n", ban->komi);
			send_gtp("= \n\n");
		} else if (strstr(str,"undo")){
			if (ban->tesuu >= 3){
				unmake_move(ban);
				unmake_move(ban);
			}
			send_gtp("= \n\n");
		} else if (strstr(str,"name")){
			send_gtp("= deltaGo\n\n");
		} else if ( strstr(str,"protocol_version")){
			send_gtp("= 2\n\n");
		} else if ( strstr(str,"version")){
			send_gtp("= 1.0.0\n\n");
		} else if ( strstr(str,"genmove w") || strstr(str,"genmove b")){
 			int x, y, pos;
			double pos2prob[XY_SIZE];
			fprintf( stderr, "I'm thinking... \n");
			double time_s = Tool_GetTimeNow();
			
			get_feature(NULL, 0, ban->tesuu - 1, ban);
			pos = get_cnn_prob(ban, hdl, pos2prob);
			if (pos == 0){
				fprintf(stderr, "PASS\n\n");
				send_gtp("= PASS\n\n");
			} else {
				x = POS_TO_X(pos);
				y = POS_TO_Y(pos);
				fprintf(stderr, "%.1s%s\n\n", &X_CORD[x], Y_CORD[y]);
				send_gtp("= %.1s%s\n\n", &X_CORD[x], Y_CORD[BOARD_EDGE_SIZE + 1  - y]);
			}
			outPos(pos);
			fprintf(stderr, "\ntime = %f\n", Tool_GetTimeNow() - time_s);
			make_move(pos, ban);
		} else if ( strstr(str,"play")){ // 相手の手を受信 "play W D17" のように来る
			int tbn;
			int pos = get_receive_pos(&tbn, str);
			if (tbn == ban->tbn){
				make_move(pos, ban);
			} else {  // 置き碁の場合?はPASSを挿入
				make_move(0, ban);
				make_move(pos, ban);
			}
			send_gtp("= \n\n");
		} else {
			send_gtp("= \n\n");  // それ以外のコマンドにはOKを返す
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	game_hdl_t *hdl;
	if ((hdl = open_game_hdl(argc, argv)) == NULL){
		fprintf(stderr, "open game_hdl error\n");
		return 0;
	}
	// read cnn parameter
	FILE *fp_in;
	if ((fp_in = fopen("paramter.bin","rb")) == NULL){
		fprintf(stderr, "file open error: %s\n", "paramter.bin");
		return 0;
	}
	fread(cnn_param,sizeof(cnn_t), sizeof(parameter_t)/sizeof(cnn_t), fp_in); 
	gtp(hdl);
	return 0;
}
