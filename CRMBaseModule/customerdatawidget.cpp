#include <QTableWidgetItem>
#include <QMessageBox>

#include "customerdatawidget.h"
#include "ui_customerdatawidget.h"
#include "customer.h"
#include "customerdialog.h"
#include "cdialogmode.h"

using namespace CRMUi;

CustomerDataWidget::CustomerDataWidget(QWidget *parent, QJsonDocument *doc) :
    AbstractDataTable(parent),
    ui(new Ui::CustomerDataWidget)
{
    ui->setupUi(this);
    cJsonWorker = CRMCommons::CJsonWorker::getInstance();

    ui->customerDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->customerDataTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->customerDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for (int i = 0; i< ui->customerDataTable->horizontalHeader()->count(); i++) {
        ui->customerDataTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    connect(ui->customerDataTable, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(onTableDataModelClicked(QModelIndex)));

    connect(ui->customerDataTable, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(customContextMenuRequested(QPoint)));

    connect(cJsonWorker, SIGNAL(onCustomerSavedSignal(CRMModels::Customer*)), this,
            SLOT(onCustomerSavedSlot(CRMModels::Customer*)));

    connect(cJsonWorker, SIGNAL(onCustomerUpdatedSignal(CRMModels::Customer*)), this,
            SLOT(onCustomerUpdatedSlot(CRMModels::Customer*)));

    connect(cJsonWorker, SIGNAL(onCustomerDeletedSignal(QNetworkReply *)), this,
            SLOT(onCustomerDeletedSlot(QNetworkReply *)));

    connect(cJsonWorker, SIGNAL(onCustomerGetByIdSignal(CRMModels::Customer *)), this,
            SLOT(onCustomerGetByIdSlot(CRMModels::Customer*)));

    int i = 0;
    foreach (QJsonValue topLevelVal, doc->array()) {
        QJsonObject topLevelObject = topLevelVal.toObject();
        qDebug() << topLevelObject;
        qDebug() << topLevelObject.value(JSON_CSTMR_FULLNAME);

        ui->customerDataTable->insertRow(i);
        ui->customerDataTable->setItem(i,0, new QTableWidgetItem(topLevelObject.value(JSON_CSTMR_ID).toVariant().toString()));
        ui->customerDataTable->setItem(i,1, new QTableWidgetItem(topLevelObject.value(JSON_CSTMR_FULLNAME).toString()));
        ui->customerDataTable->setItem(i,2, new QTableWidgetItem(topLevelObject.value(JSON_CSTMR_TYPEID).toVariant().toString()));
        ui->customerDataTable->setItem(i,3, new QTableWidgetItem(topLevelObject.value(JSON_CSTMR_EMAIL).toString()));
        ui->customerDataTable->setItem(i,4, new QTableWidgetItem(topLevelObject.value(JSON_CSTMR_PHONE).toString()));
        ui->customerDataTable->setItem(i,5, new QTableWidgetItem(topLevelObject.value(JSON_CSTMR_SOURCE).toString()));
        ui->customerDataTable->setItem(i,6, new QTableWidgetItem(topLevelObject.value(JSON_CSTMR_FAX).toString()));

        i++;
    }
}

CustomerDataWidget::~CustomerDataWidget()
{
    delete ui;
}

void CustomerDataWidget::onTableDataModelClicked(const QModelIndex &index)
{
    qDebug() << "Click on customer data table...";
}

void CustomerDataWidget::customContextMenuRequested(const QPoint &point)
{
    qDebug() << "Context menu...";
    qDebug() << "Selected row index: " << ui->customerDataTable->currentIndex().row();
    contextMenu->popup(ui->customerDataTable->viewport()->mapToGlobal(point));
}

void CustomerDataWidget::createNewTriggeredSlot()
{
    qDebug() << "Click on create new action";
    CustomerDialog *customerDialog = new CustomerDialog(CRMCommons::CRTMODE, 0, this);
    customerDialog->setModal(true);
    customerDialog->exec();
}

void CustomerDataWidget::editSelectedTriggeredSlot()
{
    qDebug() << "Click on edit selected item";
    int selectedRow = ui->customerDataTable->currentIndex().row();
    if (selectedRow >= 0) {
        QString customerId = ui->customerDataTable->item(selectedRow, 0)->data(0).toString();
        cJsonWorker->getCustomerById(customerId);
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("There are no selected rows"));
    }
}

void CustomerDataWidget::deleteSelectedTriggeredSlot()
{
    qDebug() << "Click on delete selected item";
    int selectedRow = ui->customerDataTable->currentIndex().row();
    if (selectedRow >= 0) {
        QString customerId = ui->customerDataTable->item(selectedRow, 0)->data(0).toString();
        cJsonWorker->deleteCustomer(customerId.toInt());
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("There are no selected rows"));
    }
}

void CustomerDataWidget::onCustomerSavedSlot(CRMModels::Customer *customer)
{
    qDebug() << "Adding new customer to datatabel row";
    int lastRow = ui->customerDataTable->rowCount();
    ui->customerDataTable->insertRow(lastRow);
    ui->customerDataTable->setItem(lastRow,0, new QTableWidgetItem(QString::number(customer->getId())));
    ui->customerDataTable->setItem(lastRow,1, new QTableWidgetItem(customer->getFullName()));
    //ui->customerDataTable->setItem(lastRow,2, new QTableWidgetItem(QString(customer->getTypeId())));
    ui->customerDataTable->setItem(lastRow,3, new QTableWidgetItem(customer->getEmail()));
    ui->customerDataTable->setItem(lastRow,4, new QTableWidgetItem(customer->getPhone()));
    ui->customerDataTable->setItem(lastRow,5, new QTableWidgetItem(customer->getSource()));
    ui->customerDataTable->setItem(lastRow,6, new QTableWidgetItem(customer->getFax()));
}

void CustomerDataWidget::onCustomerUpdatedSlot(CRMModels::Customer *customer)
{
    qDebug() << "Update customer";
    int selectedRow = ui->customerDataTable->currentIndex().row();
    ui->customerDataTable->item(selectedRow, 1)->setData(0, customer->getFullName());
    ui->customerDataTable->item(selectedRow, 3)->setData(0, customer->getEmail());
    ui->customerDataTable->item(selectedRow, 4)->setData(0, customer->getPhone());
    ui->customerDataTable->item(selectedRow, 5)->setData(0, customer->getSource());
    ui->customerDataTable->item(selectedRow, 6)->setData(0, customer->getFax());
}

void CustomerDataWidget::onCustomerDeletedSlot(QNetworkReply *reply)
{
    ui->customerDataTable->removeRow(ui->customerDataTable->currentRow());
    qDebug() << "Remove customer from customer datatable";
}

void CustomerDataWidget::onCustomerGetByIdSlot(CRMModels::Customer *customer)
{
    CustomerDialog *customerDialog = new CustomerDialog(CRMCommons::DialogMode::EDITMODE, customer, this);
    customerDialog->setModal(true);
    customerDialog->exec();
}
