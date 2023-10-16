#include <iostream>
#include <omp.h>

using namespace std;

const int gSize = 3;
double gFilter[gSize][gSize] =
{ 1 / 16.0, 2 / 16.0, 1 / 16.0,
  2 / 16.0, 4 / 16.0, 2 / 16.0,
  1 / 16.0, 2 / 16.0, 1 / 16.0 };

int first[1024][768][3];
int second[1280][960][3];
int third[2048][1536][3];

int main() {
	cout << "Privet\n";

	int i, j, k, x, y;
	int buffer = 0;

	const int sizeX = 4;
	const int sizeY = 4;
	const int channels = 3;

	const int radius = gSize - 1;
	const int intenX = sizeX + radius;
	const int intenY = sizeY + radius;

	int test[sizeX][sizeY][channels]{ 0 };
	int intensity[intenX][intenY]{ 0 };
	
	for (i = 0; i < sizeX; i++) {
		for (j = 0; j < sizeY; j++) {
			for (k = 0; k < channels; k++) {
				test[i][j][k] = (sizeX) * (sizeX - 1) * i + (sizeY - 1) * j + k;
			}
		}
	}

	//вывод сгенерированной матрицы
	cout << "Generated matrix\n";
	for (i = 0; i < sizeX; i++) {
		for (j = 0; j < sizeY; j++) {
			for (k = 0; k < channels; k++) {
				cout << test[i][j][k] << " ";
			}
			cout << "| ";
		}
		cout << "\n";
	}

	for (i = 1; i < intenX - 1; i++) {
		for (j = 1; j < intenY - 1; j++) {
			for (k = 0; k < channels; k++) {
				intensity[i][j] += test[i - 1][j - 1][k];
			}
			intensity[i][j] /= 3;
		}
	}
	
	//вывод матрицы интенсивности
	cout << "Intensity matrx\n";
	for (i = 0; i < intenX; i++) {
		for (j = 0; j < intenY; j++) {
			cout << intensity[i][j] << " ";
		}
		cout << "\n";
	}

	for (i = 1; i < intenX - 1; i++) {
		for (j = 1; j < intenY - 1; j++) {
			for (x = -1; x < gSize - 1; x++) {
				for (y = -1; y < gSize - 1; y++) {
					buffer += intensity[i + x][j + y] * gFilter[1 + x][1 + y];
				}
			}	
			intensity[i][j] = buffer;
			buffer = 0;
		}
	}

	//вывод матрицы с наложением фильтра
	cout << "Filtered matrix\n";
	for (i = 0; i < intenX; i++) {
		for (j = 0; j < intenY; j++) {
			cout << intensity[i][j] << " ";
		}
		cout << "\n";
	}

	//for (i = 0; i < sizeX; i++) {
	//	for (j = 0; j < sizeY; j++) {
	//		for (k = 0; k < channels; k++) {

	//		}
	//	}
	//}
}