#include "audiooutput.h"

#include <QAudioDevice>
#include <QAudioSink>
#include <QDebug>
#include <QVBoxLayout>
#include <qmath.h>
#include <qendian.h>
#include <QFile>
Generator::Generator(const QAudioFormat &format)
{
    if (format.isValid())
    {
        qDebug() << format;

        QFile file(":/2333.raw");
        file.open(QIODevice::ReadOnly);
        m_buffer = file.readAll();
        file.close();
    }
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    close();
}

qint64 Generator::readData(char *data, qint64 len)
{
    qint64 total = 0;
    if (!m_buffer.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
            m_pos = (m_pos + chunk) % m_buffer.size();
            total += chunk;
        }
    }
    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

AudioTest::AudioTest()
    : m_devices(new QMediaDevices(this)),
      m_pushTimer(new QTimer(this))
{
    initializeWindow();
    initializeAudio(m_devices->defaultAudioOutput());
}

AudioTest::~AudioTest()
{
    m_pushTimer->stop();
}

void AudioTest::initializeWindow()
{
}

void AudioTest::initializeAudio(const QAudioDevice &deviceInfo)
{
    QAudioFormat format = deviceInfo.preferredFormat();

    format.setChannelCount(1);
    format.setSampleRate(16000);
    m_generator.reset(new Generator(format));
    m_audioOutput.reset(new QAudioSink(deviceInfo, format));
    m_generator->start();

    toggleMode();
}

void AudioTest::toggleMode()
{
    m_pushTimer->stop();
    m_audioOutput->stop();

    auto io = m_audioOutput->start();
    m_pushTimer->disconnect();

    connect(m_pushTimer, &QTimer::timeout, [this, io]() {
        if (m_audioOutput->state() == QAudio::StoppedState)
            return;

        int len = m_audioOutput->bytesFree();
        QByteArray buffer(len, 0);
        len = m_generator->read(buffer.data(), len);
        if (len)
           io->write(buffer.data(), len);
    });

    m_pushTimer->start(10);
}

