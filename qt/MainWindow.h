// MainWindow.h
#pragma once
#include <QMainWindow>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollArea>
#include <QGroupBox>
#include <QStatusBar>

extern "C" {
    extern uint32_t       dnslib_count();
    extern const char*    dnslib_name(uint32_t);
    extern const char*    dnslib_tag(uint32_t);
    extern const char*    dnslib_ip(uint32_t);
    extern const char*    dnslib_notes(uint32_t);
    extern int            dnslib_usable(uint32_t);
    extern void           dnslib_reset_store();
    extern const char*    dnslib_active_connection();
    extern const char*    dnslib_current_dns();
    extern const char*    dnslib_apply_custom(const char* ip_csv);
}

inline QString q(const char* s) {
    return s ? QString::fromUtf8(s) : QString();
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private:
    QList<QCheckBox*>  boxes;   // cases à cocher des résolveurs
    QTextEdit*         customEdit = nullptr;
    QLabel*            status = nullptr;
};
