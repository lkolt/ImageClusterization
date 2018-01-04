#include "opencv2/opencv.hpp"
#include "iostream"
#include <bitset>
#include "Vantage-Point tree.h"

using namespace std;
using namespace cv;

const int dimension = 16;
const int hashSize = dimension * dimension;
const double MAX_DIST_TO_NEIGHBOR = 100;

struct img {
	string name;
	bitset<hashSize> bits;
	int cluster;

	double dist(const img &y) {			// Hamming distance
		return (bits ^ y.bits).count();
	}
};
int count_clusters = 0;

int counter = 0;
vector <img> images;

bitset<hashSize> calcImageHash(IplImage* src){

	IplImage *res = 0, *gray = 0, *bin = 0;

	res = cvCreateImage(cvSize(dimension, dimension), src->depth, src->nChannels);
	gray = cvCreateImage(cvSize(dimension, dimension), IPL_DEPTH_8U, 1);
	bin = cvCreateImage(cvSize(dimension, dimension), IPL_DEPTH_8U, 1);

	// уменьшаем картинку
	cvResize(src, res);
	// переводим в градации серого
	cvCvtColor(res, gray, CV_BGR2GRAY);
	// вычисляем среднее
	CvScalar average = cvAvg(gray);
	printf("[i] average: %.2f \n", average.val[0]);
	// получим бинарное изображение относительно среднего
	// для этого воспользуемся пороговым преобразованием
	cvThreshold(gray, bin, average.val[0], 255, CV_THRESH_BINARY);

	// построим хэш
	bitset<hashSize> result;
	int cnt = 0;
	for (int y = 0; y<bin->height; y++) {
		uchar* ptr = (uchar*)(bin->imageData + y * bin->widthStep);
		for (int x = 0; x<bin->width; x++) {
			result.set(cnt++, ptr[x] ? 1 : 0);
		}
	}

	// освобождаем ресурсы
	cvReleaseImage(&res);
	cvReleaseImage(&gray);
	cvReleaseImage(&bin);

	return result;
}

double sqr(double a) {
	return a * a;
}

int getCluster(vector<img> results, vector<double> distances) {
	pair<double, int> mx = { 0, -1 };

	map<int, double> mp;

	for (int i = 0; i < results.size(); i++) {
		int cluster = results[i].cluster;
		double dist = distances[i];
		if (dist < MAX_DIST_TO_NEIGHBOR) {
			mp[cluster] += 1.0 / sqr(dist);
			mx = max(mx, { mp[cluster], cluster });
		}
	}

	if (mx.second == -1) {
		mx.second = ++count_clusters;
	}
	return mx.second;
}

int main(int argc, char* argv[])
{
	VpTree<img> tree;
	IplImage *object = 0, *image = 0;

	char obj_name[] = "";
		
	int n = argc;

	for (int i = 1; i < n; i++) {

		char* object_filename = argc >= 2 ? argv[i] : obj_name;

		// получаем картинку
		object = cvLoadImage(object_filename, 1);
		printf("[i] object: %s\n", object_filename);
		if (!object) {
			printf("[!] Error: cant load object image: %s\n", object_filename);
			return -1;
		}

		img image;
		image.name = object_filename;
		
		// построим хэш
		bitset<hashSize> hash = calcImageHash(object);
/*		cout << "Hash size: " << hashSize << endl << "Hash: " ;

		for (int j = 0; j < hashSize; j++) {
			cout << hash[j] << " ";
		}
		cout << endl;
		*/
		image.bits = hash;
		images.push_back(image);

		vector<img> results;
		vector<double> distances;
		tree.search(image, 4, &results, &distances);

		cout << "Search for " << image.name << endl;
		for (int i = 0; i < results.size(); i++) {
			cout << ">>Found: " << results[i].name << " " << distances[i] << endl;
		}

		image.cluster = getCluster(results, distances);
		cout << "IMAGE CLUSTER: " << image.cluster << endl;

		tree.insert(image);

		// освобождаем ресурсы
		cvReleaseImage(&object);
	}

	cvWaitKey(0);
	return 0;
}


