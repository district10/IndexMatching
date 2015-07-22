#ifndef SELECTOR_H
#define SELECTOR_H

#include <QDialog>
#include <QDebug>
#include <QFileDialog>
#include <QTreeView>
#include <QStandardItemModel>
#include <vector>
#include <stdio.h>
#include <iostream>
#include "indexmatching.h"

namespace Ui {
class Selector;
}

class Selector : public QDialog
{
    Q_OBJECT

public:
    explicit Selector(QWidget *parent = 0);
    ~Selector();

private slots:
    void on_pushButton_profile_clicked();
    void on_pushButton_pose_clicked();
    void on_pushButton_idxmat_clicked();

    void initViewPoses(QTreeView *view, QStandardItemModel *model, std::vector<pose_t> &poses);
    void initViewProfiles(QTreeView *view, QStandardItemModel *model, std::vector<profile_t> &profiles);

    void on_treeView_profile_clicked(const QModelIndex &index);
    void on_treeView_pose_clicked(const QModelIndex &index);

private:
    Ui::Selector *ui;
    std::vector<profile_t> profiles;
    std::vector<pose_t> poses;
    std::vector<int> idx_profiles;
    std::vector<int> idx_poses;

    QTreeView *view_poses;
    QTreeView *view_profiles;

    QStandardItemModel *model_poses;
    QStandardItemModel *model_profiles;
};

#endif // SELECTOR_H
