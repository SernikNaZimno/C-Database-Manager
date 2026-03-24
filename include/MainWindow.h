#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QComboBox>
#include <QVBoxLayout>
#include <QSqlQueryModel>
#include <memory>
#include "IDatabaseBackend.h"
#include <QTreeWidget>
#include <QSplitter>

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    std::unique_ptr<IDatabaseBackend> dbBackend;
    QSqlQueryModel* queryModel;

    // Elementy UI
    QComboBox* dbTypeSelector;
    QPushButton* openBtn;   // Przycisk do otwierania istniejącego pliku
    QPushButton* createBtn; // Przycisk do tworzenia nowego pliku
    QTextEdit* queryInput;
    QPushButton* executeBtn;
    QTableView* resultView;
    QTreeWidget* dbExplorer;

    void refrehTableList();

private slots:
    void handleConnect();

    void refreshTableList() const;

    void handleExecuteQuery();

    void handleOpenDatabase();   // Nowy slot dla otwierania

    void handleCreateDatabase(); // Nowy slot dla tworzenia

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif