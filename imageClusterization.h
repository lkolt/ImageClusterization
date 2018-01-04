#pragma once

#ifndef IMGCLUSTER_H
#define IMGCLUSTER_H

#include "opencv2/opencv.hpp"
#include "iostream"
#include <bitset>
#include "Vantage-Point tree.h"

using namespace std;

const int dimension = 16;
const int hashSize = dimension * dimension;
const double MAX_DIST_TO_NEIGHBOR = 85;

struct img {
	string name;
	bitset<hashSize> bits;
	int cluster;

	double dist(const img &y) {			// Hamming distance
		return (bits ^ y.bits).count();
	}
};

bitset<hashSize> calcImageHash(IplImage* src);

double sqr(double a);

int getCluster(vector<img> results, vector<double> distances);

int clusterization(int argc, char* argv[]);

#endif //IMGCLUSTER_H