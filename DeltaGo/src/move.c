#include "go.h"

static void add_new_ren(const int xy, int nb_num[4], int nb_stones[4][4],
                        goban_t *ban)
{
  const int color = ban->color[xy];
  renInfo_t *renInfo = &ban->renInfo[++ban->renNum];
  int i;

  renInfo->color = color;

  renInfo->occupiedPoints = 1;
  ADD_AN_ELEMENT_TO_BB(renInfo->occupiedBB, xy);

  renInfo->breathPoints = nb_num[SP];
  for (i = 0; i < nb_num[SP]; i++){
    ADD_AN_ELEMENT_TO_BB(renInfo->breathBB, nb_stones[SP][i]);
  }
  ban->renID[xy] = ban->renNum;

  return;
}

static void erase_ren(goban_t *ban)
{
  renInfo_t *renInfo = &ban->renInfo[ban->renNum--];
  memset(renInfo, 0, sizeof(renInfo_t));

  return;
}

static void merge_ren(const int pos, int nb_num[4], int nb_stones[4][4],
                      history_t *histInfo, goban_t *ban)
{
  const int color = ban->color[pos];
  const int renID = ban->renID[nb_stones[color][0]];
  renInfo_t *renInfo = &ban->renInfo[renID], *renInfo2;
  int i, k;
  unsigned long long bit;

  histInfo->renInfo       = *renInfo;
  histInfo->mergeNum = 0;
  histInfo->mergeRenID[histInfo->mergeNum++] = renID;

  /* renew renID, and merge occupiedBB & breathPoint */
  for (i = 1; i < nb_num[color]; i++){
    const int posNB = nb_stones[color][i];

    renInfo2 = &ban->renInfo[ban->renID[posNB]];
    if (!BELONGS_TO(renInfo->occupiedBB, posNB)){
      histInfo->mergeRenID[histInfo->mergeNum++] = ban->renID[posNB];
      for (k = 0; k < BB_IDX_SIZE; k++){
        for (bit = renInfo2->occupiedBB[k]; bit; bit &= (bit - 1)){
          ban->renID[GET_POS(k, Get_FirstBit64(bit))] = renID;
        }
      }
      renInfo->occupiedPoints += renInfo2->occupiedPoints;
      MERGE_BB(renInfo->occupiedBB, renInfo2->occupiedBB);
      MERGE_BB(renInfo->breathBB, renInfo2->breathBB);
      renInfo2->deadFlag = ON;
    }
  }
  /* update occupied info */
  renInfo->occupiedPoints++;
  ADD_AN_ELEMENT_TO_BB(renInfo->occupiedBB, pos);
  ban->renID[pos] = renID;

  /* update breathBB */
  ERASE_AN_ELEMENT_FROM_BB(renInfo->breathBB, pos);

  for (i = 0; i < nb_num[SP]; i++){
    ADD_AN_ELEMENT_TO_BB(renInfo->breathBB, nb_stones[SP][i]);
  }
  /* re calc renInfo->breathPoints */
  renInfo->breathPoints = 0;
  for (k = 0; k < BB_IDX_SIZE; k++){
    renInfo->breathPoints += pop_cnt(renInfo->breathBB[k]);
  }
  return;
}


static void split_ren(const int pos,
                      history_t *histInfo, goban_t *ban)
{
  const int renID = histInfo->mergeRenID[0];
  renInfo_t *renInfo = &ban->renInfo[renID], *renInfo2;
  int i, k;
  unsigned long long bit;

  /* renew renID, and merge occupiedBB & breathPoint */
  for (i = 1; i < histInfo->mergeNum; i++){
    const int renID2 = histInfo->mergeRenID[i];

    renInfo2 = &ban->renInfo[renID2];

    for (k = 0; k < BB_IDX_SIZE; k++){
      for (bit = renInfo2->occupiedBB[k]; bit; bit &= (bit - 1)){
        ban->renID[GET_POS(k, Get_FirstBit64(bit))] = renID2;
      }
    }
    renInfo2->deadFlag = OFF;
  }
  *renInfo = histInfo->renInfo;

  return;
}

void make_move(const int pos, goban_t *ban)
{
  const int color = TBN2COLOR(ban->tbn);
  int nb_num[4], nb_stones[4][4];
  int d, i, k, posRemove;
  unsigned long long bit;
  history_t *histInfo = &ban->histInfo[ban->tesuu++];

  assert (ban->tesuu < TESUU_MAX);
  assert (ban->renNum < REN_MAX);

  histInfo->removeNum = 0;
  histInfo->ko        = 0;
  histInfo->pos       = pos;
  if (!pos){
    ban->tbn ^= 1;
    return;
  }
  assert (ban->color[pos] == SP);

  ban->color[pos] = color;
  ban->occupiedPoints[ban->tbn]++;
  ADD_AN_ELEMENT_TO_BB(ban->occupiedBB[ban->tbn], pos);

  /* update renn */
  get_nb_stones(pos, nb_num, nb_stones, ban);
  if (nb_num[color] == 0){
    add_new_ren(pos, nb_num, nb_stones, ban);
  } else {
    merge_ren(pos, nb_num, nb_stones, histInfo, ban);
  }
  /* erase opponent's breathPoint, and remove opponent's stone if any*/
  for (i = 0; i < nb_num[GET_AITE(color)]; i++){
    const int renID = ban->renID[nb_stones[GET_AITE(color)][i]];
    renInfo_t *renInfo = &ban->renInfo[renID];

    /* erase opponent's breathPoint around pos */
    if (BELONGS_TO(renInfo->breathBB, pos)){
      if (renInfo->breathPoints > 1){
        ERASE_AN_ELEMENT_FROM_BB(renInfo->breathBB, pos);
        renInfo->breathPoints--;
      } else { /* remove opponent's stone if no breath point */
        histInfo->removeRenID[histInfo->removeNum++] = renID;

	SPLIT_OUT_BB(ban->occupiedBB[ban->tbn^1], ban->renInfo[renID].occupiedBB);

	ban->renInfo[renID].deadFlag = ON;

	ban->occupiedPoints[ban->tbn^1] -= renInfo->occupiedPoints;

        ban->prisoner[ban->tbn] += renInfo->occupiedPoints;
        for (k = 0; k < BB_IDX_SIZE; k++){
          for (bit = renInfo->occupiedBB[k]; bit; bit &= (bit - 1)){

            posRemove = GET_POS(k, Get_FirstBit64(bit));

            ban->renID[posRemove] = 0;
            ban->color[posRemove] = SP;

            /* update teban's breath point by stone removal*/
            for (d = 1; d < D_MAX; d += 2){
              int tgt = posRemove + D2DELTA[d];
              renInfo_t *renInfo2 = &ban->renInfo[ban->renID[tgt]];

              if (color == ban->color[tgt]){
                if (!BELONGS_TO(renInfo2->breathBB, posRemove)){
                  ADD_AN_ELEMENT_TO_BB(renInfo2->breathBB, posRemove);
                  renInfo2->breathPoints++;
                }
              }
            }
          }
        }
      }
    }
  }
  /* judge ko */
  if (histInfo->removeNum == 1 &&
      ban->renInfo[ban->renID[pos]].occupiedPoints == 1 &&
      ban->renInfo[ban->renID[pos]].breathPoints == 1 &&
      ban->renInfo[histInfo->removeRenID[0]].occupiedPoints == 1){


    histInfo->ko = get_single_pos(ban->renInfo[histInfo->removeRenID[0]].occupiedBB);

  }
  /* update tbn */
  ban->tbn ^= 1;


  return;
}

void unmake_move(goban_t *ban)
{
  int color, d, i, k, posReset;
  unsigned long long bit;
  history_t *histInfo = &ban->histInfo[--ban->tesuu];
  const int pos = histInfo->pos;
  int tgt, cnt = 0;
  renInfo_t *renInfo;

  /* update tbn */
  ban->tbn ^= 1;

  if (!pos){
    return;
  }

  color = TBN2COLOR(ban->tbn);
  assert (ban->color[pos] == color);

  ban->color[pos] = SP;
  ban->renID[pos] = 0;
  ERASE_AN_ELEMENT_FROM_BB(ban->occupiedBB[ban->tbn], pos);
  ban->occupiedPoints[ban->tbn]--;

  for (d = 1; d < D_MAX; d += 2){
    tgt = pos + D2DELTA[d];
    cnt += (ban->color[tgt] == color);
    /* add opponent's breathPoint */
    if (ban->color[tgt] == GET_AITE(color)){
      renInfo = &ban->renInfo[ban->renID[tgt]];
      if (!BELONGS_TO(renInfo->breathBB, pos)){
	ADD_AN_ELEMENT_TO_BB(renInfo->breathBB, pos);
	renInfo->breathPoints++;
      }
    }
  }
  /* update mikata's ren */
  if (cnt == 0){ /* if (no mikata's stone around pos) */
    erase_ren(ban);
  } else {
    split_ren(pos, histInfo, ban);
  }
  /* re set opponent's stone if any */
  for (i = 0; i < histInfo->removeNum; i++){
    const int renID = histInfo->removeRenID[i];
    renInfo_t *renInfo = &ban->renInfo[renID];

    ban->renInfo[renID].deadFlag = OFF;

    ban->prisoner[ban->tbn] -= renInfo->occupiedPoints;
    ban->occupiedPoints[ban->tbn^1] += renInfo->occupiedPoints;
    MERGE_BB(ban->occupiedBB[ban->tbn^1], renInfo->occupiedBB);

    for (k = 0; k < BB_IDX_SIZE; k++){
      for (bit = renInfo->occupiedBB[k]; bit; bit &= (bit - 1)){
        posReset = GET_POS(k, Get_FirstBit64(bit));
        ban->renID[posReset] = renID;
        ban->color[posReset] = GET_AITE(color);
        /* update teban's breath point by stone re set */
        for (d = 1; d < D_MAX; d += 2){
          int tgt = posReset + D2DELTA[d];
          renInfo_t *renInfo2 = &ban->renInfo[ban->renID[tgt]];

          if (color == ban->color[tgt]){
            if (BELONGS_TO(renInfo2->breathBB, posReset)){
              ERASE_AN_ELEMENT_FROM_BB(renInfo2->breathBB, posReset);
              renInfo2->breathPoints--;
            }
          }
        }
      }
    }
  }

  return;
}
