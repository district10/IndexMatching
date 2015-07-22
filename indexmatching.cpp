#include "indexmatching.h"

void loadPoseFile(const char *path, std::vector<pose_t> &poses)
{
    if (poses.size() != 0) {
        return;
    }

    FILE *fp = fopen(path, "r");
    if (NULL == fp) {
        return;
    }

    // 1437363741845     -2488   2.30000   0.20000   0.02000   0.00000   0.00000   0.00000   0.00000   0.00000   0.00000   0.00000   0.00000   0.00000
    int64_t ts;
    int motornum;
    double roll, yaw, pitch;
    while ( 5 == fscanf( fp,
                    "%lld %d %lf %lf %lf"
                    "     %*lf %*lf %*lf"
                    "     %*lf %*lf %*lf"
                    "     %*lf %*lf %*lf\n",
                    &ts, &motornum, &roll, &yaw, &pitch)) {

        pose_t pose;
        pose.timestamp = ts;
        pose.motornum = motornum;
        pose.roll = roll;
        pose.yaw = yaw;
        pose.pitch = pitch;

        poses.push_back(pose);
    }

    fprintf(stdout, "\nLoaded %d poses: \n", poses.size());
    for (int i = 0; i < poses.size(); ++i) {
        printf("%4i: %lld %4d %10.4lf %10.4lf %10.4lf\n", i,
               poses.at(i).timestamp,
               poses.at(i).motornum,
               poses.at(i).roll,
               poses.at(i).pitch,
               poses.at(i).yaw );
    }
}


void loadProfileFile(const char *path, std::vector<profile_t> &profiles)
{
    if (profiles.size() != 0) {
        return;
    }

    FILE *fp = fopen(path, "r");
    if (NULL == fp) {
        return;
    }

    //      -119.5010460205        1437363741843       119.5010460205
    int64_t ts = 0;
    int64_t ts0 = 0;
    double x, z;
    while ( 3 == fscanf( fp, "%lf %lld %lf", &x, &ts, &z ) ) {
        point2 pt;
        pt.x = x;
        pt.z = z;

        if (ts == ts0) {                                         // old profile
            profiles[profiles.size()-1].pts.push_back(pt);
        } else {                                                 // new profile
            profile_t p;
            p.timestamp = ts;
            p.pts.push_back(pt);
            profiles.push_back(p);
        }

        ts0 = ts;
    }

    fprintf(stdout, "\nLoaded %d profiles: \n", profiles.size());
    for (int i = 0; i < profiles.size(); ++i) {
        printf("%4i: %lld\n", i, profiles.at(i).timestamp);
    }
}


int compare(const void *a, const void *b)
{
    blend_t *aa = (blend_t *)a;
    blend_t *bb = (blend_t *)b;
    return aa->timestamp - bb->timestamp;
}


void indexmatching(std::vector<pose_t> &poses, std::vector<profile_t> &profiles, std::vector<int> idx_poses, std::vector<int> &idx_profiles)
{
    if (poses.size() == 0 || profiles.size() == 0 || idx_poses.size() != 0 || idx_profiles.size() != 0) {
        return;
    }

    int b = 0, b0 = 0, b1 = 0;
    std::vector<blend_t> blends;

    // profiles
    for (int i = 0; i < profiles.size(); ++i) {
        blend_t b;
        b.timestamp = profiles.at(i).timestamp;
        b.type = TYPE_PROFILE;
        blends.push_back(b);
    }

    // poses
    for (int i = 0; i < poses.size(); ++i) {
        blend_t b;
        b.timestamp = poses.at(i).timestamp;
        b.type = TYPE_POSE;
        blends.push_back(b);
    }

    // sort
    qsort(&blends[0], blends.size(), sizeof(blends[0]), compare);

    // print out blends
    fputs("\nBLENDS:\n", stdout);
    for (int i = 0; i < blends.size(); ++i) {
        fprintf( stdout, "%4i: %lld %s\n",
                 i,
                 blends.at(i).timestamp,
                 blends.at(i).type == TYPE_POSE
                 ? "POSE."
                 : "PROFILE." );
    }

    // find nearest matching profile to each pose
    b = 0;
    b0 = -1;
    b1 = -1;
    for (int i = 0; i < poses.size(); ++i) {

        // previous
        while (b < blends.size() && blends.at(b).type != TYPE_POSE) {
            b0 = b;                             // the previous profile
            ++b;
            b1 = b;
        }

        // init
        if (b0 == -1) { b0 = b; }
        if (b1 == -1) { b1 = b; }

        // next
        while (b1 < blends.size() && blends.at(b1).type != TYPE_PROFILE) {
            ++b1;
        }

        // adjustment
        if (b1 >= blends.size()) {
            b1 = blends.size() - 1;
        }

        if (b0 == b) {                          // no PREV profile
            idx_poses.push_back(b1);
        } else if (b1 == b) {                   // no NEXT profile
            idx_poses.push_back(b0);
        } else {                                // PREV or NEXT
            int dt_prev = blends.at(b).timestamp - blends.at(b0).timestamp;
            int dt_next = blends.at(b1).timestamp - blends.at(b).timestamp;
            idx_poses.push_back( dt_prev < dt_next
                                 ? b0
                                 : b1 );
        }

        // GOTO next BLEND for POSE
        ++b;
    }

    // find nearest matching pose to each profile
    b = 0;
    b0 = -1;
    b1 = -1;
    for (int i = 0; i < profiles.size(); ++i) {

        // previous
        while (b < blends.size() && blends.at(b).type != TYPE_PROFILE) {
            b0 = b;                             // the previous profile
            ++b;
            b1 = b;
        }

        // init
        if (b0 == -1) { b0 = b; }
        if (b1 == -1) { b1 = b; }

        // next
        while (b1 < blends.size() && blends.at(b1).type != TYPE_POSE) {
            ++b1;
        }

        // adjustment
        if (b1 >= blends.size()) {
            b1 = blends.size() - 1;
        }

        if (b0 == b) {                          // no PREV profile
            idx_profiles.push_back(b1);
        } else if (b1 == b) {                   // no NEXT profile
            idx_profiles.push_back(b0);
        } else {                                // PREV or NEXT
            int dt_prev = blends.at(b).timestamp - blends.at(b0).timestamp;
            int dt_next = blends.at(b1).timestamp - blends.at(b).timestamp;
            idx_profiles.push_back( dt_prev < dt_next
                                 ? b0
                                 : b1 );
        }

        // GOTO next BLEND for PROFILE
        ++b;
    }

    fputs("\nPOSES:\n", stdout);
    for (int i = 0; i < idx_poses.size(); ++i) {
        fprintf(stdout, "%4i: %5d ", i, idx_poses.at(i));
        int idx = 0;
        int hit = 0;
        while (idx < idx_poses.at(i)) {
            if (blends.at(idx).type == TYPE_PROFILE) {
                ++hit;
            }
            ++idx;
        }
        idx_poses[i] = hit;
        fprintf(stdout, "%4d\n", idx_poses.at(i));
    }

    fputs("\nPROFILES:\n", stdout);
    for (int i = 0; i < idx_profiles.size(); ++i) {
        fprintf(stdout, "%4i: %5d ", i, idx_profiles.at(i));
        int idx = 0;
        int hit = 0;
        while (idx < idx_profiles.at(i)) {
            if (blends.at(idx).type == TYPE_POSE) {
                ++hit;
            }
            ++idx;
        }
        idx_profiles[i] = hit;
        fprintf(stdout, "%4d\n", idx_profiles.at(i));
    }
}
