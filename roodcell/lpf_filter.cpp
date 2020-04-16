#include <Arduino.h>
#include"lpf_filter.h"

/*取得したデータを順次挿入*/
void LPF_Buf_init(int* buf, int* reader,int tmp) {

	for (int cnt = 0; cnt < BUF_SIZE; cnt++) {
		(*(buf + (*reader))) = tmp;
		(*reader)++;
		if (*reader >= BUF_SIZE) {
			*reader = 0;
		}
	}
}
void LPF_update(int* iWeight,int* reader, int data) {
	(*(iWeight + *(reader))) = data;
	(*reader)++;
	if (*reader >= BUF_SIZE) {
		(*reader) = 0;
	}
}
/*取得したデータの平均を計算*/
int LPF_calc(int* iWeight,int* aveWeight,int* reader){
		int tmp=0;
		
		for (int cnt = 0; cnt < BUF_SIZE; cnt++) {
			tmp += (*(iWeight+(*reader)));
			(*reader)++;
			if (*reader >= BUF_SIZE) {
				(*reader) = 0;
			}
		}

		tmp /= BUF_SIZE;

		*aveWeight = tmp;
    return tmp;
}
