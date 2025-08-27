#include "verifyImageComparator.h"
#include <drogon/drogon.h>

std::string base64_decode(const std::string& encoded_string) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    auto is_base64 = [](unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
        };

    int in_len = encoded_string.size();
    int i = 0, j = 0, in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

cv::Mat base64ToMat(const std::string& base64Data) {
    using namespace cv;
    using namespace std;
    
    // 移除可能的数据URI前缀
    string base64 = base64Data;
    size_t pos = base64.find(",");
    if (pos != string::npos) {
        base64 = base64.substr(pos + 1);
    }
    
    // Base64解码
    string decodedString = base64_decode(base64);
    vector<uchar> data(decodedString.begin(), decodedString.end());
    
    // 解码图像
    Mat image = imdecode(data, IMREAD_UNCHANGED);
    return image;
}

// 提取黑色路径并二值化
cv::Mat extractPath(const cv::Mat& image) {
    using namespace cv;
    
    // 分离通道
    std::vector<Mat> channels;
    split(image, channels);
    
    // 获取Alpha通道（透明度）
    Mat alpha = channels[3];
    
    // 创建只包含黑色路径的二值图像
    Mat binary = Mat::zeros(image.size(), CV_8UC1);
    
    // 遍历每个像素，找到黑色且不透明的路径
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            Vec4b pixel = image.at<Vec4b>(y, x);
            // 如果是黑色且不透明
            if (pixel[0] < 50 && pixel[1] < 50 && pixel[2] < 50 && pixel[3] > 200) {
                binary.at<uchar>(y, x) = 255;
            }
        }
    }
    
    // 形态学操作去除噪点
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(binary, binary, MORPH_CLOSE, kernel);
    
    return binary;
}

// 提取轨迹的关键点
std::vector<cv::Point> extractKeyPoints(const cv::Mat& binaryImage) {
    using namespace cv;
    
    // 查找轮廓
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(binaryImage, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    // 如果没有找到轮廓，返回空向量
    if (contours.empty()) {
        return std::vector<Point>();
    }
    
    // 找到最长的轮廓（主要路径）
    int maxIndex = 0;
    double maxLength = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double length = arcLength(contours[i], false);
        if (length > maxLength) {
            maxLength = length;
            maxIndex = i;
        }
    }
    
    // 对轮廓进行重采样，提取关键点
    std::vector<Point> keyPoints;
    std::vector<Point> contour = contours[maxIndex];
    
    // 计算轮廓总长度
    double totalLength = 0;
    for (size_t i = 1; i < contour.size(); i++) {
        totalLength += norm(contour[i] - contour[i-1]);
    }
    
    // 均匀采样关键点
    int numPoints = 20; // 采样20个关键点
    double step = totalLength / (numPoints - 1);
    double accumulated = 0;
    
    keyPoints.push_back(contour[0]);
    size_t currentIndex = 1;
    
    for (int i = 1; i < numPoints - 1; i++) {
        double target = i * step;
        
        while (accumulated < target && currentIndex < contour.size()) {
            accumulated += norm(contour[currentIndex] - contour[currentIndex-1]);
            currentIndex++;
        }
        
        if (currentIndex < contour.size()) {
            keyPoints.push_back(contour[currentIndex-1]);
        }
    }
    
    keyPoints.push_back(contour.back());
    
    return keyPoints;
}

// 计算关键点的切线角度特征
std::vector<double> calculateSlopeFeatures(const std::vector<cv::Point>& keyPoints) {
    std::vector<double> slopes;
    
    if (keyPoints.size() < 2) {
        return slopes;
    }
    
    // 计算每个关键点处的切线角度（使用前后点计算方向）
    for (size_t i = 1; i < keyPoints.size() - 1; i++) {
        cv::Point prev = keyPoints[i-1];
        cv::Point curr = keyPoints[i];
        cv::Point next = keyPoints[i+1];
        
        // 计算前后向量
        cv::Point2f vec1(curr.x - prev.x, curr.y - prev.y);
        cv::Point2f vec2(next.x - curr.x, next.y - curr.y);
        
        // 计算角度（弧度）
        double angle1 = atan2(vec1.y, vec1.x);
        double angle2 = atan2(vec2.y, vec2.x);
        
        // 平均角度作为该点的切线角度
        double avgAngle = (angle1 + angle2) / 2.0;
        slopes.push_back(avgAngle);
    }
    
    return slopes;
}

// 计算曲率特征
std::vector<double> calculateCurvatureFeatures(const std::vector<cv::Point>& keyPoints) {
    std::vector<double> curvatures;
    
    if (keyPoints.size() < 3) {
        return curvatures;
    }
    
    for (size_t i = 1; i < keyPoints.size() - 1; i++) {
        cv::Point prev = keyPoints[i-1];
        cv::Point curr = keyPoints[i];
        cv::Point next = keyPoints[i+1];
        
        // 计算向量
        cv::Point2f vec1(curr.x - prev.x, curr.y - prev.y);
        cv::Point2f vec2(next.x - curr.x, next.y - curr.y);
        
        // 计算角度变化
        double angle1 = atan2(vec1.y, vec1.x);
        double angle2 = atan2(vec2.y, vec2.x);
        
        // 角度差（曲率）
        double angleDiff = angle2 - angle1;
        
        // 确保角度差在[-π, π]范围内
        if (angleDiff > CV_PI) angleDiff -= 2 * CV_PI;
        if (angleDiff < -CV_PI) angleDiff += 2 * CV_PI;
        
        curvatures.push_back(angleDiff);
    }
    
    return curvatures;
}

// 动态时间规整(DTW)距离计算
double dtwDistance(const std::vector<double>& seq1, const std::vector<double>& seq2) {
    size_t n = seq1.size();
    size_t m = seq2.size();
    
    if (n == 0 || m == 0) {
        return 1.0; // 最大距离
    }
    
    // 创建DTW矩阵
    std::vector<std::vector<double>> dtw(n, std::vector<double>(m, 0));
    
    // 初始化第一行和第一列
    dtw[0][0] = std::abs(seq1[0] - seq2[0]);
    
    for (size_t i = 1; i < n; i++) {
        dtw[i][0] = dtw[i-1][0] + std::abs(seq1[i] - seq2[0]);
    }
    
    for (size_t j = 1; j < m; j++) {
        dtw[0][j] = dtw[0][j-1] + std::abs(seq1[0] - seq2[j]);
    }
    
    // 填充DTW矩阵
    for (size_t i = 1; i < n; i++) {
        for (size_t j = 1; j < m; j++) {
            double cost = std::abs(seq1[i] - seq2[j]);
            dtw[i][j] = cost + std::min({dtw[i-1][j], dtw[i][j-1], dtw[i-1][j-1]});
        }
    }
    
    return dtw[n-1][m-1];
}

// 计算特征序列的相似度
double compareFeatures(const std::vector<double>& features1, const std::vector<double>& features2) {
    if (features1.empty() || features2.empty()) {
        return 0.0;
    }
    
    // 使用DTW计算距离
    double distance = dtwDistance(features1, features2);
    
    // 将距离转换为相似度（0-1）
    double maxPossibleDistance = features1.size() * CV_PI; // 最大可能距离
    double similarity = 1.0 - (distance / maxPossibleDistance);
    
    return std::max(0.0, std::min(1.0, similarity));
}

ImageCompareResult compareImagesBase64(
    const std::string& base64Img1,
    const std::string& base64Img2,
    bool isSimple
) {
    using namespace cv;

    // 获取阈值
    auto verifyImageCompareThreshold = drogon::app().getCustomConfig()["verifyImageCompareThreshold"].asDouble();
    auto verifyImageSimpleCompareThreshold = drogon::app().getCustomConfig()["verifyImageSimpleCompareThreshold"].asDouble();
    double threshold = isSimple ? verifyImageSimpleCompareThreshold : verifyImageCompareThreshold;

    // 解码图像
    Mat img1 = base64ToMat(base64Img1);
    Mat img2 = base64ToMat(base64Img2);

    if (img1.size() != img2.size()) {
        double area1 = img1.rows * img1.cols;
        double area2 = img2.rows * img2.cols;
        double sizeDiff = std::abs(area1 - area2) / std::max(area1, area2);
        // 根据尺寸差异调整阈值，最多升高10%
        threshold *= (1.0 + std::min(sizeDiff, 0.1));
    }
        
    // 提取路径
    Mat path1 = extractPath(img1);
    Mat path2 = extractPath(img2);
    
    // 提取关键点
    std::vector<Point> keyPoints1 = extractKeyPoints(path1);
    std::vector<Point> keyPoints2 = extractKeyPoints(path2);
    
    // 如果关键点数量太少，返回低相似度
    if (keyPoints1.size() < 10 || keyPoints2.size() < 10) {
        return {0.1, false};
    }
    
    // 计算斜率特征
    std::vector<double> slopes1 = calculateSlopeFeatures(keyPoints1);
    std::vector<double> slopes2 = calculateSlopeFeatures(keyPoints2);
    
    // 计算曲率特征
    std::vector<double> curvatures1 = calculateCurvatureFeatures(keyPoints1);
    std::vector<double> curvatures2 = calculateCurvatureFeatures(keyPoints2);
    
    // 计算特征相似度
    double slopeSimilarity = compareFeatures(slopes1, slopes2);
    double curvatureSimilarity = compareFeatures(curvatures1, curvatures2);
    
    // 综合相似度
    double overallSimilarity = (slopeSimilarity * 0.6 + curvatureSimilarity * 0.4);
    
    // 判断是否超过阈值
    bool isSame = overallSimilarity >= threshold;
    
    return {overallSimilarity, isSame};
}
