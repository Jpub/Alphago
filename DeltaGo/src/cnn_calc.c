#include "go.h"
#ifdef USE_SSE
#include <immintrin.h>
#endif

inline int get_hiddenNext_5x5(game_hdl_t *hdl,  cnn_t hiddenNext[21][21][FILTER_SIZE], cnn_t hiddenPrev[23][23][CHANNEL_SIZE], 
	const int x, const int y, const int inputNums, const int outputNums)
{
	int input, output;
	
	
	for (input = 0; input < inputNums; input++){
		// for (output = 0; output < outputNums; output++)for (q = 0; q < 5; q++)	for (p = 0; p < 5; p++)
		// hiddenNext[x][y][output]  += hiddenPrev[(x) + (p) - 1][(y) + (q) - 1][input] * cnn_5x5_orig[(p) + (q) * 5]

		cnn_t i00 = hiddenPrev[(x) + (0) - 1][(y) + (0) - 1][input];
		cnn_t i01 = hiddenPrev[(x) + (0) - 1][(y) + (1) - 1][input];
		cnn_t i02 = hiddenPrev[(x) + (0) - 1][(y) + (2) - 1][input];
		cnn_t i03 = hiddenPrev[(x) + (0) - 1][(y) + (3) - 1][input];
		cnn_t i04 = hiddenPrev[(x) + (0) - 1][(y) + (4) - 1][input];
		cnn_t i10 = hiddenPrev[(x) + (1) - 1][(y) + (0) - 1][input];
		cnn_t i11 = hiddenPrev[(x) + (1) - 1][(y) + (1) - 1][input];
		cnn_t i12 = hiddenPrev[(x) + (1) - 1][(y) + (2) - 1][input];
		cnn_t i13 = hiddenPrev[(x) + (1) - 1][(y) + (3) - 1][input];
		cnn_t i14 = hiddenPrev[(x) + (1) - 1][(y) + (4) - 1][input];
		cnn_t i20 = hiddenPrev[(x) + (2) - 1][(y) + (0) - 1][input];
		cnn_t i21 = hiddenPrev[(x) + (2) - 1][(y) + (1) - 1][input];
		cnn_t i22 = hiddenPrev[(x) + (2) - 1][(y) + (2) - 1][input];
		cnn_t i23 = hiddenPrev[(x) + (2) - 1][(y) + (3) - 1][input];
		cnn_t i24 = hiddenPrev[(x) + (2) - 1][(y) + (4) - 1][input];
		cnn_t i30 = hiddenPrev[(x) + (3) - 1][(y) + (0) - 1][input];
		cnn_t i31 = hiddenPrev[(x) + (3) - 1][(y) + (1) - 1][input];
		cnn_t i32 = hiddenPrev[(x) + (3) - 1][(y) + (2) - 1][input];
		cnn_t i33 = hiddenPrev[(x) + (3) - 1][(y) + (3) - 1][input];
		cnn_t i34 = hiddenPrev[(x) + (3) - 1][(y) + (4) - 1][input];
		cnn_t i40 = hiddenPrev[(x) + (4) - 1][(y) + (0) - 1][input];
		cnn_t i41 = hiddenPrev[(x) + (4) - 1][(y) + (1) - 1][input];
		cnn_t i42 = hiddenPrev[(x) + (4) - 1][(y) + (2) - 1][input];
		cnn_t i43 = hiddenPrev[(x) + (4) - 1][(y) + (3) - 1][input];
		cnn_t i44 = hiddenPrev[(x) + (4) - 1][(y) + (4) - 1][input];
#ifdef USE_SSE
		assert (outputNums %VEC_WIDTH == 0);
		
		__m256 i000 = _mm256_set1_ps(i00);
		__m256 i001 = _mm256_set1_ps(i01);
		__m256 i002 = _mm256_set1_ps(i02);
		__m256 i003 = _mm256_set1_ps(i03);
		__m256 i004 = _mm256_set1_ps(i04);

		__m256 i010 = _mm256_set1_ps(i10);
		__m256 i011 = _mm256_set1_ps(i11);
		__m256 i012 = _mm256_set1_ps(i12);
		__m256 i013 = _mm256_set1_ps(i13);
		__m256 i014 = _mm256_set1_ps(i14);

		__m256 i020 = _mm256_set1_ps(i20);
		__m256 i021 = _mm256_set1_ps(i21);
		__m256 i022 = _mm256_set1_ps(i22);
		__m256 i023 = _mm256_set1_ps(i23);
		__m256 i024 = _mm256_set1_ps(i24);

		__m256 i030 = _mm256_set1_ps(i30);
		__m256 i031 = _mm256_set1_ps(i31);
		__m256 i032 = _mm256_set1_ps(i32);
		__m256 i033 = _mm256_set1_ps(i33);
		__m256 i034 = _mm256_set1_ps(i34);

		__m256 i040 = _mm256_set1_ps(i40);
		__m256 i041 = _mm256_set1_ps(i41);
		__m256 i042 = _mm256_set1_ps(i42);
		__m256 i043 = _mm256_set1_ps(i43);
		__m256 i044 = _mm256_set1_ps(i44);


		cnn_t *weight = cnn_param->weight_cnn_5x5[input][0];
		for (output = 0; output < outputNums; output += VEC_WIDTH){
			
			__m256 v = _mm256_mul_ps(_mm256_loadu_ps(&weight[0 * VEC_WIDTH]), i000); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[1 * VEC_WIDTH]), i010)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[2 * VEC_WIDTH]), i020));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[3 * VEC_WIDTH]), i030));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[4 * VEC_WIDTH]), i040));

			
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[5 * VEC_WIDTH]), i001));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[6 * VEC_WIDTH]), i011));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[7 * VEC_WIDTH]), i021));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[8 * VEC_WIDTH]), i031));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[9 * VEC_WIDTH]), i041));
			
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[10 * VEC_WIDTH]), i002));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[11 * VEC_WIDTH]), i012));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[12 * VEC_WIDTH]), i022));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[13 * VEC_WIDTH]), i032));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[14 * VEC_WIDTH]), i042));

			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[15 * VEC_WIDTH]), i003));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[16 * VEC_WIDTH]), i013));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[17 * VEC_WIDTH]), i023));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[18 * VEC_WIDTH]), i033));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[19 * VEC_WIDTH]), i043));

			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[20 * VEC_WIDTH]), i004));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[21 * VEC_WIDTH]), i014));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[22 * VEC_WIDTH]), i024));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[23 * VEC_WIDTH]), i034));
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[24 * VEC_WIDTH]), i044));

			__m256 prev = _mm256_loadu_ps(&hiddenNext[x][y][output]); 
			_mm256_storeu_ps(&hiddenNext[x][y][output], _mm256_add_ps(prev,v));
			weight += VEC_WIDTH * 25;
		}
#else
		
		for (output = 0; output < outputNums; output += VEC_WIDTH){	
			cnn_t *weight = cnn_param->weight_cnn_5x5[input][output];
			int j;

			for (j = 0; j < VEC_WIDTH; j++){
				hiddenNext[x][y][output + j] += 
				i00 * weight[ 0 * 8 + j] + i10 * weight[ 1 * 8 + j] + i20 * weight[ 2 * 8 + j] + i30 * weight[ 3 * 8 + j] + i40 * weight[ 4 * 8 + j] +
				i01 * weight[ 5 * 8 + j] + i11 * weight[ 6 * 8 + j] + i21 * weight[ 7 * 8 + j] + i31 * weight[ 8 * 8 + j] + i41 * weight[ 9 * 8 + j] +
				i02 * weight[10 * 8 + j] + i12 * weight[11 * 8 + j] + i22 * weight[12 * 8 + j] + i32 * weight[13 * 8 + j] + i42 * weight[14 * 8 + j] +
				i03 * weight[15 * 8 + j] + i13 * weight[16 * 8 + j] + i23 * weight[17 * 8 + j] + i33 * weight[18 * 8 + j] + i43 * weight[19 * 8 + j] +
				i04 * weight[20 * 8 + j] + i14 * weight[21 * 8 + j] + i24 * weight[22 * 8 + j] + i34 * weight[23 * 8 + j] + i44 * weight[24 * 8 + j];
			}
		}
#endif
	}

	for (output = 0; output < outputNums; output++){
		if (hiddenNext[x][y][output] <= 0.0){
			hiddenNext[x][y][output] = 0.0;
		}
	}
	return 0;
}

static int convolution5x5 (cnn_t hiddenNext[21][21][FILTER_SIZE], cnn_t hiddenPrev[23][23][CHANNEL_SIZE], game_hdl_t *hdl, 
			const int outputNums, const int inputNums)
{
	int x;

	for (x = 1; x <= 19; x++){
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 1, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 2, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 3, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 4, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 5, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 6, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 7, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 8, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x, 9, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,10, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,11, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,12, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,13, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,14, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,15, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,16, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,17, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,18, inputNums, outputNums);
		get_hiddenNext_5x5(hdl, hiddenNext, hiddenPrev, x,19, inputNums, outputNums);
	}
	return 0;
}


inline int get_hiddenNext_3x3(game_hdl_t *hdl,  cnn_t hiddenNext[21][21][FILTER_SIZE], cnn_t hiddenPrev[21][21][FILTER_SIZE], 
	const int x, const int y, const int inputNums, const int outputNums, const int layer)
{
	int input, output;
	
	for (input = 0; input < inputNums; input++){
		//for (output = 0; output < outputNums; output++)for (q = 0; q < 3; q++)for (p = 0; p < 3; p++)
		// hiddenNext[x][y][output]  += hiddenPrev[(x) + (p) - 1][(y) + (q) - 1][input] * cnn_3x3_orig[(p) + (q) * 3]
		
		cnn_t i00 = hiddenPrev[(x) + (0) - 1][(y) + (0) - 1][input];
		cnn_t i01 = hiddenPrev[(x) + (0) - 1][(y) + (1) - 1][input];
		cnn_t i02 = hiddenPrev[(x) + (0) - 1][(y) + (2) - 1][input];
		cnn_t i10 = hiddenPrev[(x) + (1) - 1][(y) + (0) - 1][input];
		cnn_t i11 = hiddenPrev[(x) + (1) - 1][(y) + (1) - 1][input];
		cnn_t i12 = hiddenPrev[(x) + (1) - 1][(y) + (2) - 1][input];
		cnn_t i20 = hiddenPrev[(x) + (2) - 1][(y) + (0) - 1][input];
		cnn_t i21 = hiddenPrev[(x) + (2) - 1][(y) + (1) - 1][input];
		cnn_t i22 = hiddenPrev[(x) + (2) - 1][(y) + (2) - 1][input];
		
#ifdef USE_SSE
		assert (outputNums %VEC_WIDTH == 0);
		
		__m256 i000 = _mm256_set1_ps(i00);
		__m256 i001 = _mm256_set1_ps(i01);
		__m256 i002 = _mm256_set1_ps(i02);
		__m256 i010 = _mm256_set1_ps(i10);
		__m256 i011 = _mm256_set1_ps(i11);
		__m256 i012 = _mm256_set1_ps(i12);
		__m256 i020 = _mm256_set1_ps(i20);
		__m256 i021 = _mm256_set1_ps(i21);
		__m256 i022 = _mm256_set1_ps(i22);
		cnn_t *weight = cnn_param->weight_cnn_3x3[layer][input][0];
		for (output = 0; output < outputNums; output += VEC_WIDTH){
			
			__m256 v = _mm256_mul_ps(_mm256_loadu_ps(&weight[0 * VEC_WIDTH]), i000); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[1 * VEC_WIDTH]), i010)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[2 * VEC_WIDTH]), i020)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[3 * VEC_WIDTH]), i001)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[4 * VEC_WIDTH]), i011)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[5 * VEC_WIDTH]), i021)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[6 * VEC_WIDTH]), i002)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[7 * VEC_WIDTH]), i012)); 
			v = _mm256_add_ps(v, _mm256_mul_ps(_mm256_loadu_ps(&weight[8 * VEC_WIDTH]), i022)); 
			__m256 prev = _mm256_loadu_ps(&hiddenNext[x][y][output]); 
			_mm256_storeu_ps(&hiddenNext[x][y][output], _mm256_add_ps(prev,v));
			weight += VEC_WIDTH * 9;
		}
		
#else
		int j;
		for (output = 0; output < outputNums; output += VEC_WIDTH){	
			cnn_t *weight = cnn_param->weight_cnn_3x3[layer][input][output];

			for (j = 0; j <  VEC_WIDTH; j++){
				hiddenNext[x][y][output + j] += i00 * weight[0 * 8 + j] + i10 * weight[1 * 8 + j] + i20 * weight[2 * 8 + j] + 
												i01 * weight[3 * 8 + j] + i11 * weight[4 * 8 + j] + i21 * weight[5 * 8 + j] + 
												i02 * weight[6 * 8 + j] + i12 * weight[7 * 8 + j] + i22 * weight[8 * 8 + j];
			}
		}
#endif
	}
	for (output = 0; output < outputNums; output++){
		if (hiddenNext[x][y][output] <= 0.0){
			hiddenNext[x][y][output] = 0.0;
		}
	}
	return 0;
}

static int convolution3x3 (const int layer, cnn_t hiddenNext[21][21][FILTER_SIZE], cnn_t hiddenPrev[21][21][FILTER_SIZE], game_hdl_t *hdl, 
			const int outputNums, const int inputNums)
{
	int x;

	for (x = 1; x <= 19; x++){
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 1, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 2, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 3, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 4, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 5, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 6, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 7, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 8, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x, 9, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,10, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,11, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,12, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,13, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,14, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,15, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,16, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,17, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,18, inputNums, outputNums, layer);
		get_hiddenNext_3x3(hdl, hiddenNext, hiddenPrev, x,19, inputNums, outputNums, layer);
	}
	return 0;
}

static inline int convolution13 (cnn_t hiddenOut[21][21], cnn_t hiddenPrev[21][21][FILTER_SIZE], game_hdl_t *hdl, const int inputNums)
{
	int input, x, y;
	
	for (x = 1; x <= 19; x++){
		for (y = 1; y <= 19; y++){
			cnn_t tmp = cnn_param->weight_cnn_bias[(x - 1) + 19 * (y - 1)];
			for (input = 0; input < inputNums; input++){
				tmp += hiddenPrev[x][y][input] * cnn_param->weight_cnn_1x1[input];
			}
			hiddenOut[x][y] = tmp;
		}
	}
	
	return 0;
}

int get_cnn_prob(goban_t *ban, game_hdl_t *hdl, double pos_to_prob[XY_SIZE])
{
	int x, y, input;
	for (input = 0; input < CHANNEL_SIZE; input++){
		for (x = 1; x <= 19; x++){
			for (y = 1; y <= 19; y++){
				hdl->hiddenIn[x + 1][y + 1][input] = (cnn_t)ban->feature[input][x][y]; 
			}
		}
	}
	
	memset (hdl->hidden, 0, 14 * 21 * 21 * FILTER_SIZE * sizeof(cnn_t));
	memset (hdl->hiddenOut, 0, 21 * 21 * sizeof(cnn_t));
	
	convolution5x5(    hdl->hidden[1],  hdl->hiddenIn,   hdl, FILTER_SIZE, CHANNEL_SIZE);
	convolution3x3(2,  hdl->hidden[2],  hdl->hidden[1],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(3,  hdl->hidden[3],  hdl->hidden[2],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(4,  hdl->hidden[4],  hdl->hidden[3],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(5,  hdl->hidden[5],  hdl->hidden[4],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(6,  hdl->hidden[6],  hdl->hidden[5],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(7,  hdl->hidden[7],  hdl->hidden[6],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(8,  hdl->hidden[8],  hdl->hidden[7],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(9,  hdl->hidden[9],  hdl->hidden[8],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(10, hdl->hidden[10], hdl->hidden[9],  hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(11, hdl->hidden[11], hdl->hidden[10], hdl, FILTER_SIZE, FILTER_SIZE);
	convolution3x3(12, hdl->hidden[12], hdl->hidden[11], hdl, FILTER_SIZE, FILTER_SIZE);
	convolution13(     hdl->hiddenOut, hdl->hidden[12], hdl, FILTER_SIZE);	
	
	double max = -1000000.0;
	int best_pos = 0;
	double prob;
	// get prob_tot
	double prob_tot = 0.0;
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			prob = exp(hdl->hiddenOut[x][y]);
			prob_tot += prob;
		}
	}
	if (prob_tot == 0.0){
		prob_tot = 1.0;
	}
	// get best_pos and print prob
	for (y = 1; y <= BOARD_EDGE_SIZE ; y++){
		for (x = 1; x <= BOARD_EDGE_SIZE; x++){
			int pos = X_Y_TO_POS(x, y);
			double prob = exp(hdl->hiddenOut[x][y]);
			pos_to_prob[pos] = prob/prob_tot;
			if (ban->color[pos] == SP && judge_eff_te(pos, ban)){ 
				if (prob >= max){
					max = prob;
					best_pos = pos;
				}
			}
		}
	}
	return best_pos;
}
