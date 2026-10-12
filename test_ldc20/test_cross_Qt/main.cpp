#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Cross Compile Test");
    window.setMinimumSize(400, 200);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    QLabel *label = new QLabel("Hello ARM64 World!\nCross-compilation successful!", &window);
    label->setAlignment(Qt::AlignCenter);
    
    // Увеличим шрифт для наглядности
    QFont font = label->font();
    font.setPointSize(16);
    font.setBold(true);
    label->setFont(font);

    layout->addWidget(label);
    window.show();

    return app.exec();
}
