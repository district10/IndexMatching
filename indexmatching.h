#ifndef INDEXMATCHING_H
#define INDEXMATCHING_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    double x;
    double z;
} point2;

typedef struct {
    std::vector<point2> pts;
    int64_t timestamp;
} profile_t;

typedef struct {
    int64_t timestamp;
    int motornum;
    double roll;
    double yaw;
    double pitch;
} pose_t;

enum MATCHING_TYPE {
    TYPE_POSE,
    TYPE_PROFILE,
};

typedef struct {
    int64_t timestamp;
    enum MATCHING_TYPE type;
} blend_t;


void loadPoseFile(const char *path, std::vector<pose_t> &poses);
void loadProfileFile(const char *path, std::vector<profile_t> &profiles);

int compare(const void *a, const void *b);
void indexmatching(std::vector<pose_t> &poses, std::vector<profile_t> &profiles,
                   std::vector<int> idx_poses, std::vector<int> &idx_profiles);

#endif
