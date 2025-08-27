#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

struct ImageCompareResult {
    double similarity; // 相似度分数
    bool isSame; // 是否超过阈值
};

std::string base64_decode(const std::string& encoded_string);
cv::Mat base64ToMat(const std::string& base64Data);
cv::Mat extractPath(const cv::Mat& image);
std::vector<cv::Point> extractKeyPoints(const cv::Mat& binaryImage);
std::vector<double> calculateSlopeFeatures(const std::vector<cv::Point>& keyPoints);
std::vector<double> calculateCurvatureFeatures(const std::vector<cv::Point>& keyPoints);
double dtwDistance(const std::vector<double>& seq1, const std::vector<double>& seq2);
double compareFeatures(const std::vector<double>& features1, const std::vector<double>& features2);

ImageCompareResult compareImagesBase64(
    const std::string& base64Img1,
    const std::string& base64Img2,
    bool isSimple = false
);