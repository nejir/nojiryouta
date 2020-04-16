#include <HX711.h>
#include <Arduino.h>
#include "lpf_filter.h"

#define DT_PIN  2                     /*ポートの指定*/
#define SCK_PIN  3

float calibrate = -22.9;              /*重量調整用の値*/
 
HX711 Scale;

/************************************************
    体重の暗号化処理
 ***********************************************/
int padKey=0x2386;                    /*共通鍵*/
int oneTimePad(int gram){
  int encodeNum=gram^padKey;          /*暗号化処理*/
  return encodeNum;
  }

/************************************************
    セットアップ
    tare:現在の重さを基準とする
 ***********************************************/
void setup() {
  Serial.begin(9600);           /*9600bpsで通信*/
  Scale.begin(DT_PIN,SCK_PIN);
  Scale.set_scale();
  Scale.tare();                 /*Reset the scale to*/
}

    static int enNum=0;
    long gram = 0;
    static int read_weight=0;
    static int weight_box[BUF_SIZE];  /*BUF_SIIZE=5　重量のデータを一時保管*/
    static int AVG_weight=0;          /*平均重量の測定*/
    int Data_cnt=0;
    static int oneflag=0;             /*送信済みチェックフラグ*/
    static int send_flag=0;           /*送信用フラグ*/
void loop() {
    /*****************************************************************
      体重の測定
    ********************************************************************/
    Scale.set_scale(calibrate); //Adjust to this calibration factor
    //重量を四捨五入
    gram = (Scale.get_units(5) + 50)/100;
    if(gram < 30){              /*重量が3㎏未満の時、初期化処理*/
      gram = 0;
      oneflag=0;
      send_flag=0;
      LPF_Buf_init(weight_box, read_weight, 0);
      LPF_calc(weight_box,&AVG_weight,&read_weight);
    }
    /*****************************************************************
        重量が3kgを超えている間、
    ********************************************************************/
    while(gram > 30){
      if(oneflag==1){        /*フラフが1の時初期化*/
        LPF_Buf_init(weight_box, read_weight, 0);
        LPF_calc(weight_box,&AVG_weight,&read_weight);
        send_flag=1;
        break;
      }
      /*重量と平均重量の差が300g以上、重量3kg以上の時、計測し続ける*/
      for(;int(gram)-AVG_weight>3||AVG_weight-int(gram)>3&&int(gram)>30;)
      {
          gram = (Scale.get_units(5) + 50)/100;
          LPF_update(weight_box, &read_weight, int(gram));   /*取得したデータを順次配列に挿入*/
          LPF_calc(weight_box,&AVG_weight,&read_weight);     /*取得したデータの平均を計算*/
          /*テスト用プログラム、測定している重さとその平均を表示*/
          Serial.print("gram:");
          Serial.println(gram);
          Serial.print("AVG:");
          Serial.println(AVG_weight);
          delay(1000);
      }
      if(send_flag == 0){
       /********************************************
            体重、暗号化結果、複合結果の表示,送信
        *********************************************/
          /*テスト用重量表示*/
          Serial.print("平均重量：");
          Serial.println(AVG_weight);
          
          enNum=oneTimePad(AVG_weight); /*暗号化処理*/
          Serial.print("暗号：");        
            Serial.println(enNum);      /*暗号化データの送信*/
          LPF_Buf_init(weight_box, read_weight, 0);
          LPF_calc(weight_box,&AVG_weight,&read_weight);
          send_flag=1;                  /*送信済みなので１を代入*/
          oneflag=1;                    /*送信済みなので１を代入*/
          break;
       }
    }
    Scale.power_down();
    delay(10);
    Scale.power_up();
}
