#include "../include/MainWindow.h"
#include "../include/SQLiteBackend.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    // --- LEWA STRONA (Eksplorator) ---
    dbExplorer = new QTreeWidget();
    dbExplorer->setHeaderLabel("Tabele w bazie");
    dbExplorer->setMinimumWidth(200);

    // --- PRAWA STRONA (Panel operacyjny) ---
    auto* rightPanel = new QWidget();
    auto* rightLayout = new QVBoxLayout(rightPanel);

    dbTypeSelector = new QComboBox();
    dbTypeSelector->addItem("SQLite");
    dbTypeSelector->addItem("MySQL (Brak implementacji)");

    // Rozdzielone przyciski
    openBtn = new QPushButton("Otwórz istniejącą bazę");
    createBtn = new QPushButton("Utwórz nową bazę");

    queryInput = new QTextEdit();
    queryInput->setPlaceholderText("Wpisz komendę SQL tutaj...");
    executeBtn = new QPushButton("Wykonaj SQL");
    resultView = new QTableView();

    queryModel = new QSqlQueryModel(this);
    resultView->setModel(queryModel);

    // Dodanie elementów do prawego layoutu
    rightLayout->addWidget(dbTypeSelector);
    rightLayout->addWidget(openBtn);    // Dodano
    rightLayout->addWidget(createBtn);  // Dodano
    rightLayout->addWidget(queryInput);
    rightLayout->addWidget(executeBtn);
    rightLayout->addWidget(resultView);

    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(dbExplorer);
    splitter->addWidget(rightPanel);
    mainLayout->addWidget(splitter);

    setCentralWidget(centralWidget);
    resize(1000, 650);

    connect(openBtn, &QPushButton::clicked, this, &MainWindow::handleOpenDatabase);
    connect(createBtn, &QPushButton::clicked, this, &MainWindow::handleCreateDatabase);
    connect(executeBtn, &QPushButton::clicked, this, &MainWindow::handleExecuteQuery);

    setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("Universal Database Manager");
}
MainWindow::~MainWindow() = default;

void MainWindow::handleConnect() {
    if (dbTypeSelector->currentText() == "SQLite") {
        dbBackend = std::make_unique<SQLiteBackend>();

        QString fileName = QFileDialog::getSaveFileName(this, "Wybierz lokalizację bazy SQLite");
        if (!fileName.isEmpty()) {
            if (dbBackend->connectToDatabase(fileName)) {
                QMessageBox::information(this, "Sukces", "Połączono z bazą SQLite!");

                // --- DODANO ---
                // Po udanym połączeniu wywołujemy odświeżenie listy tabel w panelu bocznym
                refreshTableList();
            }
        }
    } else {
        QMessageBox::warning(this, "Brak wsparcia", "Ten silnik nie jest jeszcze zaimplementowany.");
    }
}

void MainWindow::refreshTableList() const {
    dbExplorer->clear(); // Czyścimy listę na wypadek, gdybyśmy łączyli się z kolejną bazą
    if (!dbBackend) return;

    QStringList tables = dbBackend->getTables();

    for (const QString& tableName : tables) {
        // Tworzymy nowy "liść" w drzewie dla każdej nazwy tabeli
        auto* item = new QTreeWidgetItem(dbExplorer);
        item->setText(0, tableName);
        item->setIcon(0, QIcon(":/icon.ico")); // Dla ozdoby używamy Twojej ikony
    }
}

void MainWindow::handleExecuteQuery() {
    if (!dbBackend) {
        QMessageBox::warning(this, "Błąd", "Najpierw połącz się z bazą!");
        return;
    }

    QString sql = queryInput->toPlainText();
    QSqlQuery result = dbBackend->executeRawCommand(sql);

    queryModel->setQuery(std::move(result));
}

void MainWindow::handleOpenDatabase() {
    if (dbTypeSelector->currentText() == "SQLite") {
        dbBackend = std::make_unique<SQLiteBackend>();

        // getOpenFileName wymusza wybór pliku, który JUŻ ISTNIEJE
        QString fileName = QFileDialog::getOpenFileName(this, "Otwórz bazę SQLite", "", "Bazy danych (*.db *.sqlite);;Wszystkie pliki (*.*)");

        if (!fileName.isEmpty()) {
            if (dbBackend->connectToDatabase(fileName)) {
                QMessageBox::information(this, "Sukces", "Otwarto istniejącą bazę!");
                refreshTableList();
            }
        }
    }
}

void MainWindow::handleCreateDatabase() {
    if (dbTypeSelector->currentText() == "SQLite") {
        dbBackend = std::make_unique<SQLiteBackend>();

        // getSaveFileName pozwala wpisać nazwę NOWEGO pliku
        QString fileName = QFileDialog::getSaveFileName(this, "Stwórz nową bazę SQLite", "", "Bazy danych (*.db *.sqlite)");

        if (!fileName.isEmpty()) {
            // W SQLite połączenie z nieistniejącym plikiem automatycznie go tworzy
            if (dbBackend->connectToDatabase(fileName)) {
                QMessageBox::information(this, "Sukces", "Utworzono nową bazę danych!");
                refreshTableList();
            }
        }
    }
}