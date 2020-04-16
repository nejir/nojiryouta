#define BUF_SIZE 5

void LPF_Buf_init(int* buf, int* reader,int tmp);
int LPF_calc(int* iWeight, int* aveWeight, int* reader);
void LPF_update(int* iWeight, int* reader, int data);
