#include <iostream>
#include <omp.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>

using namespace cv;
using namespace std;

int threadNum = thread::hardware_concurrency();

vector<vector<double>> gFilter{
	{ 1 / 16.0, 2 / 16.0, 1 / 16.0 },
	{ 2 / 16.0, 4 / 16.0, 2 / 16.0 },
	{ 1 / 16.0, 2 / 16.0, 1 / 16.0 } };
int gSize = gFilter.size() - 1;

void process_image(string image_name) {
	Mat input = imread(image_name);
	Mat inputGray;
	cvtColor(input, inputGray, COLOR_BGR2GRAY);
	if (input.empty()) {
		cout << "Image error\n";
	}
	Mat output = Mat::zeros(input.size(), CV_8UC1);
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			uchar buff = 0;
			for (int x = -1; x < gSize; x++) {
				for (int y = -1; y < gSize; y++) {
					if (!(i + x < 0 || j + y < 0 || i + x >(input.rows - 1) || j + y >(input.cols - 1))) {
						Vec3b inputPixel = input.at<Vec3b>(i + x, j + y);
						buff += inputPixel[0] * gFilter.at(x + 1).at(y + 1);
					}
				}
			}
			output.at<uchar>(i, j) = static_cast <uchar>(buff);
		}
	}
	imwrite("blur_" + image_name, output);
}
void process_image_OMP(string image_name) {
	Mat input = imread(image_name);
	Mat inputGray;
	cvtColor(input, inputGray, COLOR_BGR2GRAY);
	if (input.empty()) {
		cout << "Image error\n";
	}
	Mat output = Mat::zeros(input.size(), CV_8UC1);
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < input.rows; i++) {
			for (int j = 0; j < input.cols; j++) {
				uchar buff = 0;
				for (int x = -1; x < gSize; x++) {
					for (int y = -1; y < gSize; y++) {
						if (!(i + x < 0 || j + y < 0 || i + x >(input.rows - 1) || j + y >(input.cols - 1))) {
							Vec3b inputPixel = input.at<Vec3b>(i + x, j + y);
							buff += inputPixel[0] * gFilter.at(x + 1).at(y + 1);
						}
					}
				}
				output.at<uchar>(i, j) = static_cast <uchar>(buff);
			}
		}
	}
	imwrite("blur_" + image_name, output);
}
void print_resolution(string image_name) {
	Mat input = imread(image_name);
	cout << "Picture's resolution: " << input.cols << "x" << input.rows << "\n";
}

uint32_t** create_empty(int size) {
	uint32_t** m = new uint32_t*[size];
	for (int i = 0; i < size; ++i) {
		m[i] = new uint32_t[size];
		for (int j = 0; j < size; ++j) {
			m[i][j] = 0;
		}
	}
	return m;
}
uint32_t** create_random(int size) {
	uint32_t** m = new uint32_t * [size];
	for (int i = 0; i < size; ++i) {
		m[i] = new uint32_t[size];
		for (int j = 0; j < size; ++j) {
			m[i][j] = rand() % 5;
		}
	}
	return m;
}
void print_matrix(uint32_t** m, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			cout << m[i][j] << " ";
		}
		cout << "\n";
	}
	cout << "\n\n";
}
void multiply_matrices(uint32_t** res, uint32_t** mA, uint32_t** mB, int size) {
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			for (int k = 0; k < size; k++) {
				res[i][j] += mA[i][k] * mB[k][j];
			}
		}
	}
}
void multiply_matrices_OMP(uint32_t** res, uint32_t** mA, uint32_t** mB, int size) {
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j) {
				for (int k = 0; k < size; k++) {
					res[i][j] += mA[i][k] * mB[k][j];
				}
			}
		}
	}

}
bool check_equality(uint32_t** mA, uint32_t** mB, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (mA[i][j] != mB[i][j]) {
				return false;
			}
		}
	}
	return true;
}

int main() {
	utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

	#ifdef _OPENMP
		cout << ("OpenMP is supported!\n");
	#endif
	#ifndef _OPENMP
		cout << ("OpenMP is NOT supported!\n");
		return 0;
	#endif
	cout << "Number of threads: " << threadNum << "\n";
	omp_set_num_threads(threadNum);

	bool singleThread(1), multiThread(1);
	bool task1(0), task2(1);

	//Mat input = imread("mountain.png");
	//Mat inputGray;
	//cvtColor(input, inputGray, COLOR_BGR2GRAY);
	//imshow("Input", input);
	//imshow("Output", inputGray);
	//int o = waitKey(0);

	if (task1) {
		int testCount = 10;
		double avgTime = 0;
		vector<string> pictures{ "mountain.png", "city.png", "desert.png"};
		vector<double> timeResult;
		auto iter{ pictures.begin() };
		while (iter != pictures.end()) {
			print_resolution(*iter);
			cout << "Image process test (no OMP)\n";
			for (int test = 0; test < testCount; test++) {
				auto t_start = omp_get_wtime();
				process_image(*iter);
				auto t_end = omp_get_wtime();
				avgTime += t_end - t_start;
				cout << "Attempt " << test + 1 << ", time: " << t_end - t_start << " seconds\n";
			}
			cout << "Average time is " << avgTime / testCount << " seconds\n\n";
			timeResult.push_back(avgTime / testCount);
			avgTime = 0;

			cout << "Image process test (with OMP)\n";
			for (int test = 0; test < testCount; test++) {
				auto t_start = omp_get_wtime();
				process_image_OMP(*iter);
				auto t_end = omp_get_wtime();
				avgTime += t_end - t_start;
				cout << "Attempt " << test + 1 << ", time: " << t_end - t_start << " seconds\n";
			}
			cout << "Average time is " << avgTime / testCount << " seconds\n\n";
			timeResult.push_back(avgTime / testCount);
			avgTime = 0;

			++iter;
		}
		cout << "Results table\n";
		for (int i = 0; i < timeResult.size() / 2; i++) {
			cout << "Picture #" << i + 1 << ": " << timeResult.at(2 * i) << " | " << timeResult.at(2 * i + 1) << " | " << timeResult.at(2 * i) / timeResult.at(2 * i + 1) << "\n";
		}
	}

	if (task2) {
		int m_size = 4096;
		cout << "Matrix size: " << m_size << "x" << m_size << "\n";
		int testCount = 10;
		double avgTime = 0;
		uint32_t** no_thr_res = create_empty(m_size);
		uint32_t** multi_thr_res = create_empty(m_size);
		vector<double> timeResult;
		uint32_t** mA = create_random(m_size);
		uint32_t** mB = create_random(m_size);

		cout << "Matrix multiplication test (no OMP)\n";
		for (int test = 0; test < testCount; test++) {
			uint32_t** mRes = create_empty(m_size);
			auto t_start = omp_get_wtime();
			multiply_matrices(mRes, mA, mB, m_size);
			auto t_end = omp_get_wtime();
			avgTime += t_end - t_start;
			cout << "Attempt " << test + 1 << ", time: " << t_end - t_start << " seconds\n";
			if (test == testCount - 1) {
				no_thr_res = mRes;
			}
		}
		cout << "Average time is " << avgTime / testCount << " seconds\n\n";
		timeResult.push_back(avgTime / testCount);
		avgTime = 0;

		cout << "Matrix multiplication test (with OMP)\n";
		for (int test = 0; test < testCount; test++) {
			uint32_t** mRes = create_empty(m_size);
			auto t_start = omp_get_wtime();
			multiply_matrices_OMP(mRes, mA, mB, m_size);
			auto t_end = omp_get_wtime();
			avgTime += t_end - t_start;
			cout << "Attempt " << test + 1 << ", time: " << t_end - t_start << " seconds\n";
			if (test == testCount - 1) {
				multi_thr_res = mRes;
			}
		}
		cout << "Average time is " << avgTime / testCount << " seconds\n\n";
		timeResult.push_back(avgTime / testCount);
		avgTime = 0;

		//print_matrix(no_thr_res, m_size);
		//print_matrix(multi_thr_res, m_size);

		bool are_equal = check_equality(no_thr_res, multi_thr_res, m_size);
		if (are_equal) { cout << "Matrices are equal\n"; }
		else { cout << "Matrices are NOT equal\n"; }

		cout << "Results table\n";
		for (int i = 0; i < timeResult.size() / 2; i++) {
			cout << "Multiplication #" << i + 1 << ": " << timeResult.at(2 * i) << " | " << timeResult.at(2 * i + 1) << " | " << timeResult.at(2 * i) / timeResult.at(2 * i + 1) << "\n";
		}
	}
}