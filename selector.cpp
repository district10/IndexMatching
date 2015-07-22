#include "selector.h"
#include "ui_selector.h"

Selector::Selector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Selector)
{
    ui->setupUi(this);

    // ui
    view_poses = ui->treeView_pose;
    view_profiles = ui->treeView_profile;
}

Selector::~Selector()
{
    delete ui;
}

void Selector::on_pushButton_profile_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                "Select PROFILE file to open",
                                                "D:/tzx/git/IndexMatching/data",
                                                "POSE file (*.txt)");
    if (path.isEmpty()) {
        return;
    }

    loadProfileFile(path.toStdString().c_str(), profiles);
    initViewProfiles(view_profiles, model_profiles, profiles);
}

void Selector::on_pushButton_pose_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                "Select POSE file to open",
                                                "D:/tzx/git/IndexMatching/data",
                                                "POSE file (*.txt)");
    if (path.isEmpty()) {
        return;
    }

    loadPoseFile(path.toStdString().c_str(), poses);
    initViewPoses(view_poses, model_poses, poses);
}

void Selector::on_pushButton_idxmat_clicked()
{
    indexmatching(poses, profiles, idx_poses, idx_profiles);
}

void Selector::initViewPoses(QTreeView *view, QStandardItemModel *model, std::vector<pose_t> &poses)
{
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels( QStringList() << "Poses:" );
    for (int i = 0; i < poses.size(); ++i) {
        QStandardItem *item     = new QStandardItem(QString("").sprintf("%lld", poses.at(i).timestamp));
        QStandardItem *motornum = new QStandardItem(QString("").sprintf("%d"  , poses.at(i).motornum));
        QStandardItem *roll     = new QStandardItem(QString("").sprintf("%lf" , poses.at(i).roll));
        QStandardItem *pitch    = new QStandardItem(QString("").sprintf("%lf" , poses.at(i).pitch));
        QStandardItem *yaw      = new QStandardItem(QString("").sprintf("%lf" , poses.at(i).yaw));
        item->appendRow(motornum);
        item->appendRow(roll);
        item->appendRow(yaw);
        item->appendRow(pitch);
        model->appendRow(item);

    }
    view->setModel(model);
}

void Selector::initViewProfiles(QTreeView *view, QStandardItemModel *model, std::vector<profile_t> &profiles)
{
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels( QStringList() << "Profiles:" );
    for (int i = 0; i < profiles.size(); ++i) {
        QStandardItem *item  = new QStandardItem(QString("").sprintf("%lld", profiles.at(i).timestamp));
        for (int j = 0; j < profiles.at(i).pts.size(); ++j) {
            QStandardItem *pt = new QStandardItem( QString("").sprintf( "x: %+10.5lf z: %+10.5lf",
                                                                        profiles.at(i).pts.at(j).x,
                                                                        profiles.at(i).pts.at(j).z ));
            item->appendRow(pt);
        }
        model->appendRow(item);
    }
    view->setModel(model);
}


void Selector::on_treeView_profile_clicked(const QModelIndex &index)
{
    // TODO:
    // <profile index> -------> <idx_profiles[idx]> ------------> <pose index>
    // the question is:
    // ?               how to select <pose index>?
    /*
     * I tried:
     *  view_poses->currentIndex().row();
     *  view_poses->setSelectionBehavior(QAbstractItemView::SelectColumns);
     *  view_poses->setSelectionMode(QAbstractItemView::MultiSelection);
     *  view_poses->setSelectionMode(QAbstractItemView::NoSelection);
     *  view_poses->visualRect(index);
     *  view_profiles->scrollToTop();
     *  view_profiles->setAlternatingRowColors(true);
     *  view_profiles->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
     */
}

void Selector::on_treeView_pose_clicked(const QModelIndex &index)
{
}
