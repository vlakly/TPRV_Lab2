#include <iostream>
#include <omp.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const int gSize = 3;
double gFilter[gSize][gSize] =
{ 1 / 16.0, 2 / 16.0, 1 / 16.0,
  2 / 16.0, 4 / 16.0, 2 / 16.0,
  1 / 16.0, 2 / 16.0, 1 / 16.0 };

int threadNum = thread::hardware_concurrency();

const int sizeX = 1024;
const int sizeY = 768;
const int channels = 3;

const int m_size = 1024;

void fill_image(int*** m);

void fill_matrix(int matrix[m_size][m_size]);
void scalar_multiplication(int a[m_size][m_size], int b[m_size][m_size], int c[m_size][m_size]);
void compare_matrices(int a[m_size][m_size], int b[m_size][m_size]);
void transpose(int a[m_size][m_size]);

class Timer
{
public:
	void start()
	{
		m_StartTime = chrono::system_clock::now();
		m_bRunning = true;
	}

	void stop()
	{
		m_EndTime = chrono::system_clock::now();
		m_bRunning = false;
	}

	double elapsedMilliseconds()
	{
		chrono::time_point<chrono::system_clock> endTime;

		if (m_bRunning)
		{
			endTime = chrono::system_clock::now();
		}
		else
		{
			endTime = m_EndTime;
		}

		return chrono::duration_cast<chrono::milliseconds>(endTime - m_StartTime).count();
	}

	double elapsedSeconds()
	{
		return elapsedMilliseconds() / 1000.0;
	}

private:
	chrono::time_point<chrono::system_clock> m_StartTime;
	chrono::time_point<chrono::system_clock> m_EndTime;
	bool                                               m_bRunning = false;
};

int main() {
	bool task1(1),
		task2(0);

	// ====== task 1 ======
	if (task1) {
		int i, j, k, x, y;

		cout << "Number of threads: " << threadNum << "\n";

		//int image[sizeX][sizeY][channels];

		int*** image = new int** [sizeX]();
		for (i = 0; i < sizeX; ++i) {
			image[i] = new int* [sizeY]();
			for (j = 0; j < sizeY; ++j) {
				image[i][j] = new int[channels]();
			}
		}

		fill_image(image);

		const int radius = gSize - 1;
		const int intenX = sizeX + radius;
		const int intenY = sizeY + radius;

		int intensity[intenX][intenY]{ 0 };
		int resultImage[sizeX][sizeY]{ 0 };

		//заполнение матрицы интенсивности
		for (i = 1; i < intenX - 1; i++) {
			for (j = 1; j < intenY - 1; j++) {
				for (k = 0; k < channels; k++) {
					intensity[i][j] += image[i - 1][j - 1][k];
				}
				intensity[i][j] /= 3;
			}
		}

		//наложение фильтра
		for (i = 1; i < intenX - 1; i++) {
			for (j = 1; j < intenY - 1; j++) {
				for (x = -1; x < gSize - 1; x++) {
					for (y = -1; y < gSize - 1; y++) {
						resultImage[i - 1][j - 1] += intensity[i + x][j + y] * gFilter[1 + x][1 + y];
					}
				}
			}
		}

		//вывод матрицы с наложением фильтра
		cout << "Filtered matrix\n";
		for (i = 0; i < sizeX; i++) {
			for (j = 0; j < sizeY; j++) {
				cout << resultImage[i][j] << " ";
			}
			cout << "\n";
		}

		string image_path = "D:/ttv/картинки/bob.png";
		Mat img = imread(image_path, IMREAD_COLOR);

		imshow("Display window", img);
		int k = waitKey(0); // Wait for a keystroke in the window
	}
 
	// ====== task 2 ======
	//int m_A[m_size][m_size];
	//int m_B[m_size][m_size];
	//int m_ScalarSingle[m_size][m_size];

	//cout << "Matrix size: " << m_size << endl;
	//cout << "Creating matrices..." << endl;
	//fill_matrix(m_A);
	//fill_matrix(m_B);
	//cout << "Matrices are created" << endl;

	//double timeScalarSingle;

	//bool scalarSingle(0);

	//double imageCount(10.0),
	//	overallTime(0.0);

	//Timer timer;
	//if (scalarSingle) {
	//	cout << "Multiplying matrices (scalar, single thread)..." << endl;
	//	timer.start();
	//	scalar_multiplication(m_A, m_B, m_ScalarSingle);
	//	timer.stop();
	//	cout << "Scalar (single) multiplication is done: " << timer.elapsedSeconds() << " seconds" << endl;
	//	timeScalarSingle = timer.elapsedSeconds();
	//}
}

void fill_image(int*** m) {
	int i, j, k;
	for (i = 0; i < sizeX; i++) {
		for (j = 0; j < sizeY; j++) {
			for (k = 0; k < channels; k++) {
				m[i][j][k] = rand() % 256;
			}
		}
	}
}

void fill_matrix(int matrix[m_size][m_size]) {
	int i, j;
	for (i = 0; i < m_size; i++) {
		for (j = 0; j < m_size; j++) {
			matrix[i][j] = rand() % 5;
		}
	}
}
void scalar_multiplication(int a[m_size][m_size], int b[m_size][m_size], int c[m_size][m_size]) {
	int i, j, k;
	for (i = 0; i < m_size; i++) {
		for (j = 0; j < m_size; j++) {
			for (k = 0; k < m_size; k++) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
		//if (i % 256 == 0) {
		//    cout << "current i (scalar): " << i << endl;
		//}
	}
}
void compare_matrices(int a[m_size][m_size], int b[m_size][m_size]) {
	int i, j;
	int errors = 0;
	for (i = 0; i < m_size; i++) {
		for (j = 0; j < m_size; j++) {
			if (a[i][j] != b[i][j]) {
				errors++;
				//cout << "Elems: " << a[i][j] << " " << b[i][j] << endl;
				//cout << "i = " << i << " ,j = " << j << endl;
			}
		}
	}
	cout << "Error count: " << errors << endl;
}
void transpose(int a[m_size][m_size]) {
	int i, j, t;
	for (i = 0; i < m_size; ++i)
	{
		for (j = i; j < m_size; ++j)
		{
			t = a[i][j];
			a[i][j] = a[j][i];
			a[j][i] = t;
		}
	}
}