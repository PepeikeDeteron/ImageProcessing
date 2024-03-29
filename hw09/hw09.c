#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void affine(FILE *fp, int width, int width2, int height, int height2, int maxdepth2, unsigned char gray[], unsigned char gray_out[]);


int main(int argc, char *argv[])
{
  FILE *fp, *fp2;
  int width, width2, height, height2, maxdepth, maxdepth2;
  char buf[128];
  unsigned char *gray, *gray_out;

  // 第一引数に入力画像 (source.pgm)，第二引数に目的画像 (destination.pgm)を指定する．
  if (argc != 3)                             goto ARGUMENT_ERR;
  if ((fp  = fopen(argv[1], "rb")) == NULL)  goto FILE_ERR;
  if ((fp2 = fopen(argv[2], "rb")) == NULL)  goto FILE_ERR;

  // 入力画像
  fgets(buf, sizeof(buf), fp);
  fgets(buf, sizeof(buf), fp);
  fscanf(fp, "%d %d\n%d\n", &width, &height, &maxdepth);

  // 目的画像
  fgets(buf, sizeof(buf), fp2);
  fgets(buf, sizeof(buf), fp2);
  fscanf(fp2, "%d %d\n%d\n", &width2, &height2, &maxdepth2);

  if ((gray = (unsigned char *)malloc(sizeof(unsigned char) * width * height)) == NULL) goto MEMORY_ERR;
  if ((gray_out = (unsigned char *)malloc(sizeof(unsigned char) * 700 * 750)) == NULL)  goto MEMORY_ERR;

  fread(gray, sizeof(unsigned char), width * height, fp);
  fread(gray_out, sizeof(unsigned char), width2 * height2, fp2);

  affine(fp, width, width2, height, height2, maxdepth2, gray, gray_out);

  free(gray_out);
  free(gray);

  fclose(fp2);
  fclose(fp);

  return 0;


  ARGUMENT_ERR:
    printf("コマンドライン引数の数は1つにしてください．\n");
    exit(1);

  FILE_ERR:
    printf("ファイルを開けません．\n");
    exit(1);

  MEMORY_ERR:
    printf("メモリ領域の確保に失敗しました．\n");
    exit(1);
}


// アフィン変換による画像の幾何学的歪みの除去
void affine(FILE *fp, int width, int width2, int height, int height2, int maxdepth2, unsigned char gray[], unsigned char gray_out[])
{
  memset(gray_out, 0, sizeof((unsigned char)gray_out));

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      int x = (int)(0.61641092 * i + (-0.1105587) * j + 137.763995);
      int y = (int)(0.10711459 * i + (0.6137211)  * j + 20.0363433);

      gray_out[x * height2 + y] = gray[i * height + j];
    }
  }

  fp = fopen("affine.pgm", "wb");
  fprintf(fp, "P5\n# Grayscale image\n%d %d\n%d\n", width2, height2, maxdepth2);
  fwrite(gray_out, sizeof(unsigned char), width2 * height2, fp);
}
