#ifndef TESTENCRYPTION_H
#define TESTENCRYPTION_H

#include <QObject>

class TestEncryption : public QObject
{
    Q_OBJECT
public:
    explicit TestEncryption(QObject *parent = nullptr);

private slots:
    void testBasicEncryption();
    void testEuint_16();
    void testEuint_32();
    void testEuint_64();
    void testEstring();
};

#endif // TESTENCRYPTION_H
