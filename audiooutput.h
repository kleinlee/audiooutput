#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <math.h>

#include <QAudioSink>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QLabel>
#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QScopedPointer>
#include <QMediaDevices>

class Generator : public QIODevice
{
    Q_OBJECT

public:
    Generator(const QAudioFormat &format);

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    qint64 size() const override { return m_buffer.size(); }

private:
    qint64 m_pos = 0;
    QByteArray m_buffer;
};

class AudioTest : public QMainWindow
{
    Q_OBJECT

public:
    AudioTest();
    ~AudioTest();

private:
    void initializeWindow();
    void initializeAudio(const QAudioDevice &deviceInfo);

private:
    QMediaDevices *m_devices = nullptr;
    QTimer *m_pushTimer = nullptr;

    QScopedPointer<Generator> m_generator;
    QScopedPointer<QAudioSink> m_audioOutput;

    bool m_pullMode = true;

private slots:
    void toggleMode();
    void volumeChanged(int);
};

#endif // AUDIOOUTPUT_H
