// MainWindow.cpp — GUI avec cases à cocher (checkboxes) pour chaque résolveur
#include "MainWindow.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QCheckBox>
#include <QMessageBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

MainWindow::MainWindow() {
    setWindowTitle("Changement DNS — KDE Plasma");
    resize(640, 600);

    QLabel* connLabel = new QLabel;
    connLabel->setStyleSheet("font-weight: bold; padding: 4px;");

    QLabel* dnsLabel = new QLabel;
    dnsLabel->setStyleSheet("font-family: monospace; color: #333;");

    // ---- Grille de cases à cocher -------------------------------
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameStyle(QFrame::Panel);

    QWidget* gridHost = new QWidget;
    QGridLayout* grid  = new QGridLayout(gridHost);
    grid->setSpacing(6);
    grid->setContentsMargins(10, 10, 10, 10);

    for (uint32_t i = 0; i < dnslib_count(); ++i) {
        const QString name  = q(dnslib_name(i));
        const QString tag   = q(dnslib_tag(i));
        const QString ip    = q(dnslib_ip(i));
        const QString notes = q(dnslib_notes(i));
        const bool    usable = dnslib_usable(i) != 0;

        QCheckBox* cb = new QCheckBox(name + "  " + tag);
        cb->setToolTip(notes + "\nIP: " + (ip.isEmpty() ? QString("(aucune)") : ip));
        cb->setStyleSheet(
            "QCheckBox { padding: 2px; }"
            "QCheckBox::indicator { width: 16px; height: 16px; }");
        if (!usable) cb->setEnabled(false);
        boxes.push_back(cb);
        grid->addWidget(cb, (i / 2), (i % 2));   // 2 colonnes
    }
    scroll->setWidget(gridHost);

    QLabel* customHint = new QLabel("DNS personnalisés (optionnel) — IP IPv4/IPv6 séparées par des virgules :");
    customHint->setStyleSheet("color: #555; padding-top: 4px;");

    customEdit = new QTextEdit;
    customEdit->setStyleSheet("font-family: monospace;");
    customEdit->setPlaceholderText("ex: 1.1.1.1, 8.8.8.8  (ou 262d:22:70e7::1)");
    customEdit->setMaximumHeight(80);

    QPushButton* btn = new QPushButton("Valider et appliquer le DNS");
    btn->setStyleSheet(
        "QPushButton { background: #2c6fd4; color: white; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background: #1a55ad; }"
        "QPushButton:disabled { background: #888; }");

    QPushButton* btnClear = new QPushButton("Tout décocher");
    btnClear->setStyleSheet("QPushButton { padding: 4px; }");

    QPushButton* btnReset = new QPushButton("Reset DNS");
    btnReset->setStyleSheet(
        "QPushButton { background: #e67e22; color: white; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background: #d35400; }"
        "QPushButton:disabled { background: #888; }");
    btnReset->setToolTip("Réinitialise le DNS de la carte réseau à la configuration par défaut (DHCP).");

    status = new QLabel;
    status->setWordWrap(true);
    status->setStyleSheet(
        "QLabel { color: #2ecc7a; font-weight: bold; font-size: 14px; padding: 6px; "
        "background: rgba(46, 204, 122, 0.12); border: 1px solid rgba(46, 204, 122, 0.3); border-radius: 4px; }");

    QLabel* legend = new QLabel(
        "Légende: ☀️ aucun filtrage  💵 pubs  👿 malware  💋 adulte  👮 non-neutre");
    legend->setStyleSheet("color: #888; font-size: 11px;");

    // ---- Assemblage vertical -----------------------------------
    QVBoxLayout* v = new QVBoxLayout;
    v->addWidget(connLabel);
    v->addWidget(dnsLabel);
    v->addWidget(legend);
    v->addWidget(scroll, 1);
    v->addWidget(customHint);
    v->addWidget(customEdit);

    QHBoxLayout* h = new QHBoxLayout;
    h->addWidget(btn);
    h->addWidget(btnClear);
    h->addWidget(btnReset);
    h->addStretch();
    v->addLayout(h);
    v->addWidget(status);

    // ---- Version en bas à droite ---------------------------------
    QLabel* verLabel = new QLabel("dnschanger v0.1.6");
    verLabel->setStyleSheet("color: #888; font-size: 11px; padding: 2px;");
    verLabel->setAlignment(Qt::AlignRight);
    v->addWidget(verLabel, 0, Qt::AlignRight);

    QWidget* cw = new QWidget;
    cw->setLayout(v);
    setCentralWidget(cw);

    statusBar()->showMessage("Cochez un ou plusieurs résolveurs, saisissez éventuellement des DNS personnalisés, puis validez.");

    // ---- Logique de validation ----------------------------------
    QObject::connect(btn, &QPushButton::clicked, [this, btn, connLabel, dnsLabel] {
        QStringList merged;
        for (QCheckBox* cb : boxes) {
            if (cb->isChecked()) {
                // le texte du checkbox est « name  tag » ; on reprend l'index
                const uint32_t idx = boxes.indexOf(cb);
                const QString ip = q(dnslib_ip(idx));
                const QStringList parts = ip.split(",");
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

        if (merged.isEmpty()) {
            QMessageBox::warning(this, "Pas de DNS",
                "Cochez au moins un résolveur et/ou saisissez des DNS personnalisés.");
            return;
        }

        btn->setEnabled(false);

        const QString joined = merged.join(",");
        const char* cptr = joined.toUtf8().constData();
        const QString res = q(dnslib_apply_custom(cptr));

        status->setText(res);
        btn->setEnabled(true);

        connLabel->setText("Connexion active: " + q(dnslib_active_connection()));
        dnsLabel->setText("DNS actuellement appliqué: " + q(dnslib_current_dns()));
        status->setText(res);
    });

    QObject::connect(btnClear, &QPushButton::clicked, [this] {
        for (QCheckBox* cb : boxes) cb->setChecked(false);
        customEdit->clear();
    });

    QObject::connect(btnReset, &QPushButton::clicked, [this, btnReset, connLabel, dnsLabel] {
        btnReset->setEnabled(false);
        const QString res = q(dnslib_reset_dns());
        status->setText(res);
        btnReset->setEnabled(true);
        connLabel->setText("Connexion active: " + q(dnslib_active_connection()));
        dnsLabel->setText("DNS actuellement appliqué: " + q(dnslib_current_dns()));
    });

    // Compteur dynamique — met à jour le texte de statut quand on coche/décoche
    const auto updateCount = [this] {
        int n = 0;
        for (QCheckBox* cb : boxes) if (cb->isChecked()) ++n;
        status->setText(n > 0 ? QString("%1 résolveur(s) coché(s).").arg(n)
                              : "Aucun résolveur coché.");
    };
    for (QCheckBox* cb : boxes)
        QObject::connect(cb, &QCheckBox::toggled, [=]() { updateCount(); });
    updateCount();

    connLabel->setText("Connexion active: " + q(dnslib_active_connection()));
    dnsLabel->setText("DNS actuellement appliqué: " + q(dnslib_current_dns()));
}

MainWindow::~MainWindow() {
    dnslib_reset_store();
}
