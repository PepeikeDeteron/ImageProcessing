#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void sobel_filter(FILE *fp, int width, int height, int maxdepth, unsigned char gray[], unsigned char sobel[]);
void laplatian_filter(FILE *fp, int width, int height, int maxdepth, unsigned char gray[], unsigned char laplatian[]);


int main(int argc, char *argv[])
{
  FILE *fp;                                   // ファイル構造体のポインタ変数
  int width, height, maxdepth;                // 幅， 高さ， 最大階調値
  char buf[64];                               // 形式とコメントの読み飛ばし用
  unsigned char *gray, *sobel, *laplatian;    // グレイスケール画像データ， Sobel画像データ, Laplatian画像データ


  if ((fp = fopen(argv[1], "rb")) == NULL)  goto FILE_ERR;
  if (argc != 2)                            goto ARGUMENT_ERR;

  fgets(buf, sizeof(buf), fp);
  fgets(buf, sizeof(buf), fp);
  fscanf(fp, "%d %d\n%d\n", &width, &height, &maxdepth);

  if ((gray      = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL)  goto MEMORY_ERR;
  if ((sobel     = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL)  goto MEMORY_ERR;
  if ((laplatian = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL)  goto MEMORY_ERR;

  fread(gray, sizeof(unsigned char), width * height, fp);

  sobel_filter(fp, width, height, maxdepth, gray, sobel);
  laplatian_filter(fp, width, height, maxdepth, gray, laplatian);

  free(gray);
  free(sobel);
  free(laplatian);

  fclose(fp);

  return 0;


  FILE_ERR:
    printf("ファイルを開けません．\n");
    exit(1);

  ARGUMENT_ERR:
    printf("コマンドライン引数の数は1つにしてください．\n");
    exit(1);

  MEMORY_ERR:
    printf("メモリ領域の確保に失敗しました．\n");
    exit(1);
}


// エッジ検出に用いられるSobelフィルタを実装
void sobel_filter(FILE *fp, int width, int height, int maxdepth, unsigned char gray[], unsigned char sobel[])
{
  // カーネル（水平方向・垂直方向のエッジ検出用係数行列）
  int karnelX[9] = {-1,  0,  1,
                    -2,  0,  2,
                    -1,  0,  1};
  int karnelY[9] = {-1, -2, -1,
                     0,  0,  0,
                     1,  2,  1};
  int neighbor[9];              // 注目画素の近傍領域
  int dataX, dataY, dataSum;    // 水平方向， 垂直方向， 新画素値


  // 隅4辺にはSobelフィルタを適用しない
  for (int i = 1; i < width - 1; i++) {
    for (int j = 1; j < height - 1; j++) {
      dataX = dataY = 0;

      // 近傍画素の値を代入
      neighbor[0] = gray[(i - 1) * height + (j - 1)];
      neighbor[1] = gray[(i - 1) * height + (j    )];
      neighbor[2] = gray[(i - 1) * height + (j + 1)];
      neighbor[3] = gray[(i    ) * height + (j - 1)];
      neighbor[4] = gray[(i    ) * height + (j    )];
      neighbor[5] = gray[(i    ) * height + (j + 1)];
      neighbor[6] = gray[(i + 1) * height + (j - 1)];
      neighbor[7] = gray[(i + 1) * height + (j    )];
      neighbor[8] = gray[(i + 1) * height + (j + 1)];

      // 水平方向・垂直方向の， 近傍画素とカーネルの畳み込み演算
      for (int k = 0; k < 9; k++) {
        dataX += neighbor[k] * karnelX[k];
        dataY += neighbor[k] * karnelY[k];
      }
      dataSum = sqrt(pow(dataX, 2) + pow(dataY, 2));

      // 255を超えた値は255， 0未満は絶対値に
      if (dataSum > 255)  dataSum = 255;
      if (dataSum < 0)    dataSum = abs(dataSum);

      sobel[(i - 1) * height + (j - 1)] = dataSum;
    }

    // ミラーリング
    for (int i = 0; i < width; i++) {
      if (i == 0)          sobel[i] = sobel[i + width + 1];
      if (i == width - 1)  sobel[i] = sobel[i + width - 1];
      else                 sobel[i] = sobel[i + width];
    }
    for (int j = 0; j < height; j++) {
      sobel[j * width] = sobel[j * width + 1];
    }
  }

  fp = fopen("sobel.pgm", "wb");
  fprintf(fp, "P5\n# Grayscale image\n%d %d\n%d\n", width, height, maxdepth);
  fwrite(sobel, sizeof(unsigned char), width * height, fp);
}


// エッジ検出に用いられる4近傍Laplatianフィルタを実装
void laplatian_filter(FILE *fp, int width, int height, int maxdepth, unsigned char gray[], unsigned char laplatian[])
{
  // カーネル(エッジ検出用係数行列)
  int karnel[9] = {0,  1,  0,
                   1, -4,  1,
                   0,  1,  0};
  int neighbor[9];    // 注目画素の近傍領域
  int dataSum;        // 新画素値


  for (int i = 1; i < width - 1; i++) {
    for (int j = 1; j < height - 1; j++) {
      dataSum = 0;

      neighbor[0] = gray[(i - 1) * height + (j - 1)];
      neighbor[1] = gray[(i - 1) * height + (j    )];
      neighbor[2] = gray[(i - 1) * height + (j + 1)];
      neighbor[3] = gray[(i    ) * height + (j - 1)];
      neighbor[4] = gray[(i    ) * height + (j    )];
      neighbor[5] = gray[(i    ) * height + (j + 1)];
      neighbor[6] = gray[(i + 1) * height + (j - 1)];
      neighbor[7] = gray[(i + 1) * height + (j    )];
      neighbor[8] = gray[(i + 1) * height + (j + 1)];

      for (int k = 0; k < 9; k++)  dataSum += neighbor[k] * karnel[k];
      dataSum = abs(dataSum);

      if (dataSum > 255)  dataSum = 255;

      laplatian[(i - 1) * height + (j - 1)] = dataSum;
    }
  }

  // ミラーリング
  for (int i = 0; i < width; i++) {
    if (i == 0)          laplatian[i] = laplatian[i + width + 1];
    if (i == width - 1)  laplatian[i] = laplatian[i + width - 1];
    else                 laplatian[i] = laplatian[i + width];
  }
  for (int j = 0; j < height; j++) {
    laplatian[j * width] = laplatian[j * width + 1];
  }

  fp = fopen("laplatian.pgm", "wb");
  fprintf(fp, "P5\n# Grayscale image\n%d %d\n%d\n", width, height, maxdepth);
  fwrite(laplatian, sizeof(unsigned char), width * height, fp);
}