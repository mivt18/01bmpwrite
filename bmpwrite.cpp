#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <windows.h>

//comment rainber
int N, M, color,datasize;

void fillheader(char header[]) {
    int filesize;

    // BITMAPFILEHEADER
    BITMAPFILEHEADER bfh;
    ZeroMemory(&bfh, sizeof(bfh));
    
    bfh.bfType = 0x4d42;                        // сигнатура, должно быть 'BM'
    //bfh.bfSize = 54;                            // исправить размер файла
    bfh.bfReserved1 = 0;                        //
    bfh.bfReserved2 = 0;                        //
    //bfh.bfOffBits = color ? 54 + 256*4 : 54;    // начало пиксельных данных, чб добавляет размер палитры
	bfh.bfOffBits = color ? 54 : 54 + 256*4;    // начало пиксельных данных, чб добавляет размер палитры
    bfh.bfSize = bfh.bfOffBits + datasize;                            // исправить размер файла
	memcpy(header, &bfh, 14);                   // копируем в массив header

    // BITMAPINFOHEADER;
    BITMAPINFOHEADER bih;
    ZeroMemory(&bih, 40);

    bih.biSize = 40;                            // размер структуры BITMAPINFOHEADER
    bih.biWidth = N;                            // ширина
    bih.biHeight = M;                           // высота, положит означает что данные надо записывать снизу вверх
    bih.biPlanes = 1;                           //
    bih.biBitCount = color ? 24 : 8;            // число бит на пиксель
    memcpy(header + 14, &bih, 40);              // копируем в массив header
}

void fillpalette(char palette[]) {
    if (color == 2) return;
    // если чб, надо заполнять palette байтами
    // 0 0 0 0 1 1 1 0 2 2 2 0 3 3 3 0 ... 255 255 255 0
    for (int i = 0; i < 4 * 256; i+=4){
        palette[i] = i / 4;
        palette[i + 1] = i / 4;
        palette[i + 2] = i / 4;
        palette[i + 3] = 0;
    }
}

void filldata(char data[], int **r, int **g, int **b) {
    int bpp = color ? 3 : 1;
    int i, j, linesize = ceil(bpp * N / 4.0) * 4;
    // заполнить данные.
    // учесть: записывать снизу вверх, в цветном файле порядок b, g, r
    // в случае чб есть только b
    for(i = M - 1; i >= 0; i--){
        for(j = 0; j < N; j++){
            data[i*linesize + j * bpp] = b[M-1-i][j];
            if(color){
                data[i*linesize + j * bpp + 1] = g[M-1-i][j];
                data[i*linesize + j * bpp + 2] = r[M-1-i][j];
            }
        }
        for (j = linesize - 1;j >= N*bpp ;j--){
            data[i*linesize + j] = 0;
        }
    }
}

int main(char argc, char* argv[]) {
    int i, j, **r=0, **g=0, **b=0;
    std::ifstream f;
    char *filename; 
    if (argc > 1) filename = argv[1]; else filename = "input.txt";
    f = std::ifstream(filename);
    if (f.fail()) {
        std::cerr << "could not open file\n";
        return -1;
    }

    f >> N >> M >> color;

    // выделяем память под один (для чб) или три (для цв) массива MxN 
    b = new int*[M]; for (i = 0; i < M; i++) b[i] = new int[N];
    if (color) {
        g = new int*[M]; for (i = 0; i < M; i++) g[i] = new int[N];
        r = new int*[M]; for (i = 0; i < M; i++) r[i] = new int[N];
    }

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            if (color)
                f >> r[i][j] >> g[i][j] >> b[i][j];
            else
                f >> b[i][j];
        }
    }
    f.close();

    char *bmpfilename = new char[strlen(filename) + 4];
    strcpy(bmpfilename, filename);
    strcat(bmpfilename, ".bmp");
    FILE* bmpfile = fopen(bmpfilename, "wb");
    char header[54];
    char palette[4 * 256];

    //int datasize = color ? M * ceil(3 * N / 4.0) * 4 : 1;   // !!!!!!! размер пиксельных данных, чтобы размер строки был кратен 4 байтам
    datasize = color ? M * ceil(3 * N / 4.0) * 4 
                     : M * ceil(    N / 4.0) * 4;   // !!!!!!! размер пиксельных данных, чтобы размер строки был кратен 4 байтам
                                                            // исправить для чб случая, сейчас заглушка в виде 1.
    char* data = new char[datasize];

    fillheader(header);         // заполнить заголовки
    fillpalette(palette);       // заполнить палитру (если надо)
    filldata(data, r, g, b);    // заполнить массив пиксельных данных

    fwrite(header, 1, 54, bmpfile);             // записать заголовки
    if (!color) {       
        fwrite(palette, 1, 4 * 256, bmpfile);   // если чб, записать палитру
    }
    fwrite(data, 1, datasize, bmpfile);         // записать пиксельные данные

    fclose(bmpfile);
}
