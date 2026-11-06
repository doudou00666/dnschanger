// MainWindow.cpp
#include "MainWindow.h"
#include <QAbstractItemView>
#include <QMessageBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QItemSelectionModel>
#include <QListWidgetItem>

MainWindow::MainWindow() {
    setWindowTitle("Changement DNS — KDE Plasma");
    resize(620, 560);

    QLabel* connLabel = new QLabel;
    connLabel->setStyleSheet("font-weight: bold; padding: 4px;");

    QLabel* dnsLabel = new QLabel;
    dnsLabel->setStyleSheet("font-family: monospace; color: #333;");

    QListWidget* list = new QListWidget;
    list->setSelectionMode(QAbstractItemView::MultiSelection);
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

    QLabel* customHint = new QLabel("DNS personnalisés (optionnel) — IP IPv4/IPv6 séparées par des virgules :");
    customHint->setStyleSheet("color: #555; padding-top: 6px;");

    QTextEdit* customEdit = new QTextEdit;
    customEdit->setStyleSheet("font-family: monospace;");
    customEdit->setPlaceholderText("ex: 1.1.1.1, 8.8.8.8  (ou 262d:22:70e7::1)");
    customEdit->setMaximumHeight(90);

    QPushButton* btn = new QPushButton("Valider et appliquer le DNS");
    btn->setStyleSheet(
        "QPushButton { background: #2c6fd4; color: white; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background: #1a55ad; }"
        "QPushButton:disabled { background: #888; }");

    QPushButton* btnClear = new QPushButton("Effacer la sélection");
    btnClear->setStyleSheet("QPushButton { padding: 4px; }");

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
    v->addWidget(customHint);
    v->addWidget(customEdit);
    QHBoxLayout* h = new QHBoxLayout;
    h->addWidget(btn);
    h->addWidget(btnClear);
    h->addStretch();
    v->addLayout(h);
    v->addWidget(status);

    QWidget* cw = new QWidget;
    cw->setLayout(v);
    setCentralWidget(cw);

    statusBar()->showMessage("Prêt. Sélectionnez un ou plusieurs résolveurs (maillage possible) et/ou saisissez vos DNS personnalisés, puis validez.");

    QObject::connect(list, &QListWidget::currentRowChanged, [status, list] {
        const int row = list->currentRow();
        status->setText(row >= 0 ? q(dnslib_notes(row)) : QString());
    });

    QObject::connect(btn, &QPushButton::clicked, [this, list, customEdit, connLabel, dnsLabel, status, btn] {
        const QString joined = [list, customEdit]() {
            QList<QListWidgetItem*> sel = list->selectedItems();
            QStringList merged;
            for (QListWidgetItem* it : sel) {
                const QString text = it->text();
                const int idx = text.indexOf("IP: ");
                if (idx >= 0) {
                    const QString ipline = text.mid(idx + 4).trimmed();
                    if (ipline.contains("aucune")) continue;
                    const QStringList parts = ipline.split(",");
                    for (const QString& p : parts) {
                        const QString t = p.trimmed();
                        if (!t.isEmpty()) merged << t;
                    }
                }
            }
            const QString custom = customEdit->toPlainText().trimmed();
            if (!custom.isEmpty()) {
                const QStringList parts = custom.split(",");
                for (const QString& p : parts) {
                    const QString t = p.trimmed();
                    if (!t.isEmpty()) merged << t;
                }
            }
            return merged.join(",");
        }();

        if (joined.isEmpty()) {
            QMessageBox::warning(this, "Pas de DNS",
                "Sélectionnez au moins un résolveur et/ou saisissez des DNS personnalisés.");
            return;
        }

        btn->setEnabled(false);
        statusBar()->showMessage("Application en cours…");

        const char* cptr = joined.toUtf8().constData();
        const QString res = q(dnslib_apply_custom(cptr));

        statusBar()->showMessage(res);
        QMessageBox::information(this, "DNS appliqué", res + "\n\nListe appliquée:\n" + joined);
        btn->setEnabled(true);

        connLabel->setText("Connexion active: " + q(dnslib_active_connection()));
        dnsLabel->setText("DNS actuellement appliqué: " + q(dnslib_current_dns()));
    });

    QObject::connect(btnClear, &QPushButton::clicked, [list, customEdit] {
        list->clearSelection();
        customEdit->clear();
    });

    connLabel->setText("Connexion active: " + q(dnslib_active_connection()));
    dnsLabel->setText("DNS actuellement appliqué: " + q(dnslib_current_dns()));
}

MainWindow::~MainWindow() {
    dnslib_reset_store();
}
