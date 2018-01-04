#include "imageClusterization.h"

int count_clusters = 0;
int counter = 0;

bitset<hashSize> calcImageHash(IplImage* src) {
	IplImage *res = 0, *gray = 0, *bin = 0;

	res = cvCreateImage(cvSize(dimension, dimension), src->depth, src->nChannels);
	gray = cvCreateImage(cvSize(dimension, dimension), IPL_DEPTH_8U, 1);
	bin = cvCreateImage(cvSize(dimension, dimension), IPL_DEPTH_8U, 1);

	// ��������� ��������
	cvResize(src, res);
	// ��������� � �������� ������
	cvCvtColor(res, gray, CV_BGR2GRAY);
	// ��������� �������
	CvScalar average = cvAvg(gray);
	//printf("[i] average: %.2f \n", average.val[0]);
	// ������� �������� ����������� ������������ ��������
	// ��� ����� ������������� ��������� ���������������
	cvThreshold(gray, bin, average.val[0], 255, CV_THRESH_BINARY);

	// �������� ���
	bitset<hashSize> result;
	int cnt = 0;
	for (int y = 0; y<bin->height; y++) {
		uchar* ptr = (uchar*)(bin->imageData + y * bin->widthStep);
		for (int x = 0; x<bin->width; x++) {
			result.set(cnt++, ptr[x] ? 1 : 0);
		}
	}

	// ����������� �������
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
		mx.second = count_clusters++;
	}
	return mx.second;
}

int clusterization(int argc, char* argv[]) {

	vector <img> images;
	vector <vector<img>> clusters;
	VpTree<img> tree;
	IplImage *object = 0;

	for (int i = 2; i < argc; i++) {

		char* object_filename = argv[i];

		// �������� ��������
		object = cvLoadImage(object_filename, 1);
		//printf("[i] object: %s\n", object_filename);
		if (!object) {
			printf("[!] Error: cant load object image: %s\n", object_filename);
			return -1;
		}

		img image;
		image.name = object_filename;

		// �������� ���
		bitset<hashSize> hash = calcImageHash(object);

		image.bits = hash;
		images.push_back(image);

		vector<img> results;
		vector<double> distances;
		tree.search(image, 4, &results, &distances);

		image.cluster = getCluster(results, distances);

		if (image.cluster >= clusters.size()) {
			clusters.push_back(vector<img>());
		}
		clusters[image.cluster].push_back(image);

		tree.insert(image);

		// ����������� �������
		cvReleaseImage(&object);
	}

	for (int i = 0; i < clusters.size(); i++) {
		cout << "Cluster #" << i << endl;
		for (int j = 0; j < clusters[i].size(); j++) {
			cout << ">>" << clusters[i][j].name << endl;
		}
	}

	return 0;
}