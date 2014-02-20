#include "upgrade.h"
#include "ui_upgrade.h"

Upgrade::Upgrade(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::Upgrade)
{
    ui->setupUi(this);
}

Upgrade::~Upgrade()
{
    delete ui;
}
