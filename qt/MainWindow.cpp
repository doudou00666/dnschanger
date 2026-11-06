// MainWindow.cpp
#include "MainWindow.h"
#include <QAbstractItemView>
#include <QMessageBox>

MainWindow::MainWindow() {
    setWindowTitle("Changement DNS — KDE Plasma");
    resize(540, 500);

    QLabel* connLabel = new QLabel;
    connLabel->setStyleSheet("font-weight: bold; padding: 4px;");

    QLabel* dnsLabel = new QLabel;
    dnsLabel->setStyleSheet("font-family: monospace; color: #333;");

    QListWidget* list = new QListWidget;
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setStyleSheet(
        "QListWidget { font-size: 12px; }"
        "QListWidget::item:selected { background: #c4d4f4; }");

    for (uint32_t i = 0; i < dnslib_count(); ++i) {
        const QString name  = q(dnslib_name(i));
        const QString tag   = q(dnslib_tag(i));
        const QString ip    = q(dnslib_ip(i));
        const QString notes = q(dnslib_notes(i));
        const bool    usable = dnslib_usable(i) != 0;

        QListWidgetItem* item = new QListWidgetItem;
        item->setText(name + "  " + tag + "\n  IP: " + (ip.isEmpty() ? "(aucune)" : ip));
        item->setToolTip(notes);
        if (!usable)
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        list->addItem(item);
    }
    list->setCurrentRow(-1);

    QPushButton* btn = new QPushButton("Appliquer le DNS sélectionné");
    btn->setStyleSheet(
        "QPushButton { background: #2c6fd4; color: white; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background: #1a55ad; }"
        "QPushButton:disabled { background: #888; }");

    QLabel* status = new QLabel;
    status->setWordWrap(true);
    status->setStyleSheet("color: #555;");

    QLabel* legend = new QLabel(
        "Légende: ☀️ aucun filtrage  💵 pubs  👿 malware  💋 adulte  👮 non-neutre");
    legend->setStyleSheet("color: #888; font-size: 11px;");

    QVBoxLayout* v = new QVBoxLayout;
    v->addWidget(connLabel);
    v->addWidget(dnsLabel);
    v->addWidget(legend);
    v->addWidget(list, 1);
    v->addWidget(btn);
    v->addWidget(status);

    QWidget* cw = new QWidget;
    cw->setLayout(v);
    setCentralWidget(cw);

    statusBar()->showMessage("Prêt.");

    QObject::connect(list, &QListWidget::currentRowChanged, [status, list] {
        const int row = list->currentRow();
        status->setText(row >= 0 ? q(dnslib_notes(row)) : QString());
    });

    QObject::connect(btn, &QPushButton::clicked, [this, list, connLabel, dnsLabel, status, btn] {
        const int row = list->currentRow();
        if (row < 0) {
            statusBar()->showMessage("Sélectionnez un résolveur dans la liste.");
            return;
        }
        btn->setEnabled(false);
        statusBar()->showMessage("Application en cours…");
        const QString res = q(dnslib_set_dns(row));
        statusBar()->showMessage(res);
        QMessageBox::information(this, "DNS appliqué", res);
        btn->setEnabled(true);
        connLabel->setText("Connexion active: " + q(dnslib_active_connection()));
        dnsLabel->setText("DNS actuellement appliqué: " + q(dnslib_current_dns()));
        dnslib_reset_store();
    });

    connLabel->setText("Connexion active: " + q(dnslib_active_connection()));
    dnsLabel->setText("DNS actuellement appliqué: " + q(dnslib_current_dns()));
}

MainWindow::~MainWindow() {
    dnslib_reset_store();
}
