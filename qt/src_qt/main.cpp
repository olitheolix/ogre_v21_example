/* MIT License

Copyright (c) 2016 Oliver Nagy

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include "OgreDemoQt.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Retina display support for Mac OS, iOS and X11:
  // http://blog.qt.io/blog/2013/04/25/retina-display-support-for-mac-os-ios-and-x11/
  //
  // AA_UseHighDpiPixmaps attribute is off by default in Qt 5.1 but will most
  // likely be on by default in a future release of Qt.
  //    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

  QIcon appIcon;
  appIcon.addFile(":/Icons/AppIcon32");
  appIcon.addFile(":/Icons/AppIcon128");
  app.setWindowIcon(appIcon);

  QWidget window;
  window.resize(640, 480);
  window.setWindowTitle
    (QApplication::translate("childwidget", "Ogre 2.1: Shadow Map Demo (Qt5)"));

  OgreDemoQt* ogreWindow = new OgreDemoQt();
  QWidget* ogreWidget = QWidget::createWindowContainer(ogreWindow);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(ogreWidget);
  window.setLayout(layout);
  window.show();

  return app.exec();
}
