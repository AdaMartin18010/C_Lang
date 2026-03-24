
---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../readme.md](../../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

### 【阶段层】学习定位

**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链

| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/readme.md](../../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

### 【总体层】知识体系架构

```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择

| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/readme.md) |

---

# 机器人计算机视觉 (Computer Vision for Robotics)

> **层级定位**: 18_Robotics_Physical_Control > 07_Computer_Vision_Robotics
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 线性代数、图像处理、C++、相机模型
> **参考标准**: OpenCV, PCL, ROS2 Vision

---

## 内容

- [OpenCV C接口](./01_OpenCV_C_Interface.md)
- [相机标定与校正](./02_Camera_Calibration.md)
- [目标检测与跟踪](./03_Object_Detection_Tracking.md)
- [视觉里程计与SLAM](./04_Visual_Odometry_SLAM.md)
- [视觉伺服控制](./05_Visual_Servoing.md)

---

## 相机模型与标定

### 针孔相机模型

```
世界坐标系 (Xw, Yw, Zw)
        ↓ [R|t] (外参)
相机坐标系 (Xc, Yc, Zc)
        ↓ 投影
图像平面 (u, v)
        ↓ K (内参)
像素坐标 (px, py)
```

### OpenCV相机标定 (C接口)

```c
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>

// 相机标定参数结构体
typedef struct {
    double camera_matrix[3][3];      // 内参矩阵 K
    double dist_coeffs[5];            // 畸变系数 [k1, k2, p1, p2, k3]
    double rotation_vecs[100][3];     // 旋转向量 (每幅图像)
    double translation_vecs[100][3];  // 平移向量
    double reprojection_error;        // 重投影误差
} CameraCalibration;

// 使用棋盘格标定
int calibrate_camera(const char** image_paths, int num_images,
                     int board_width, int board_height, float square_size,
                     CameraCalibration* result) {

    std::vector<std::vector<cv::Point3f>> object_points;
    std::vector<std::vector<cv::Point2f>> image_points;

    // 准备世界坐标系下的角点坐标
    std::vector<cv::Point3f> obj;
    for (int i = 0; i < board_height; i++) {
        for (int j = 0; j < board_width; j++) {
            obj.push_back(cv::Point3f(j * square_size, i * square_size, 0));
        }
    }

    cv::Size image_size;

    // 遍历所有标定图像
    for (int i = 0; i < num_images; i++) {
        cv::Mat image = cv::imread(image_paths[i], cv::IMREAD_GRAYSCALE);
        if (image.empty()) continue;

        image_size = image.size();

        // 检测棋盘格角点
        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(image,
                                               cv::Size(board_width, board_height),
                                               corners,
                                               cv::CALIB_CB_ADAPTIVE_THRESH +
                                               cv::CALIB_CB_NORMALIZE_IMAGE);

        if (found) {
            // 亚像素精确化
            cv::cornerSubPix(image, corners, cv::Size(11, 11), cv::Size(-1, -1),
                            cv::TermCriteria(cv::TermCriteria::EPS +
                                            cv::TermCriteria::COUNT, 30, 0.1));

            image_points.push_back(corners);
            object_points.push_back(obj);
        }
    }

    // 执行标定
    cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    cv::Mat dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);
    std::vector<cv::Mat> rvecs, tvecs;

    double rms = cv::calibrateCamera(object_points, image_points, image_size,
                                     camera_matrix, dist_coeffs, rvecs, tvecs,
                                     cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);

    // 复制结果
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result->camera_matrix[i][j] = camera_matrix.at<double>(i, j);
        }
    }

    for (int i = 0; i < 5; i++) {
        result->dist_coeffs[i] = dist_coeffs.at<double>(i);
    }

    result->reprojection_error = rms;

    return 0;
}

// 图像去畸变
void undistort_image(const uint8_t* input, uint8_t* output,
                     int width, int height,
                     const CameraCalibration* calib) {
    cv::Mat src(height, width, CV_8UC1, (void*)input);
    cv::Mat dst;

    cv::Mat camera_matrix(3, 3, CV_64F, (void*)calib->camera_matrix);
    cv::Mat dist_coeffs(5, 1, CV_64F, (void*)calib->dist_coeffs);

    cv::undistort(src, dst, camera_matrix, dist_coeffs);

    memcpy(output, dst.data, width * height);
}
```

---

## 深入理解

### 1. 视觉里程计 (Visual Odometry)

基于图像序列估计相机运动。

```c
// 特征点提取与匹配
typedef struct {
    float x, y;      // 像素坐标
    float descriptor[128];  // SIFT/SURF描述子
} FeaturePoint;

// 光流法跟踪 (Lucas-Kanade)
void optical_flow_tracking(const uint8_t* prev_img, const uint8_t* curr_img,
                           int width, int height,
                           FeaturePoint* prev_points, int num_points,
                           FeaturePoint* curr_points, int* status) {

    cv::Mat prev_gray(height, width, CV_8UC1, (void*)prev_img);
    cv::Mat curr_gray(height, width, CV_8UC1, (void*)curr_img);

    std::vector<cv::Point2f> prev_pts, curr_pts;
    for (int i = 0; i < num_points; i++) {
        prev_pts.push_back(cv::Point2f(prev_points[i].x, prev_points[i].y));
    }

    std::vector<uchar> track_status;
    std::vector<float> err;

    // LK光流计算
    cv::calcOpticalFlowPyrLK(prev_gray, curr_gray, prev_pts, curr_pts,
                             track_status, err, cv::Size(21, 21), 3,
                             cv::TermCriteria(cv::TermCriteria::COUNT +
                                             cv::TermCriteria::EPS, 30, 0.01));

    // 复制结果
    for (int i = 0; i < num_points; i++) {
        curr_points[i].x = curr_pts[i].x;
        curr_points[i].y = curr_pts[i].y;
        status[i] = track_status[i];
    }
}

// 本质矩阵估计与运动恢复
void estimate_motion(const FeaturePoint* points1, const FeaturePoint* points2,
                     int num_matches, const double K[3][3],
                     double R[3][3], double t[3]) {

    std::vector<cv::Point2f> pts1, pts2;
    for (int i = 0; i < num_matches; i++) {
        pts1.push_back(cv::Point2f(points1[i].x, points1[i].y));
        pts2.push_back(cv::Point2f(points2[i].x, points2[i].y));
    }

    cv::Mat camera_matrix(3, 3, CV_64F, (void*)K);

    // 计算本质矩阵
    cv::Mat E = cv::findEssentialMat(pts1, pts2, camera_matrix,
                                     cv::RANSAC, 0.999, 1.0);

    // 从本质矩阵恢复R和t
    cv::Mat R_mat, t_vec;
    cv::recoverPose(E, pts1, pts2, camera_matrix, R_mat, t_vec);

    // 复制结果
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] = R_mat.at<double>(i, j);
        }
        t[i] = t_vec.at<double>(i);
    }
}
```

### 2. 目标检测 (YOLO/SSD)

```c
// OpenCV DNN模块加载YOLO模型
typedef struct {
    cv::dnn::Net net;
    int input_width;
    int input_height;
    float confidence_threshold;
    float nms_threshold;
    std::vector<std::string> class_names;
} YOLODetector;

// 初始化YOLO检测器
int yolo_init(YOLODetector* detector, const char* model_cfg,
              const char* model_weights, const char* class_names_file) {

    detector->net = cv::dnn::readNetFromDarknet(model_cfg, model_weights);
    detector->net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    detector->net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    detector->input_width = 416;
    detector->input_height = 416;
    detector->confidence_threshold = 0.5;
    detector->nms_threshold = 0.4;

    // 加载类别名称
    std::ifstream ifs(class_names_file);
    std::string line;
    while (std::getline(ifs, line)) {
        detector->class_names.push_back(line);
    }

    return 0;
}

// 执行目标检测
void yolo_detect(YOLODetector* detector, const uint8_t* image_data,
                 int width, int height, int channels,
                 struct Detection* detections, int* num_detections, int max_detections) {

    cv::Mat frame(height, width, (channels == 3) ? CV_8UC3 : CV_8UC1,
                  (void*)image_data);

    // 预处理
    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0/255.0,
                                          cv::Size(detector->input_width,
                                                  detector->input_height),
                                          cv::Scalar(0, 0, 0), true, false);

    detector->net.setInput(blob);

    // 前向传播
    std::vector<cv::Mat> outputs;
    detector->net.forward(outputs, detector->net.getUnconnectedOutLayersNames());

    // 解析检测结果
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<int> class_ids;

    for (size_t i = 0; i < outputs.size(); ++i) {
        float* data = (float*)outputs[i].data;
        for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
            cv::Mat scores = outputs[i].row(j).colRange(5, outputs[i].cols);
            cv::Point class_id_point;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &class_id_point);

            if (confidence > detector->confidence_threshold) {
                int center_x = (int)(data[0] * width);
                int center_y = (int)(data[1] * height);
                int box_width = (int)(data[2] * width);
                int box_height = (int)(data[3] * height);
                int left = center_x - box_width / 2;
                int top = center_y - box_height / 2;

                class_ids.push_back(class_id_point.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, box_width, box_height));
            }
        }
    }

    // 非极大值抑制 (NMS)
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, detector->confidence_threshold,
                      detector->nms_threshold, indices);

    // 复制结果
    *num_detections = std::min((int)indices.size(), max_detections);
    for (int i = 0; i < *num_detections; i++) {
        int idx = indices[i];
        detections[i].x = boxes[idx].x;
        detections[i].y = boxes[idx].y;
        detections[i].width = boxes[idx].width;
        detections[i].height = boxes[idx].height;
        detections[i].confidence = confidences[idx];
        detections[i].class_id = class_ids[idx];
    }
}
```

### 3. 视觉伺服 (Visual Servoing)

```c
// 基于图像的视觉伺服 (IBVS)

// 计算图像雅可比矩阵 (交互矩阵)
void image_jacobian(double u, double v, double Z, double J[2][6]) {
    // 归一化坐标
    double x = u;
    double y = v;

    // 交互矩阵 L
    J[0][0] = -1/Z;  J[0][1] =  0;    J[0][2] =  x/Z;
    J[0][3] =  x*y;  J[0][4] = -(1+x*x); J[0][5] =  y;

    J[1][0] =  0;    J[1][1] = -1/Z;  J[1][2] =  y/Z;
    J[1][3] =  1+y*y; J[1][4] = -x*y;  J[1][5] = -x;
}

// 视觉伺服控制律
void visual_servo_control(const double* current_features,    // 当前图像特征 [u1,v1, u2,v2, ...]
                          const double* desired_features,     // 期望图像特征
                          int num_features,
                          const double* depths,               // 特征点深度
                          double lambda,                       // 控制增益
                          double* camera_velocity) {          // 输出相机速度 [vx,vy,vz,wx,wy,wz]

    // 构建雅可比矩阵
    int n = num_features * 2;
    cv::Mat L(n, 6, CV_64F);

    for (int i = 0; i < num_features; i++) {
        double J[2][6];
        image_jacobian(current_features[i*2], current_features[i*2+1],
                       depths[i], J);

        for (int j = 0; j < 6; j++) {
            L.at<double>(i*2, j) = J[0][j];
            L.at<double>(i*2+1, j) = J[1][j];
        }
    }

    // 计算特征误差
    cv::Mat e(n, 1, CV_64F);
    for (int i = 0; i < n; i++) {
        e.at<double>(i) = current_features[i] - desired_features[i];
    }

    // 计算伪逆: L^+ = (L^T * L)^-1 * L^T
    cv::Mat L_pinv;
    cv::invert(L, L_pinv, cv::DECOMP_SVD);

    // 控制律: v = -lambda * L^+ * e
    cv::Mat v = -lambda * L_pinv * e;

    for (int i = 0; i < 6; i++) {
        camera_velocity[i] = v.at<double>(i);
    }
}

// 应用于机械臂的完整视觉伺服循环
void visual_servo_arm_loop(YOLODetector* detector,
                           const double target_position[2],  // 期望目标在图像中的位置
                           const double K[3][3],             // 相机内参
                           double* joint_velocities) {

    // 1. 获取当前图像
    cv::Mat frame = capture_frame();

    // 2. 检测目标
    Detection det;
    int num_det;
    yolo_detect(detector, frame.data, frame.cols, frame.rows, 3,
                &det, &num_det, 1);

    if (num_det == 0) return;

    // 3. 计算目标中心
    double current_feature[2] = {
        det.x + det.width / 2.0,
        det.y + det.height / 2.0
    };

    // 4. 估计深度 (使用已知目标尺寸或深度相机)
    double estimated_depth = 0.5;  // 假设深度 0.5m

    // 5. 视觉伺服计算
    double camera_vel[6];
    visual_servo_control(current_feature, target_position, 1, &estimated_depth,
                         0.5, camera_vel);

    // 6. 转换到关节速度 (使用机械臂雅可比)
    // joint_vel = J_arm^-1 * camera_vel
    map_camera_vel_to_joint_vel(camera_vel, joint_velocities);
}
```

---

## 最佳实践

1. **相机标定**: 使用20+图像，多角度覆盖
2. **特征选择**: 光照变化用SIFT/SURF，实时用ORB
3. **深度估计**: 结合深度相机或双目立体视觉
4. **实时优化**: 使用CUDA/OpenCL加速

---

## 权威参考

- [1] OpenCV Documentation: <https://docs.opencv.org/>
- [2] "Multiple View Geometry" by Hartley & Zisserman
- [3] "Computer Vision: Algorithms and Applications" by Szeliski
- [4] ROS2 Vision: <https://github.com/ros-perception>

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
