#include "imageprovider.h"

ImageProvider::ImageProvider(): QQuickImageProvider(QQuickImageProvider::Image)
{

}

ImageProvider::~ImageProvider()
{

}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    return m_img;
}

void ImageProvider::setImage(QImage &img)
{
    m_img = img;
}
